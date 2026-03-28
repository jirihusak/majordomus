/*
 * MajordomusControl - Home Automation Gateway
 * Driver for the Majordomus RoomIO device (RS-485).
 * Sensors: temperature (×2), analog inputs (×2), pulse counters (×8),
 *          digital inputs (×8), optional: humidity, VOC, CO2, illuminance, distance.
 * Outputs: digital outputs (×8), DAC (×2), counter presets (×8).
 * Button events: click, doubleClick, tripleClick, longHold (via ButtonFilter).
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

import Configuration.Configuration;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/**
 *
 * @author husak
 */
public class RoomIO extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;
    private List<Boolean> setCntChanged = new ArrayList<>(Collections.nCopies(8, false));

    public RoomIO(String connectionName, String name, String type) {
        super(connectionName, name, type);

        cmdList.addAll(Arrays.asList("dac0", "dac1",
                "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7",
                "setCnt0", "setCnt1", "setCnt2", "setCnt3",
                "setCnt4", "setCnt5", "setCnt6", "setCnt7", "reboot"));
        infList.addAll(Arrays.asList("version", "pwr", "pwrOut",
                "t0", "t1",
                "di0", "di1", "di2", "di3", "di4", "di5", "di6", "di7",
                "cnt0", "cnt1", "cnt2", "cnt3", "cnt4", "cnt5", "cnt6", "cnt7",
                "adc0", "adc1",
                "dac0", "dac1",
                "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7"));

        for (String s : cmdList) {
            propertyMap.put(s, new DeviceProperty());
        }
    }

    // -------------------------------------------------------------------------
    // Incoming data from RS-485 device
    // -------------------------------------------------------------------------
    @Override
    public synchronized void setDataByKey(String key, String data) {
        int parsedData;
        try {
            switch (key) {
                // --- tele: periodic sensor readings ---
                case "t0"   -> { infFromDevice("tele", "temperature0", key, toMqtt(Float.parseFloat(data)), true);
                               publishApparentTemperature(); }
                case "t1"   -> infFromDevice("tele", "temperature1", key, toMqtt(Float.parseFloat(data)), true);
                case "adc0" -> infFromDevice("tele", "analog0",      key, toMqtt(Float.parseFloat(data)), true);
                case "adc1" -> infFromDevice("tele", "analog1",      key, toMqtt(Float.parseFloat(data)), true);
                case "cnt0" -> infFromDevice("tele", "counter0", key, data, true);
                case "cnt1" -> infFromDevice("tele", "counter1", key, data, true);
                case "cnt2" -> infFromDevice("tele", "counter2", key, data, true);
                case "cnt3" -> infFromDevice("tele", "counter3", key, data, true);
                case "cnt4" -> infFromDevice("tele", "counter4", key, data, true);
                case "cnt5" -> infFromDevice("tele", "counter5", key, data, true);
                case "cnt6" -> infFromDevice("tele", "counter6", key, data, true);
                case "cnt7" -> infFromDevice("tele", "counter7", key, data, true);

                // Optional extended sensors
                case "t2"  -> { if (!infList.contains(key)) infList.add(key);
                                infFromDevice("tele", "temperature2", key, toMqtt(Float.parseFloat(data)), true); }
                case "rh"  -> { if (!infList.contains(key)) infList.add(key);
                                infFromDevice("tele", "humidity",     key, toMqtt(Float.parseFloat(data)), true);
                                publishApparentTemperature(); }
                case "voc" -> { if (!infList.contains(key)) infList.add(key);
                                infFromDevice("tele", "voc",          key, toMqtt(Float.parseFloat(data)), true); }
                case "co2" -> { if (!infList.contains(key)) infList.add(key);
                                infFromDevice("tele", "co2",          key, toMqtt(Float.parseFloat(data)), true); }
                case "lux" -> { if (!infList.contains(key)) infList.add(key);
                                infFromDevice("tele", "illuminance",  key, toMqtt(Float.parseFloat(data)), true); }
                case "dst" -> { if (!infList.contains(key)) infList.add(key);
                                infFromDevice("tele", "distance",     key, toMqtt(Float.parseFloat(data)), true); }

                // --- tele: digital input states (retained, on change) ---
                case "di" -> {
                    parsedData = Integer.parseInt(data);
                    for (int i = 0; i < 8; i++) {
                        infFromDevice("tele", "input" + i, "di" + i, ((parsedData >> i) & 1), true);
                    }
                }

                // --- evt: button state on change (not retained) ---
                case "btn" -> {
                    parsedData = Integer.parseInt(data);
                    for (int i = 0; i < 8; i++) {
                        infFromDevice("evt", "button" + i, "btn" + i, ((parsedData >> i) & 1), false);
                    }
                }

                // --- state: device status ---
                case "version" -> infFromDevice("state", "version",  key, data,                  true);
                case "pwr"     -> infFromDevice("state", "power",    key, Float.parseFloat(data), true);
                case "pwrOut"  -> infFromDevice("state", "powerOut", key, Float.parseFloat(data), true);
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
            msg += ",msg:data,";
            msg += "do:"   + serializeDO()        + ",";
            msg += "dac0:" + getDataByKey("dac0") + ",";
            msg += "dac1:" + getDataByKey("dac1");

            for (int i = 0; i < 8; i++) {
                if (setCntChanged.get(i)) {
                    msg += ",setCnt" + i + ":" + getDataByKey("setCnt" + i);
                    setCntChanged.set(i, false);
                }
            }
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
            case "do0"    -> infFromDevice("state", "output0", key, Integer.valueOf(value), true);
            case "do1"    -> infFromDevice("state", "output1", key, Integer.valueOf(value), true);
            case "do2"    -> infFromDevice("state", "output2", key, Integer.valueOf(value), true);
            case "do3"    -> infFromDevice("state", "output3", key, Integer.valueOf(value), true);
            case "do4"    -> infFromDevice("state", "output4", key, Integer.valueOf(value), true);
            case "do5"    -> infFromDevice("state", "output5", key, Integer.valueOf(value), true);
            case "do6"    -> infFromDevice("state", "output6", key, Integer.valueOf(value), true);
            case "do7"    -> infFromDevice("state", "output7", key, Integer.valueOf(value), true);
            case "dac0"   -> infFromDevice("state", "dac0",    key, Float.valueOf(value),   true);
            case "dac1"   -> infFromDevice("state", "dac1",    key, Float.valueOf(value),   true);
            case "setCnt0" -> { infFromDevice("state", "setCounter0", key, Integer.valueOf(value), true); setCntChanged.set(0, true); }
            case "setCnt1" -> { infFromDevice("state", "setCounter1", key, Integer.valueOf(value), true); setCntChanged.set(1, true); }
            case "setCnt2" -> { infFromDevice("state", "setCounter2", key, Integer.valueOf(value), true); setCntChanged.set(2, true); }
            case "setCnt3" -> { infFromDevice("state", "setCounter3", key, Integer.valueOf(value), true); setCntChanged.set(3, true); }
            case "setCnt4" -> { infFromDevice("state", "setCounter4", key, Integer.valueOf(value), true); setCntChanged.set(4, true); }
            case "setCnt5" -> { infFromDevice("state", "setCounter5", key, Integer.valueOf(value), true); setCntChanged.set(5, true); }
            case "setCnt6" -> { infFromDevice("state", "setCounter6", key, Integer.valueOf(value), true); setCntChanged.set(6, true); }
            case "setCnt7" -> { infFromDevice("state", "setCounter7", key, Integer.valueOf(value), true); setCntChanged.set(7, true); }
        }
    }

    private String serializeDO() {
        int result = 0;
        for (int i = 0; i < 8; i++) {
            result |= (Integer.parseInt(getDataByKey("do" + i)) == 0) ? 0 : (1 << i);
        }
        return String.valueOf(result);
    }

    // -------------------------------------------------------------------------
    // Home Assistant MQTT discovery config
    // -------------------------------------------------------------------------
    @Override
    public void publishHomeAssistentConfig(String topic) {
        String base  = Configuration.getXMLObj().getMQTTBroker().get(0).topic + getName() + "/";
        String avail = base + "state/online";

        ObjectMapper mapper = new ObjectMapper();
        ObjectNode device   = buildDeviceNode(mapper, "RoomIO");

        // Temperatures t0–t1
        String[] tempMqtt = {"temperature0", "temperature1"};
        for (int i = 0; i < 2; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_t" + i;
            c.put("name",               tempMqtt[i]);
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/" + tempMqtt[i]);
            c.put("unit_of_measurement","°C");
            c.put("device_class",       "temperature");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Analog inputs adc0–adc1
        for (int i = 0; i < 2; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_adc" + i;
            c.put("name",               "analog" + i);
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/analog" + i);
            c.put("unit_of_measurement","V");
            c.put("device_class",       "voltage");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Counters cnt0–cnt7
        for (int i = 0; i < 8; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_cnt" + i;
            c.put("name",               "counter" + i);
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/counter" + i);
            c.put("unit_of_measurement","pulses");
            c.put("state_class",        "total_increasing");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Power supply voltage (diagnostic)
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_power";
            c.put("name",               "power");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "state/power");
            c.put("unit_of_measurement","V");
            c.put("device_class",       "voltage");
            c.put("state_class",        "measurement");
            c.put("entity_category",    "diagnostic");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Digital inputs di0–di7 (retained state)
        for (int i = 0; i < 8; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_input" + i;
            c.put("name",        "input" + i);
            c.put("unique_id",   id);
            c.put("state_topic", base + "tele/input" + i);
            c.put("payload_on",  "1");
            c.put("payload_off", "0");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "binary_sensor/" + id + "/config", c);
        }

        // Digital outputs do0–do7 (switch)
        for (int i = 0; i < 8; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_output" + i;
            c.put("name",          "output" + i);
            c.put("unique_id",     id);
            c.put("state_topic",   base + "state/output" + i);
            c.put("command_topic", base + "cmd/do" + i);
            c.put("payload_on",    "1");
            c.put("payload_off",   "0");
            c.put("state_on",      "1");
            c.put("state_off",     "0");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "switch/" + id + "/config", c);
        }

        // DAC outputs dac0–dac1 (number 0–255)
        for (int i = 0; i < 2; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_dac" + i;
            c.put("name",          "dac" + i);
            c.put("unique_id",     id);
            c.put("state_topic",   base + "state/dac" + i);
            c.put("command_topic", base + "cmd/dac" + i);
            c.put("min",  0);
            c.put("max",  255);
            c.put("step", 1);
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "number/" + id + "/config", c);
        }
    }
}
