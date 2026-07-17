/*
 * MajordomusControl - Home Automation Gateway
 * Driver for the Majordomus RoomIR device (RS-485).
 * Sensor: MLX90642 infrared thermal camera (32×24 = 768 pixels).
 *
 * The IR image is transmitted in 6 base64-encoded blocks of 128 pixels:
 *   id:X,offset:N,seq:S,tmin:A,tmax:B,irData:<base64>,crc:XX
 * Pixels are normalized to 0..255 over the scene range; tmin/tmax are in
 * 0.01 °C units. Decoding: T = (tmin + px*(tmax-tmin)/255) / 100 °C.
 * Blocks of one frame share the same seq (0-9); the frame is published
 * once all 6 blocks of the current seq have been received.
 *
 * Image processing (configured per device in config.xml <irConfig>):
 *  - person detection via clustering (connected components in human temp band)
 *  - presence detection in up to 4 rectangular zones -> tele/zone<i>Presence
 *  - fire detection (max temp above threshold)       -> tele/fireAlarm + tele/maxTemperature
 *  - PNG heatmap for HA MQTT camera                  -> tele/irImagePng
 *
 * Copyright (C) 2024  Ing. Jiří Husák
 * Author:  Ing. Jiří Husák
 * Contact: info@majordomus.tech
 * Website: www.majordomus.tech
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
package Devices;

import Configuration.ConfXmlObject;
import Configuration.Configuration;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Base64;
import java.util.Collections;
import java.util.List;
import javax.imageio.ImageIO;

/**
 *
 * @author husak
 */
