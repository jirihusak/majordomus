/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Devices;

//import GUI.GuiModels;
import Configuration.Configuration;
import MQTT.MQTTinterface;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

/**
 *
 * @author husak
 */
public class RoomSens extends DeviceGeneric {
    
    private LocalDateTime lastStatusReq = LocalDateTime.MIN;

    public RoomSens(String connectionName, String name, String type) {
        super(connectionName, name, type);
        
        cmdList.addAll(Arrays.asList("dac0", "dac1", "do0", "do1", "do2", "do3", "beep", "reqT", "light"));
        infList.addAll(Arrays.asList("version", "pwr", "pwrOut", "t0", "t1", "t2", "t3", "rh", "voc", "mo", "co2", "lux", "nox", "nl", "di0","di1", "di2", "di3",
                "adc0", "adc1", "dac0", "dac1", "do0", "do1", "do2", "do3", "beep", "reqT", "light"));
        for(String s:cmdList)
        {
            propertyMap.put(s, new DeviceProperty());
        }
    }

    @Override
    public synchronized void setDataByKey(String key, String data) {

        //System.out.println("Recv:" + key + ":" + data);
        int parsedData;
        try {

            switch (key) {
                case "t0" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "t1" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "t2" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "t3" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "rh" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "voc" ->
                    infFromDevice(key, Integer.parseInt(data), true);
                case "nox" ->
                    infFromDevice(key, Integer.parseInt(data), true);
                case "co2" ->
                    infFromDevice(key, Integer.parseInt(data), true);
                case "lux" ->
                    infFromDevice(key, Integer.parseInt(data), true);
                case "mo" ->
                    infFromDevice(key, Integer.parseInt(data), true);
                case "nl" ->
                    infFromDevice(key, Integer.parseInt(data), true);
                case "di" -> {
                    parsedData = Integer.parseInt(data);
                    infFromDevice("di0", ((parsedData >> 0) & 1), true);
                    infFromDevice("di1", ((parsedData >> 1) & 1), true);
                    infFromDevice("di2", ((parsedData >> 2) & 1), true);
                    infFromDevice("di3", ((parsedData >> 3) & 1), true);
                }
                case "btn" -> {
                    parsedData = Integer.parseInt(data);
                    infFromDevice("btn0", ((parsedData >> 0) & 1), false);
                    infFromDevice("btn1", ((parsedData >> 1) & 1), false);
                    infFromDevice("btn2", ((parsedData >> 2) & 1), false);
                    infFromDevice("btn3", ((parsedData >> 3) & 1), false);
                }
                case "adc0" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "adc1" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "type" ->
                    infFromDevice(key, data, true);
                case "version" ->
                    infFromDevice(key, data, true);
                case "pwr" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "pwrOut" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "newReqT" ->
                    infFromDevice(key, Float.parseFloat(data), false);

            }

        } catch (NumberFormatException e) {
            //System.err.println("NumberFormatException" + key + ":" + data);
        }
    }

    @Override
    public String getDataByKey(String key) {

        if (propertyMap.containsKey(key)) {
            return propertyMap.get(key).data.toString();
        } else {
            return "";
        }
    }

    @Override
    public String sendMsg() {
        LocalDateTime now1 = LocalDateTime.now();
        
        String msg = "id:" + name;
        
        if(Duration.between(lastStatusReq, now1).toSeconds() > 10)
        {
            msg += ",msg:status";
            lastStatusReq = now1;
        }
        else {
            msg += ",msg:data,";    
            
            msg += "do:" + serializeDO() + ",";
            msg += "dac0:" + getDataByKey("dac0") + ",";
            msg += "dac1:" + getDataByKey("dac1") + ",";
            msg += "light:" + getDataByKey("light") + ",";
            msg += "reqT:" + getDataByKey("reqT");

            // bell
            if (!getDataByKey("beep").equals("0")) {
                msg += ",beep:" + getDataByKey("beep");
                cmdToDevice("beep", "0");
            }
        }
        
        
        
        
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());

