/*
 * MajordomusControl - Home Automation Gateway
 * Driver for the Majordomus TempOutBoard device (RS-485).
 * Sensors: temperature (×8, raw integer / 10.0 °C, e.g. DS18B20 1-Wire sensors).
 * Outputs: digital outputs (×10), resistor output (ro).
 * Temperature values use INT16_MIN (-32768) as "sensor not connected" sentinel → published as "NaN".
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
import java.util.Arrays;

/**
 *
 * @author husak
 */
public class TempOutBoard extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;

    public TempOutBoard(String connectionName, String name, String type) {
        super(connectionName, name, type);

        cmdList.addAll(Arrays.asList("ro",
                "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7", "do8", "do9"));
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
                // --- tele: temperatures (raw integer / 10.0, e.g. "225" → 22.5 °C) ---
                case "t0" -> infFromDevice("tele", "temperature0", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t1" -> infFromDevice("tele", "temperature1", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t2" -> infFromDevice("tele", "temperature2", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t3" -> infFromDevice("tele", "temperature3", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t4" -> infFromDevice("tele", "temperature4", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t5" -> infFromDevice("tele", "temperature5", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t6" -> infFromDevice("tele", "temperature6", key, toMqttScaled(Integer.parseInt(data), 10.0), true);
                case "t7" -> infFromDevice("tele", "temperature7", key, toMqttScaled(Integer.parseInt(data), 10.0), true);

                // --- state: device status ---
                case "version" -> infFromDevice("state", "version",    key, data,                  true);
                case "pwr"     -> infFromDevice("state", "power",      key, Float.parseFloat(data), true);
                case "pwrOut"  -> infFromDevice("state", "powerOut",   key, Float.parseFloat(data), true);
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

        if (Duration.between(lastStatusReq, now1).toSeconds() > 10) {
            msg += ",msg:status";
            lastStatusReq = now1;
        } else {
            msg += ",msg:data,";
            msg += "do:" + serializeDO() + ",";
            msg += "ro:" + getDataByKey("ro");
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
            case "do0" -> infFromDevice("state", "output0", key, Integer.valueOf(value), true);
            case "do1" -> infFromDevice("state", "output1", key, Integer.valueOf(value), true);
            case "do2" -> infFromDevice("state", "output2", key, Integer.valueOf(value), true);
            case "do3" -> infFromDevice("state", "output3", key, Integer.valueOf(value), true);
            case "do4" -> infFromDevice("state", "output4", key, Integer.valueOf(value), true);
            case "do5" -> infFromDevice("state", "output5", key, Integer.valueOf(value), true);
            case "do6" -> infFromDevice("state", "output6", key, Integer.valueOf(value), true);
            case "do7" -> infFromDevice("state", "output7", key, Integer.valueOf(value), true);
            case "do8" -> infFromDevice("state", "output8", key, Integer.valueOf(value), true);
            case "do9" -> infFromDevice("state", "output9", key, Integer.valueOf(value), true);
            case "ro"  -> infFromDevice("state", "ro",      key, Integer.valueOf(value), true);
        }
    }

    private String serializeDO() {
        int result = 0;
        for (int i = 0; i < 10; i++) {
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
        ObjectNode device   = buildDeviceNode(mapper, "TempOutBoard");

        // Temperatures t0–t7
        for (int i = 0; i < 8; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_t" + i;
            c.put("name",               "temperature" + i);
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/temperature" + i);
            c.put("unit_of_measurement","°C");
            c.put("device_class",       "temperature");
            c.put("state_class",        "measurement");
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

        // Digital outputs do0–do9 (switch)
        for (int i = 0; i < 10; i++) {
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
    }
}
