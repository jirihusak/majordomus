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
public class RoomIO extends DeviceGeneric {
    
    private LocalDateTime lastStatusReq = LocalDateTime.MIN;

    public RoomIO(String connectionName, String name) {
        super(connectionName, name);
        
        cmdList.addAll(Arrays.asList("dac0", "dac1", "do0","do1", "do2", "do3", "do4", "do5", "do6", "do7"));
        for(String s:cmdList)
        {
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
                    infFromDevice(key, Float.parseFloat(data), true);
                    break;
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
                    infFromDevice(key, Float.parseFloat(data), true);
                    break;
                case "adc1":
                    infFromDevice(key, Float.parseFloat(data), true);
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
        
        if(Duration.between(lastStatusReq, now1).toSeconds() > 10)
        {
            msg += ",msg:status";
            lastStatusReq = now1;
        }
        else {
            msg += ",msg:data,";  
            
            msg += "do:" + serializeDO() + ",";

            msg += "dac0:" + getDataByKey("dac0") + ",";
            msg += "dac1:" + getDataByKey("dac1");
        }
        
        
       
        
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());

        msg += String.format(",crc:%02x\r\n", (int)crc);
        return msg;
    }

    @Override
    public synchronized void cmdToDevice(String key, String value) {
        
        //System.out.println("Cmd:" + key + ":" + value);
        GuiModels.getInstance().updateDeviceMap(name, key, value);
        
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
        }
    }
    
    // serialize do0 - do9 to one bit array
    private String serializeDO()
    {
        int result = 0;
        
        for (int i = 0; i < 8; i++) {
            result |= (Integer.parseInt(getDataByKey("do"+i)) == 0) ? 0 : (1 << i);
        }
        
        return String.valueOf(result);
    }
}
