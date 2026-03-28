/*
 * MajordomusControl - Home Automation Gateway
 * Driver for the Majordomus RoomSensor device (RS-485).
 * Sensors: temperature (×4), humidity, VOC, NOx, CO2, illuminance, noise, motion,
 *          digital inputs (×4), analog inputs (×2).
 * Outputs: digital outputs (×4), DAC (×2), requested temperature setpoint, light level, beep.
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
import MQTT.MQTTinterface;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.time.Duration;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;

/**
 *
 * @author husak
 */
public class RoomSens extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;

    public RoomSens(String connectionName, String name, String type) {
        super(connectionName, name, type);

        cmdList.addAll(Arrays.asList("dac0", "dac1", "do0", "do1", "do2", "do3",
                "beep", "reqT", "light", "reboot"));
        infList.addAll(Arrays.asList("version", "pwr", "pwrOut",
                "t0", "t1", "t2", "t3", "rh", "at", "voc", "mo", "co2", "lux", "nox", "nl",
                "di0", "di1", "di2", "di3", "adc0", "adc1",
                "dac0", "dac1", "do0", "do1", "do2", "do3",
                "beep", "reqT", "light", "lastMo"));
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
                case "t0"  -> { infFromDevice("tele", "temperature0", key, toMqtt(Float.parseFloat(data)), true);
                               publishApparentTemperature(); }
                case "t1"  -> infFromDevice("tele", "temperature1", key, toMqtt(Float.parseFloat(data)),   true);
                case "t2"  -> infFromDevice("tele", "temperature2", key, toMqtt(Float.parseFloat(data)),   true);
                case "t3"  -> infFromDevice("tele", "temperature3", key, toMqtt(Float.parseFloat(data)),   true);
                case "rh"  -> { infFromDevice("tele", "humidity",     key, toMqtt(Float.parseFloat(data)), true);
                               publishApparentTemperature(); }
                case "voc" -> infFromDevice("tele", "voc",          key, toMqtt(Integer.parseInt(data)),   true);
                case "nox" -> infFromDevice("tele", "nox",          key, toMqtt(Integer.parseInt(data)),   true);
                case "co2" -> infFromDevice("tele", "co2",          key, toMqtt(Integer.parseInt(data)),   true);
                case "lux" -> infFromDevice("tele", "illuminance",  key, toMqtt(Integer.parseInt(data)),   true);
                case "nl"  -> infFromDevice("tele", "noise",        key, toMqtt(Integer.parseInt(data)),   true);
                case "adc0"-> infFromDevice("tele", "analog0",      key, toMqtt(Float.parseFloat(data)),   true);
                case "adc1"-> infFromDevice("tele", "analog1",      key, toMqtt(Float.parseFloat(data)),   true);

                // --- tele: digital input states (retained, on change) ---
                case "di"  -> {
                    parsedData = Integer.parseInt(data);
                    infFromDevice("tele", "input0", "di0", ((parsedData >> 0) & 1), true);
                    infFromDevice("tele", "input1", "di1", ((parsedData >> 1) & 1), true);
                    infFromDevice("tele", "input2", "di2", ((parsedData >> 2) & 1), true);
                    infFromDevice("tele", "input3", "di3", ((parsedData >> 3) & 1), true);
                }

                // --- evt: events (not retained) ---
                case "mo"  -> infFromDevice("evt", "motion", key, Integer.parseInt(data), false);
                case "btn" -> {
                    parsedData = Integer.parseInt(data);
                    infFromDevice("evt", "button0", "btn0", ((parsedData >> 0) & 1), false);
                    infFromDevice("evt", "button1", "btn1", ((parsedData >> 1) & 1), false);
                    infFromDevice("evt", "button2", "btn2", ((parsedData >> 2) & 1), false);
                    infFromDevice("evt", "button3", "btn3", ((parsedData >> 3) & 1), false);
                }

                // --- state: device status ---
                case "version" -> infFromDevice("state", "version",    key, data,                  true);
                case "pwr"     -> infFromDevice("state", "power",      key, Float.parseFloat(data), true);
                case "pwrOut"  -> infFromDevice("state", "powerOut",   key, Float.parseFloat(data), true);
                case "newReqT" -> infFromDevice("state", "requestedTemperature", key, Float.parseFloat(data), true);
            }
        } catch (NumberFormatException e) {
            // ignore malformed values
        }

        if (getDataByKey("mo").equals("1")) {
            infFromDevice("evt", "lastMotion", "lastMo",
                    LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")),
                    true);
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
            msg += "do:"   + serializeDO()          + ",";
            msg += "dac0:" + getDataByKey("dac0")   + ",";
            msg += "dac1:" + getDataByKey("dac1")   + ",";
            msg += "light:"+ getDataByKey("light")  + ",";
            msg += "reqT:" + getDataByKey("reqT");

            if (!getDataByKey("beep").equals("0")) {
                msg += ",beep:" + getDataByKey("beep");
                cmdToDevice("beep", "0");
            }
        }

        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int) crc);
        return msg;
    }

    // -------------------------------------------------------------------------
    // Incoming MQTT commands (topic: <prefix>/<device>/cmd/<key>)
    // infFromDevice publishes confirmation back to state/
    // -------------------------------------------------------------------------
    @Override
    public synchronized void cmdToDevice(String key, String value) {
        switch (key) {
            case "reboot" -> super.reboot();
            case "do0"    -> infFromDevice("state", "output0",              key, Integer.valueOf(value), true);
            case "do1"    -> infFromDevice("state", "output1",              key, Integer.valueOf(value), true);
            case "do2"    -> infFromDevice("state", "output2",              key, Integer.valueOf(value), true);
            case "do3"    -> infFromDevice("state", "output3",              key, Integer.valueOf(value), true);
            case "dac0"   -> infFromDevice("state", "dac0",                 key, Float.valueOf(value),   true);
            case "dac1"   -> infFromDevice("state", "dac1",                 key, Float.valueOf(value),   true);
            case "beep"   -> infFromDevice("state", "beep",                 key, Integer.valueOf(value), false);
            case "reqT"   -> infFromDevice("state", "requestedTemperature", key, Float.valueOf(value),   true);
            case "light"  -> infFromDevice("state", "light",                key, Integer.valueOf(value), true);
        }
    }

    private String serializeDO() {
        int result = 0;
        for (int i = 0; i < 4; i++) {
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
        ObjectNode device   = buildDeviceNode(mapper, "RoomSensor");

        // Temperatures t0–t3
        String[] tempMqtt = {"temperature0", "temperature1", "temperature2", "temperature3"};
        for (int i = 0; i < 4; i++) {
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

        // Humidity
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_rh";
            c.put("name",               "humidity");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/humidity");
            c.put("unit_of_measurement","%");
            c.put("device_class",       "humidity");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Apparent temperature (feels-like, Steadman formula)
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_at";
            c.put("name",               "apparentTemperature");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/apparentTemperature");
            c.put("unit_of_measurement","°C");
            c.put("device_class",       "temperature");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // VOC
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_voc";
            c.put("name",               "voc");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/voc");
            c.put("unit_of_measurement","index");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // NOx
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_nox";
            c.put("name",               "nox");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/nox");
            c.put("unit_of_measurement","index");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // CO2
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_co2";
            c.put("name",               "co2");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/co2");
            c.put("unit_of_measurement","ppm");
            c.put("device_class",       "carbon_dioxide");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Illuminance
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_lux";
            c.put("name",               "illuminance");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "tele/illuminance");
            c.put("unit_of_measurement","lx");
            c.put("device_class",       "illuminance");
            c.put("state_class",        "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Noise
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_noise";
            c.put("name",        "noise");
            c.put("unique_id",   id);
            c.put("state_topic", base + "tele/noise");
            c.put("state_class", "measurement");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "sensor/" + id + "/config", c);
        }

        // Analog inputs
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

        // Motion – binary sensor
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_motion";
            c.put("name",        "motion");
            c.put("unique_id",   id);
            c.put("state_topic", base + "evt/motion");
            c.put("device_class","motion");
            c.put("payload_on",  "1");
            c.put("payload_off", "0");
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "binary_sensor/" + id + "/config", c);
        }

        // Digital inputs di0–di3 (retained state)
        for (int i = 0; i < 4; i++) {
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

        // Digital outputs do0–do3 (switch)
        for (int i = 0; i < 4; i++) {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_output" + i;
            c.put("name",            "output" + i);
            c.put("unique_id",       id);
            c.put("state_topic",     base + "state/output" + i);
            c.put("command_topic",   base + "cmd/do" + i);
            c.put("payload_on",      "1");
            c.put("payload_off",     "0");
            c.put("state_on",        "1");
            c.put("state_off",       "0");
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

        // Requested temperature (number 10–35 °C)
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_reqT";
            c.put("name",               "requestedTemperature");
            c.put("unique_id",          id);
            c.put("state_topic",        base + "state/requestedTemperature");
            c.put("command_topic",      base + "cmd/reqT");
            c.put("unit_of_measurement","°C");
            c.put("device_class",       "temperature");
            c.put("min",  10.0);
            c.put("max",  35.0);
            c.put("step", 0.5);
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "number/" + id + "/config", c);
        }

        // Light setpoint (number 0–100)
        {
            ObjectNode c = mapper.createObjectNode();
            String id = getName() + "_light";
            c.put("name",          "light");
            c.put("unique_id",     id);
            c.put("state_topic",   base + "state/light");
            c.put("command_topic", base + "cmd/light");
            c.put("min",  0);
            c.put("max",  100);
            c.put("step", 1);
            addAvailability(c, avail);
            c.set("device", device);
            publishHAConfig(mapper, topic + "number/" + id + "/config", c);
        }
    }
}
