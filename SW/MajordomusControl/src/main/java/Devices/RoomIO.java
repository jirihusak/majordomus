/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Devices;

import Configuration.Configuration;
import MQTT.MQTTinterface;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.introspect.ConcreteBeanPropertyBase;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.lang.reflect.Array;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author husak
 */
public class RoomIO extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;
    private List<Boolean> setCntChanged = new ArrayList<>(Collections.nCopies(8, false));

    public RoomIO(String connectionName, String name, String type) {
        super(connectionName, name, type);

        cmdList.addAll(Arrays.asList("dac0", "dac1", "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7",
                "setCnt0", "setCnt1", "setCnt2", "setCnt3", "setCnt4", "setCnt5", "setCnt6", "setCnt7"));
        infList.addAll(Arrays.asList("version", "pwr", "pwrOut", "t0", "t1", "di0", "di1", "di2", "di3", "di4", "di5", "di6", "di7",
                "cnt0", "cnt1", "cnt2", "cnt3", "cnt4", "cnt5", "cnt6", "cnt7", "adc0", "adc1",
                "dac0", "dac1", "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7"));

        for (String s : cmdList) {
            propertyMap.put(s, new DeviceProperty());
        }
    }

    @Override
    public synchronized void setDataByKey(String key, String data) {
        int parsedData;
        //System.out.println("Recv:" + key + ":" + data);
        try {
            switch (key) {
                case "t0":
                case "t1":
                    infFromDevice(key, Float.parseFloat(data), true);
                    break;
                case "di":
                    parsedData = Integer.parseInt(data);
                    infFromDevice("di0", ((parsedData >> 0) & 1), true);
                    infFromDevice("di1", ((parsedData >> 1) & 1), true);
                    infFromDevice("di2", ((parsedData >> 2) & 1), true);
                    infFromDevice("di3", ((parsedData >> 3) & 1), true);
                    infFromDevice("di4", ((parsedData >> 4) & 1), true);
                    infFromDevice("di5", ((parsedData >> 5) & 1), true);
                    infFromDevice("di6", ((parsedData >> 6) & 1), true);
                    infFromDevice("di7", ((parsedData >> 7) & 1), true);
                    break;
                case "btn":
                    parsedData = Integer.parseInt(data);
                    infFromDevice("btn0", ((parsedData >> 0) & 1), false);
                    infFromDevice("btn1", ((parsedData >> 1) & 1), false);
                    infFromDevice("btn2", ((parsedData >> 2) & 1), false);
                    infFromDevice("btn3", ((parsedData >> 3) & 1), false);
                    infFromDevice("btn4", ((parsedData >> 4) & 1), false);
                    infFromDevice("btn5", ((parsedData >> 5) & 1), false);
                    infFromDevice("btn6", ((parsedData >> 6) & 1), false);
                    infFromDevice("btn7", ((parsedData >> 7) & 1), false);
                    break;
                case "adc0":
                case "adc1":
                    infFromDevice(key, Float.parseFloat(data), true);
                    break;
                case "cnt0":
                case "cnt1":
                case "cnt2":
                case "cnt3":
                case "cnt4":
                case "cnt5":
                case "cnt6":
                case "cnt7":
                    infFromDevice(key, data, true);
                    break;
                case "type":
                    infFromDevice(key, data, true);
                    break;
                case "version":
                    infFromDevice(key, data, true);
                    break;
                case "pwr":
                    infFromDevice(key, Float.parseFloat(data), true);
                    break;
                case "pwrOut":
                    infFromDevice(key, Float.parseFloat(data), true);
                    break;
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

        if (Duration.between(lastStatusReq, now1).toSeconds() > 10) {
            msg += ",msg:status";
            lastStatusReq = now1;
        } else {
            msg += ",msg:data,";

            msg += "do:" + serializeDO() + ",";

            msg += "dac0:" + getDataByKey("dac0") + ",";
            msg += "dac1:" + getDataByKey("dac1");

            //set cnd
            if (setCntChanged.get(0)) {
                msg += ",setCnt0:" + getDataByKey("setCnt0");
                setCntChanged.set(0, false);
            }
            if (setCntChanged.get(1)) {
                msg += ",setCnt1:" + getDataByKey("setCnt1");
                setCntChanged.set(1, false);
            }
            if (setCntChanged.get(2)) {
                msg += ",setCnt2:" + getDataByKey("setCnt2");
                setCntChanged.set(2, false);
            }
            if (setCntChanged.get(3)) {
                msg += ",setCnt3:" + getDataByKey("setCnt3");
                setCntChanged.set(3, false);
            }
            if (setCntChanged.get(4)) {
                msg += ",setCnt4:" + getDataByKey("setCnt4");
                setCntChanged.set(4, false);
            }
            if (setCntChanged.get(5)) {
                msg += ",setCnt5:" + getDataByKey("setCnt5");
                setCntChanged.set(5, false);
            }
            if (setCntChanged.get(6)) {
                msg += ",setCnt6:" + getDataByKey("setCnt6");
                setCntChanged.set(6, false);
            }
            if (setCntChanged.get(7)) {
                msg += ",setCnt7:" + getDataByKey("setCnt7");
                setCntChanged.set(7, false);
            }
        }

        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());

        msg += String.format(",crc:%02x\r\n", (int) crc);
        return msg;
    }

    @Override
    public synchronized void cmdToDevice(String key, String value) {

        //System.out.println("Cmd:" + key + ":" + value);
        //GuiModels.getInstance().updateDeviceMap(name, key, value);

        switch (key) {
            case "do0" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do1" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do2" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do3" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do4" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do5" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do6" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do7" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "dac0" ->
                propertyMap.get(key).data = Float.valueOf(value);
            case "dac1" ->
                propertyMap.get(key).data = Float.valueOf(value);
            case "setCnt0" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(0, true);
            }
            case "setCnt1" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(1, true);
            }
            case "setCnt2" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(2, true);
            }
            case "setCnt3" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(3, true);
            }
            case "setCnt4" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(4, true);
            }
            case "setCnt5" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(5, true);
            }
            case "setCnt6" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(6, true);
            }
            case "setCnt7" -> {
                propertyMap.get(key).data = Integer.valueOf(value);
                setCntChanged.set(7, true);
            }
        }
    }

    // serialize do0 - do9 to one bit array
    private String serializeDO() {
        int result = 0;

        for (int i = 0; i < 8; i++) {
            result |= (Integer.parseInt(getDataByKey("do" + i)) == 0) ? 0 : (1 << i);
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

        // 8x Digital inputs
        for (int i = 0; i < 8; i++) {
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
        
        // 8x Digital outputs
        for (int i = 0; i < 8; i++) {
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
        
        // 8x pulses
        for (int i = 0; i < 8; i++) {
            ObjectNode config = mapper.createObjectNode();
            String id = getName() + "_cnt" + i;
            config.put("name", id);
            config.put("unique_id", id);
            //config.put("retain", false);
            config.put("qos", 0);
            config.put("unit_of_measurement", "pulses");
            config.put("state_topic", majordomusTopic + getName() + "/cnt" + i);
            config.set("device", device);
            try {
                MQTTinterface.getInstance().publishRaw(topic + "sensor/" + id + "/config", mapper.writerWithDefaultPrettyPrinter().writeValueAsString(config), true, 0);
            } catch (JsonProcessingException ex) { }
        }

   
        

    }
}
