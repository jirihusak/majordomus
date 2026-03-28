/*
 * MajordomusControl - Home Automation Gateway
 * Driver for the Majordomus RoomIR device (RS-485).
 * Sensor: MLX90640 infrared thermal camera (32×24 = 768 pixels).
 * The IR image is transmitted in base64-encoded chunks over RS-485 and
 * reassembled before publishing the complete float array to tele/irImage.
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

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Base64;
import java.util.Collections;
import java.util.List;

/**
 *
 * @author husak
 */
public class RoomIR extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;
    private List<Boolean> setCntChanged = new ArrayList<>(Collections.nCopies(8, false));

    final int IrImageSize = 768;
    private final float[] irImage;

    public RoomIR(String connectionName, String name, String type) {
        super(connectionName, name, type);

        irImage = new float[IrImageSize];

        cmdList.addAll(Arrays.asList("reboot"));
        infList.addAll(Arrays.asList("version", "irImg"));

        for (String s : cmdList) {
            propertyMap.put(s, new DeviceProperty());
        }
    }

    // -------------------------------------------------------------------------
    // Incoming data from RS-485 device
    // -------------------------------------------------------------------------
    @Override
    public synchronized void setDataByKey(String key, String data) {
        try {
            switch (key) {
                // --- state: device status & IR parameters ---
                case "version" -> infFromDevice("state", "version",    key, data,                  true);
                case "res"     -> infFromDevice("state", "resolution", key, Float.parseFloat(data), true);
                case "offset"  -> infFromDevice("state", "offset",     key, Integer.parseInt(data),  true);

                // --- tele: IR image (assembled from parts) ---
                case "irData" -> {
                    byte[] imgPart    = Base64.getDecoder().decode(data);
                    int    imgPartSize = imgPart.length;
                    int    offset      = Integer.parseInt(getDataByKey("offset"));

                    for (int i = 0; offset < IrImageSize && i < imgPartSize; offset++, i++) {
                        irImage[offset] = (float) (imgPart[i] * (1.0 / Float.parseFloat(getDataByKey("res"))));
                    }

                    if (Integer.parseInt(getDataByKey("offset")) >= 640) {
                        String jsonString = java.util.Arrays.toString(irImage);
                        infFromDevice("tele", "irImage", "irImg", jsonString, true);
                    }
                }
            }
        } catch (NumberFormatException e) {
            // ignore malformed values
        }
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

        if (Duration.between(lastStatusReq, now1).toSeconds() > 15) {
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

    @Override
    public void publishHomeAssistentConfig(String topic) {
        // RoomIR HA discovery not implemented
    }
}
