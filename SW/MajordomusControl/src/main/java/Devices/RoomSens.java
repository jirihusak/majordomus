/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Devices;

import GUI.GuiModels;
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

    public RoomSens(String connectionName, String name) {
        super(connectionName, name);
        
        cmdList.addAll(Arrays.asList("dac0", "dac1", "do0", "do1", "do2", "do3", "beep", "reqT", "light"));
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
        
        GuiModels.getInstance().updateDeviceMap(name, key, value);
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
}