public class RoomIR extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;
    private List<Boolean> setCntChanged = new ArrayList<>(Collections.nCopies(8, false));

    static final int IR_WIDTH  = 32;
    static final int IR_HEIGHT = 24;
    static final int MAX_ZONES = 4;
    static final int IR_BLOCK_PIXELS = 128;
    static final int IR_BLOCKS = 6;
    static final int IR_ALL_BLOCKS_MASK = (1 << IR_BLOCKS) - 1;
    final int IrImageSize = IR_WIDTH * IR_HEIGHT;
    private final float[] irImage;

    // IR frame assembly state
    private int rxSeq = -1;        // seq of the frame being assembled
    private int rxBlocksMask = 0;  // bit per received block of current seq

    // Processing configuration (loaded from config.xml <irConfig>)
    private ConfXmlObject.IRConfig irConfig = new ConfXmlObject.IRConfig();

    // PNG publishing throttle
    private LocalDateTime lastPngPublish = LocalDateTime.MIN;
    private static final int PNG_MIN_INTERVAL_S = 2;
    private static final int PNG_SCALE = 10; // 32x24 -> 320x240

    public RoomIR(String connectionName, String name, String type) {
        super(connectionName, name, type);

        irImage = new float[IrImageSize];

        cmdList.addAll(Arrays.asList("reboot"));
        infList.addAll(Arrays.asList("version", "irImg", "maxT", "minT", "fire", "pCount"));

        for (String s : cmdList) {
            propertyMap.put(s, new DeviceProperty());
        }

        applyIrConfig();
    }

    // -------------------------------------------------------------------------
    // Load IR processing settings for this device from config.xml
    // Called from constructor and after the web UI saves new settings.
    // -------------------------------------------------------------------------
    public final synchronized void applyIrConfig() {
        irConfig = new ConfXmlObject.IRConfig();
        for (ConfXmlObject.Device d : Configuration.getXMLObj().getDevices()) {
            if (name.equals(d.name) && d.irConfig != null) {
                irConfig = d.irConfig;
                break;
            }
        }

        // Expose zone presence properties in the web UI for configured zones
        for (int i = 0; i < MAX_ZONES; i++) {
            String key = "zp" + i;
            if (i < irConfig.zones.size() && irConfig.zones.get(i).enabled) {
                if (!infList.contains(key)) {
                    infList.add(key);
                }
            } else {
                infList.remove(key);
            }
        }
    }

    public ConfXmlObject.IRConfig getIrConfig() {
        return irConfig;
    }

    // -------------------------------------------------------------------------
    // Incoming data from RS-485 device
    // -------------------------------------------------------------------------

    // IR block keys are handled atomically in recvMsg() (the HashMap iteration
    // order in the generic path does not follow the message order, so irData
    // could otherwise be processed before its offset/tmin/tmax).
    @Override
    public void recvMsg(java.util.HashMap<String, Object> data) {
        if (data.containsKey("irData") && data.containsKey("offset")
                && data.containsKey("tmin") && data.containsKey("tmax")) {
            try {
                processIrBlock(
                        Integer.parseInt(data.get("offset").toString()),
                        data.containsKey("seq") ? Integer.parseInt(data.get("seq").toString()) : 0,
                        Integer.parseInt(data.get("tmin").toString()),
                        Integer.parseInt(data.get("tmax").toString()),
                        data.get("irData").toString());
            } catch (IllegalArgumentException e) {
                // malformed number or base64 - ignore block
            }
        }
        super.recvMsg(data);
    }

    @Override
    public synchronized void setDataByKey(String key, String data) {
        switch (key) {
            // --- state: device status ---
            case "version" -> infFromDevice("state", "version", key, data, true);

            // offset/seq/tmin/tmax/irData are consumed in recvMsg()
            default -> { }
        }
    }

    /**
     * Decodes one IR block and assembles the frame. Pixels are normalized
     * 0..255 over tmin..tmax (0.01 °C units). The frame is published once
     * all blocks with the same seq have been collected.
     */
    private synchronized void processIrBlock(int offset, int seq, int tminRaw, int tmaxRaw, String b64) {
        byte[] part = Base64.getDecoder().decode(b64);

        float tmin  = tminRaw / 100.0f;
        float scale = ((tmaxRaw - tminRaw) / 100.0f) / 255.0f;

        if (seq != rxSeq) { // new frame -> start collecting from scratch
            rxSeq = seq;
            rxBlocksMask = 0;
        }

        for (int i = 0; offset + i < IrImageSize && i < part.length; i++) {
            irImage[offset + i] = tmin + (part[i] & 0xFF) * scale;
        }

        if (offset % IR_BLOCK_PIXELS == 0 && offset / IR_BLOCK_PIXELS < IR_BLOCKS) {
            rxBlocksMask |= 1 << (offset / IR_BLOCK_PIXELS);
        }

        if (rxBlocksMask == IR_ALL_BLOCKS_MASK) {
            rxBlocksMask = 0;
            infFromDevice("tele", "irImage", "irImg", java.util.Arrays.toString(irImage), true);
            processIrImage();
        }
    }

    // -------------------------------------------------------------------------
    // IR image processing - runs once per complete frame
    // -------------------------------------------------------------------------
    private void processIrImage() {
        // --- min/max temperature & fire detection ---
        float maxTemp = irImage[0];
        float minTemp = irImage[0];
        for (float t : irImage) {
            if (t > maxTemp) maxTemp = t;
            if (t < minTemp) minTemp = t;
        }
        infFromDevice("tele", "maxTemperature", "maxT",
                (float) (Math.round(maxTemp * 10.0) / 10.0), true);
        infFromDevice("tele", "minTemperature", "minT",
                (float) (Math.round(minTemp * 10.0) / 10.0), true);
        infFromDevice("tele", "fireAlarm", "fire",
                (maxTemp > irConfig.fireThreshold) ? 1 : 0, true);

        // --- person clusters (connected components in human temp band) ---
        // Slepene osoby v jednom clusteru rozlisuje pocitani vrcholu:
        // kazda osoba ma vlastni teplotni maximum oddelene udolim.
        List<int[]> clusterPixels = findPersonClusters();
        int personCount = 0;
        for (int[] pixels : clusterPixels) {
            personCount += countTemperaturePeaks(pixels);
        }
        infFromDevice("tele", "personCount", "pCount", personCount, true);

        // --- zone presence: any cluster pixel inside the zone rectangle ---
        for (int i = 0; i < MAX_ZONES && i < irConfig.zones.size(); i++) {
            ConfXmlObject.IRZone zone = irConfig.zones.get(i);
            if (!zone.enabled) continue;

            boolean presence = false;
            for (int[] pixels : clusterPixels) {
                for (int idx : pixels) {
                    int x = idx % IR_WIDTH;
                    int y = idx / IR_WIDTH;
                    if (x >= zone.x && x < zone.x + zone.w && y >= zone.y && y < zone.y + zone.h) {
                        presence = true;
                        break;
                    }
                }
                if (presence) break;
            }
            infFromDevice("tele", "zone" + i + "Presence", "zp" + i, presence ? 1 : 0, true);
        }

        // --- PNG heatmap for HA camera (throttled) ---
        LocalDateTime now = LocalDateTime.now();
        if (Duration.between(lastPngPublish, now).toSeconds() >= PNG_MIN_INTERVAL_S) {
            byte[] png = renderPng();
            if (png != null) {
                MQTT.MQTTinterface.getInstance().publishBytes(name + "/tele/irImagePng", png, true);
                lastPngPublish = now;
            }
        }
    }

    // Detekce osob: prah se odvozuje od pozadi sceny (median snimku + delta),
    // absolutni personMinTemp z konfigurace slouzi jen jako spodni pojistka.
    // Obleceny clovek ma povrch klidne jen 2-4 °C nad pozadim, pevny prah
    // (napr. 28 °C) osoby v chladnejsi mistnosti spolehlive prehlizel.
    private static final float PERSON_BG_DELTA = 2.0f;
    // Minimalni prominence teplotniho vrcholu, aby se pocital jako dalsi
    // osoba ve slepenem clusteru (hlava vs. rameno tehoz cloveka ma melci
    // udoli, dve hlavy vedle sebe hlubsi).
    private static final float PEAK_PROMINENCE = 1.0f;

    /** Median teplot snimku - odhad teploty pozadi (osoby zabiraji mensinu pixelu). */
    private float backgroundTemperature() {
        float[] sorted = irImage.clone();
        java.util.Arrays.sort(sorted);
        return sorted[sorted.length / 2];
    }

    /**
     * Finds clusters of pixels within the human body temperature band
     * (8-connectivity flood fill). Clusters smaller than minClusterSize
     * are discarded as noise. Returns a list of pixel-index arrays.
     */
    private List<int[]> findPersonClusters() {
        boolean[] hot     = new boolean[IrImageSize];
        boolean[] visited = new boolean[IrImageSize];

        float threshold = Math.max(backgroundTemperature() + PERSON_BG_DELTA,
                                   irConfig.personMinTemp);
        for (int i = 0; i < IrImageSize; i++) {
            hot[i] = irImage[i] >= threshold && irImage[i] <= irConfig.personMaxTemp;
        }

        List<int[]> clusters = new ArrayList<>();
        ArrayDeque<Integer> stack = new ArrayDeque<>();

        for (int start = 0; start < IrImageSize; start++) {
            if (!hot[start] || visited[start]) continue;

            List<Integer> pixels = new ArrayList<>();
            stack.push(start);
            visited[start] = true;

            while (!stack.isEmpty()) {
                int idx = stack.pop();
                pixels.add(idx);
                int x = idx % IR_WIDTH;
                int y = idx / IR_WIDTH;

                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx < 0 || nx >= IR_WIDTH || ny < 0 || ny >= IR_HEIGHT) continue;
                        int nIdx = ny * IR_WIDTH + nx;
                        if (hot[nIdx] && !visited[nIdx]) {
                            visited[nIdx] = true;
                            stack.push(nIdx);
                        }
                    }
                }
            }

            if (pixels.size() >= irConfig.minClusterSize) {
                clusters.add(pixels.stream().mapToInt(Integer::intValue).toArray());
            }
        }

        return clusters;
    }

    /**
     * Counts persons in one cluster as the number of temperature peaks with
     * sufficient prominence (persistence flooding): pixels are processed from
     * hottest to coldest, each new island is a candidate peak; when two
     * islands meet at a saddle, the lower peak counts as a separate person
     * only if it rises at least PEAK_PROMINENCE above the saddle.
     */
    private int countTemperaturePeaks(int[] pixels) {
        Integer[] order = new Integer[pixels.length];
        for (int i = 0; i < pixels.length; i++) order[i] = pixels[i];
        java.util.Arrays.sort(order, (a, b) -> Float.compare(irImage[b], irImage[a]));

        int[] comp = new int[IrImageSize];   // pixel -> id komponenty (roots)
        java.util.Arrays.fill(comp, -1);
        int[]   parent   = new int[pixels.length];
        float[] peakTemp = new float[pixels.length];
        int nComp = 0;
        int peaks = 0;

        for (int idx : order) {
            float v = irImage[idx];
            int x = idx % IR_WIDTH;
            int y = idx / IR_WIDTH;

            int best = -1;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= IR_WIDTH || ny < 0 || ny >= IR_HEIGHT) continue;
                    int c = comp[ny * IR_WIDTH + nx];
                    if (c < 0) continue;
                    int root = findRoot(parent, c);
                    if (best < 0) {
                        best = root;
                    } else if (root != best) {
                        // dve komponenty se potkavaji na sedle v teplote v
                        int hi = (peakTemp[root] > peakTemp[best]) ? root : best;
                        int lo = (hi == root) ? best : root;
                        if (peakTemp[lo] - v >= PEAK_PROMINENCE) {
                            peaks++;   // nizsi vrchol = dalsi osoba
                        }
                        parent[lo] = hi;
                        best = hi;
                    }
                }
            }

            if (best < 0) {
                parent[nComp]   = nComp;
                peakTemp[nComp] = v;
                best = nComp++;
            }
            comp[idx] = best;
        }

        return peaks + 1;   // nejvyssi vrchol clusteru
    }

    private static int findRoot(int[] parent, int c) {
        while (parent[c] != c) {
            parent[c] = parent[parent[c]];
            c = parent[c];
        }
        return c;
    }

    /**
     * Renders the IR frame as a PNG heatmap (iron-bow palette, dynamic range,
     * nearest-neighbor upscale) for the Home Assistant MQTT camera.
     */
    private byte[] renderPng() {
        float min = irImage[0], max = irImage[0];
        for (float t : irImage) {
            if (t < min) min = t;
            if (t > max) max = t;
        }
        float range = (max - min) == 0 ? 1 : (max - min);

        BufferedImage img = new BufferedImage(
                IR_WIDTH * PNG_SCALE, IR_HEIGHT * PNG_SCALE, BufferedImage.TYPE_INT_RGB);

        for (int y = 0; y < IR_HEIGHT; y++) {
            for (int x = 0; x < IR_WIDTH; x++) {
                int rgb = ironBowColor((irImage[y * IR_WIDTH + x] - min) / range);
                for (int py = 0; py < PNG_SCALE; py++) {
                    for (int px = 0; px < PNG_SCALE; px++) {
                        img.setRGB(x * PNG_SCALE + px, y * PNG_SCALE + py, rgb);
                    }
                }
            }
        }

        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ImageIO.write(img, "png", baos);
            return baos.toByteArray();
        } catch (Exception e) {
            return null;
        }
    }

    // Iron-bow palette: dark blue -> purple -> red -> yellow -> white
    // (same mapping as the web UI heatmap)
    private static int ironBowColor(float normalized) {
        int r, g, b;
        if (normalized < 0.25f) {
            float t = normalized / 0.25f;
            r = (int) (t * 130);
            g = 0;
            b = (int) (130 + t * 125);
        } else if (normalized < 0.5f) {
            float t = (normalized - 0.25f) / 0.25f;
            r = (int) (130 + t * 125);
            g = 0;
            b = (int) (255 - t * 255);
        } else if (normalized < 0.75f) {
            float t = (normalized - 0.5f) / 0.25f;
            r = 255;
            g = (int) (t * 255);
            b = 0;
        } else {
            float t = (normalized - 0.75f) / 0.25f;
            r = 255;
            g = 255;
            b = (int) (t * 255);
        }
        return (r << 16) | (g << 8) | b;
    }

    @Override
    public String getDataByKey(String key) {
        if (propertyMap.containsKey(key)) {
            return propertyMap.get(key).data.toString();
        }
        return "";
    }

    // -------------------------------------------------------------------------
    // Outgoing message to RS-485 device
    // -------------------------------------------------------------------------
    @Override
    public String sendMsg() {
        LocalDateTime now1 = LocalDateTime.now();

        String msg = "id:" + name;

        if (Duration.between(lastStatusReq, now1).toSeconds() > 30) {
            msg += ",msg:status";
            lastStatusReq = now1;
        } else {
            msg += ",msg:data";
        }

        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int) crc);
        return msg;
    }

    // -------------------------------------------------------------------------
    // Incoming MQTT commands (topic: <prefix>/<device>/cmd/<key>)
    // -------------------------------------------------------------------------
    @Override
    public synchronized void cmdToDevice(String key, String value) {
        switch (key) {
            case "reboot" -> super.reboot();
        }
    }

    // -------------------------------------------------------------------------
    // Home Assistant MQTT discovery config
    // -------------------------------------------------------------------------
    @Override
    public void publishHomeAssistentConfig(String topic) {
        String base  = Configuration.getXMLObj().getMQTTBroker().get(0).topic + getName() + "/";
        String avail = base + "state/online";

        ObjectMapper mapper = new ObjectMapper();
        ObjectNode device   = buildDeviceNode(mapper, "RoomIR");

        // IR heatmap camera (raw PNG bytes on topic)
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_irCamera";
            c.put("name",      "heatmap");
            c.put("unique_id", id);
            c.put("topic",     base + "tele/irImagePng");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "camera/" + id + "/config", c);
        }

        // Maximum temperature in the image
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_maxT";
            c.put("name",               "maxTemperature");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/maxTemperature");
            c.put("unit_of_measurement","°C");
            c.put("device_class",       "temperature");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Minimum temperature in the image
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_minT";
            c.put("name",               "minTemperature");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/minTemperature");
            c.put("unit_of_measurement","°C");
            c.put("device_class",       "temperature");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Fire alarm - binary sensor
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_fire";
            c.put("name",        "fireAlarm");
            c.put("unique_id",   id);
            c.put("state_topic", base + "tele/fireAlarm");
            c.put("device_class","heat");
            c.put("payload_on",  "1");
            c.put("payload_off", "0");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "binary_sensor/" + id + "/config", c);
        }

        // Person count (clustering)
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_pCount";
            c.put("name",        "personCount");
            c.put("unique_id",   id);
            c.put("state_topic", base + "tele/personCount");
            c.put("state_class", "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Zones - motion binary sensors (one per configured & enabled zone)
        for (int i = 0; i < MAX_ZONES; i++) {
            String id = getName() + "_zone" + i;
            String cfgTopic = topic + "binary_sensor/" + id + "/config";

            if (i < irConfig.zones.size() && irConfig.zones.get(i).enabled) {
                ConfXmlObject.IRZone zone = irConfig.zones.get(i);
                ObjectNode c = mapper.createObjectNode();
                String zoneName = (zone.name != null && !zone.name.isEmpty())
                        ? zone.name : ("zone" + i);
                c.put("name",        "motion " + zoneName);
                c.put("unique_id",   id);
                c.put("state_topic", base + "tele/zone" + i + "Presence");
                c.put("device_class","motion");
                c.put("payload_on",  "1");
                c.put("payload_off", "0");
                addAvailability(c, avail);
                c.set("device", device);
                publishHAConfig(mapper, cfgTopic, c);
            } else {
                // remove discovery entry for zones that no longer exist
                MQTT.MQTTinterface.getInstance().publishRaw(cfgTopic, "", true, 0);
            }
        }
    }
}
