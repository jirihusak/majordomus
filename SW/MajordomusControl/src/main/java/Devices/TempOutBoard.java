/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Devices;

//import GUI.GuiModels;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

/**
 *
 * @author husak
 */
public class TempOutBoard extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;

    public TempOutBoard(String connectionName, String name, String type) {
        super(connectionName, name, type);

        cmdList.addAll(Arrays.asList("ro", "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7", "do8", "do9"));
        for (String s : cmdList) {
            propertyMap.put(s, new DeviceProperty());
        }
    }

    @Override
    public synchronized void setDataByKey(String key, String data) {

        //System.out.println("Recv:" + key + ":" + data);
        try {

            switch (key) {
                case "t0" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t1" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t2" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t3" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t4" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t5" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t6" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "t7" ->
                    infFromDevice(key, Float.parseFloat(data) / 10.0, true);
                case "type" ->
                    infFromDevice(key, data, true);
                case "version" ->
                    infFromDevice(key, data, true);
                case "pwr" ->
                    infFromDevice(key, Float.parseFloat(data), true);
                case "pwrOut" ->
                    infFromDevice(key, Float.parseFloat(data), true);

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

            msg += "ro:" + getDataByKey("ro");

        }

        
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());

        msg += String.format(",crc:%02x\r\n", (int) crc);

        return msg;
    }

    @Override
    public synchronized void cmdToDevice(String key, String value) {

        //System.out.println("----- Temp Out Board Cmd:" + key + ":" + value);
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
            case "do8" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "do9" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "ro" ->
                propertyMap.get(key).data = Integer.valueOf(value);
        }
    }
    
    // serialize do0 - do9 to one bit array
    private String serializeDO()
    {
        int result = 0;
        
        for (int i = 0; i < 10; i++) {
            result |= (Integer.parseInt(getDataByKey("do"+i)) == 0) ? 0 : (1 << i);
        }
        
        return String.valueOf(result);
    }

    @Override
    public void publishHomeAssistentConfig(String topic) {
        
    }
}