        msg += String.format(",crc:%02x\r\n", (int)crc);
        return msg;
    }

    @Override
    public synchronized void cmdToDevice(String key, String value) {
        
        //GuiModels.getInstance().updateDeviceMap(name, key, value);
        //System.out.println("Cmd:" + key + ":" + value);
        
        switch (key) {
            case "do0" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do1" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do2" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do3" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "dac0" ->
                propertyMap.get(key).data = Float.valueOf(value);
            case "dac1" ->
                propertyMap.get(key).data = Float.valueOf(value);
            case "beep" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "reqT" ->
                propertyMap.get(key).data = Float.valueOf(value);
            case "light" ->
                propertyMap.get(key).data = Integer.valueOf(value);
        }
    }
    
    // serialize do0 - do9 to one bit array
    private String serializeDO()
    {
        int result = 0;
        
        for (int i = 0; i < 4; i++) {
            result |= (Integer.parseInt(getDataByKey("do"+i)) == 0) ? 0 : (1 << i);
        }
        
        return String.valueOf(result);
    }

    @Override
    public void publishHomeAssistentConfig(String topic) {
        String majordomusTopic = Configuration.getXMLObj().getMQTTBroker().get(0).topic;

        ObjectMapper mapper = new ObjectMapper();
        // Device node - for all properies is the same
        ObjectNode device = mapper.createObjectNode();
        ArrayNode identifiers = mapper.createArrayNode();
        identifiers.add(getName());
        device.set("identifiers", identifiers);
        device.put("manufacturer", "Majordomus");
        device.put("model", "RoomIO");
        device.put("name", getName());

        // 4x Digital inputs
        for (int i = 0; i < 4; i++) {
            ObjectNode config = mapper.createObjectNode();
            String id = getName() + "_di" + i;
            config.put("name", id);
            config.put("unique_id", id);
            //config.put("retain", true);
            config.put("qos", 0);
            config.put("entity_category", "config");
            config.put("state_topic", majordomusTopic + getName() + "/di" + i);
            config.put("device_class", "power");
            config.put("payload_off", "0");
            config.put("payload_on", "1");
            config.set("device", device);
            try {
                MQTTinterface.getInstance().publishRaw(topic + "binary_sensor/" + id + "/config", mapper.writerWithDefaultPrettyPrinter().writeValueAsString(config), true, 0);
            } catch (JsonProcessingException ex) { }
        }
        
        // 4x Digital outputs
        for (int i = 0; i < 4; i++) {
            ObjectNode config = mapper.createObjectNode();
            String id = getName() + "_do" + i;
            config.put("name", id);
            config.put("unique_id", id);
            //config.put("retain", true);
            config.put("qos", 0);
            config.put("entity_category", "config");
            config.put("command_topic", majordomusTopic + getName() + "/do" + i);
            config.put("state_topic", majordomusTopic + getName() + "/do" + i);
            config.put("payload_off", "0");
            config.put("payload_on", "1");
            config.set("device", device);
            try {
                MQTTinterface.getInstance().publishRaw(topic + "switch/" + id + "/config", mapper.writerWithDefaultPrettyPrinter().writeValueAsString(config), true, 0);
            } catch (JsonProcessingException ex) { }
        }
        
        // temperature
        ObjectNode configTemp = mapper.createObjectNode();
        String id = getName() + "_t0";
        configTemp.put("name", id);
        configTemp.put("unique_id", id);
        configTemp.put("qos", 0);
        configTemp.put("unit_of_measurement", "°C");
        configTemp.put("device_class", "temperature");
        configTemp.put("state_class", "measurement");
        configTemp.put("state_topic", majordomusTopic + getName() + "/t0");
        configTemp.set("device", device);
        try {
            MQTTinterface.getInstance().publishRaw(topic + "sensor/" + id + "/config", mapper.writerWithDefaultPrettyPrinter().writeValueAsString(configTemp), true, 0);
        } catch (JsonProcessingException ex) { }
        
        // humidity
        ObjectNode configHr = mapper.createObjectNode();
        id = getName() + "_rh";
        configHr.put("name", id);
        configHr.put("unique_id", id);
        configHr.put("qos", 0);
        configHr.put("unit_of_measurement", "%");
        configHr.put("device_class", "humidity");
        configHr.put("state_class", "measurement");
        configHr.put("state_topic", majordomusTopic + getName() + "/rh");
        configHr.set("device", device);
        try {
            MQTTinterface.getInstance().publishRaw(topic + "sensor/" + id + "/config", mapper.writerWithDefaultPrettyPrinter().writeValueAsString(configHr), true, 0);
        } catch (JsonProcessingException ex) { }
        
    }
}
