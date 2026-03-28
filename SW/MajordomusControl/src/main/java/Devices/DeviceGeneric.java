/*
 * MajordomusControl - Home Automation Gateway
 * Abstract base class for all RS-485 devices.
 * Handles MQTT topic publishing (state/tele/evt categories), availability tracking,
 * sensor value validation (INT16_MIN sentinel → "NaN"), and button click-pattern detection.
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

import SerialCom.SerialCommunication;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public abstract class DeviceGeneric {

    String connectionName;
    String name;
    String type;
    boolean isRunning;
    LocalDateTime lastResponse;

    HashMap<String, DeviceProperty> propertyMap;
    protected List<String> cmdList = new ArrayList<>();
    protected List<String> infList = new ArrayList<>();

    protected class DeviceProperty {

        public DeviceProperty() {
            data = 0;
            lastData = 0;
            lastSend = LocalDateTime.now();
        }

        Object data;
        Object lastData;
        LocalDateTime lastSend;
    }

    public DeviceGeneric(String connectionName, String name, String type) {
        this.connectionName = connectionName;
        this.name = name;
        this.type = type;
        this.lastResponse = LocalDateTime.now();
        propertyMap = new HashMap<>();
    }

    // Generic access to device states
    public String getConnectionName() { return connectionName; }
    public String getName()           { return name; }
    public String getType()           { return type; }
    public boolean isOnline()         { return isRunning; }
    public List<String> getCmdList()  { return cmdList; }
    public List<String> getInfList()  { return infList; }

    public abstract void setDataByKey(String key, String data);
    public abstract String getDataByKey(String key);
    public abstract void publishHomeAssistentConfig(String topic);
    public abstract void cmdToDevice(String key, String value);

    // -------------------------------------------------------------------------
    // Sensor value validation
    // Firmware uses INT16_MIN (-32768) as "sensor not available" sentinel.
    // These helpers return "NaN" for that value, otherwise the numeric object.
    // -------------------------------------------------------------------------
    protected static final int INVALID_SENSOR = Short.MIN_VALUE; // -32768

    /** Float sensor value (e.g. temperature sent as "22.5") */
    protected static Object toMqtt(float v) {
        return ((int) v == INVALID_SENSOR) ? "NaN" : v;
    }

    /** Integer sensor value (e.g. VOC, CO2 sent as "420") */
    protected static Object toMqtt(int v) {
        return (v == INVALID_SENSOR) ? "NaN" : v;
    }

    /** Integer raw value divided by scale (e.g. TempOutBoard sends "225" → 22.5 °C) */
    protected static Object toMqttScaled(int raw, double scale) {
        return (raw == INVALID_SENSOR) ? "NaN" : raw / scale;
    }

    // -------------------------------------------------------------------------
    // Apparent ("feels-like") temperature — Steadman formula
    // AT = T + 0.33 * e - 4.00,  where e = (rh/100) * 6.105 * exp(17.27*T/(237.7+T))
    // Result rounded to 1 decimal place. Valid across typical indoor/outdoor range.
    // -------------------------------------------------------------------------
    protected static float calcApparentTemperature(float tempC, float rh) {
        double e  = (rh / 100.0) * 6.105 * Math.exp((17.27 * tempC) / (237.7 + tempC));
        double at = tempC + 0.33 * e - 4.00;
        return (float)(Math.round(at * 10.0) / 10.0);
    }

    /**
     * Publishes tele/apparentTemperature when both "t0" and "rh" are present and valid
     * (neither equals the NaN sentinel). Adds "at" to infList on first publish.
     */
    protected void publishApparentTemperature() {
        if (!propertyMap.containsKey("t0") || !propertyMap.containsKey("rh")) return;
        Object tObj  = propertyMap.get("t0").data;
        Object rhObj = propertyMap.get("rh").data;
        if ("NaN".equals(tObj) || "NaN".equals(rhObj)) return;
        float t  = ((Number) tObj).floatValue();
        float rh = ((Number) rhObj).floatValue();
        if (!infList.contains("at")) infList.add("at");
        infFromDevice("tele", "apparentTemperature", "at", calcApparentTemperature(t, rh), true);
    }

    // -------------------------------------------------------------------------
    // MQTT publishing - category/mqttKey topic structure
    //   category : "tele"  - periodic sensor data   (retained)
    //              "evt"   - events on change only   (NOT retained)
    //              "state" - device & output state   (retained)
    //   mqttKey  : full MQTT name, e.g. "temperature0"
    //   key      : internal propertyMap key,  e.g. "t0"
    // -------------------------------------------------------------------------
    public void infFromDevice(String category, String mqttKey, String key, Object value, boolean retain) {
        boolean changedAndSend = false;

        DeviceProperty property;
        if (propertyMap.containsKey(key)) {
            property = propertyMap.get(key);
        } else {
            property = new DeviceProperty();
            propertyMap.put(key, property);
            changedAndSend = true;
        }

        if (!property.lastData.equals(value)) {
            changedAndSend = true;
        }
        property.lastData = property.data;
        property.data = value;

        if (changedAndSend) {
            property.lastSend = LocalDateTime.now();
            MQTT.MQTTinterface.getInstance().publish(
                    name + "/" + category + "/" + mqttKey,
                    getDataByKey(key),
                    retain);
        }
    }

    // Publish online/offline status to state/online
    public void publishAvailability() {
        MQTT.MQTTinterface.getInstance().publish(
                name + "/state/online",
                isRunning ? "online" : "offline",
                true);
    }

    // -------------------------------------------------------------------------
    // Serial interface
    // -------------------------------------------------------------------------
    public abstract String sendMsg();

    public void recvMsg(HashMap<String, Object> data) {
        for (HashMap.Entry<String, Object> entry : data.entrySet()) {
            setDataByKey(entry.getKey(), entry.getValue().toString());
        }
        updateLastResponse();
    }

    public void updateLastResponse() {
        boolean wasOnline = this.isRunning;
        this.lastResponse = LocalDateTime.now();
        this.isRunning = true;
        if (!wasOnline) {
            publishAvailability();
        }
    }

    public void checkIsRunning() {
        LocalDateTime now1 = LocalDateTime.now();
        long duration = Duration.between(this.lastResponse, now1).toMillis();
        boolean wasOnline = this.isRunning;
        if (duration >= 3000) {
            this.isRunning = false;
        }
        if (wasOnline != this.isRunning) {
            publishAvailability();
        }
    }

    public void reboot() {
        String msg = "id:" + this.name + ",msg:config,cmd:reset";
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int) crc);
        SerialCommunication.getInstance().sendAsyncMsg(this.connectionName, msg);
    }

    // -------------------------------------------------------------------------
    // Home Assistant discovery helpers (shared by all device subclasses)
    // -------------------------------------------------------------------------
    protected ObjectNode buildDeviceNode(ObjectMapper mapper, String model) {
        ObjectNode device = mapper.createObjectNode();
        ArrayNode identifiers = mapper.createArrayNode();
        identifiers.add(getName());
        device.set("identifiers", identifiers);
        device.put("manufacturer", "Majordomus");
        device.put("model", model);
        device.put("name", getName());
        return device;
    }

    protected void addAvailability(ObjectNode config, String availTopic) {
        config.put("availability_topic", availTopic);
        config.put("payload_available", "online");
        config.put("payload_not_available", "offline");
    }

    protected void publishHAConfig(ObjectMapper mapper, String topic, ObjectNode config) {
        try {
            MQTT.MQTTinterface.getInstance().publishRaw(
                    topic,
                    mapper.writerWithDefaultPrettyPrinter().writeValueAsString(config),
                    true, 0);
        } catch (JsonProcessingException ex) { }
    }
}
