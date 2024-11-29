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
public class TempOutBoard extends DeviceGeneric {

    private LocalDateTime lastStatusReq = LocalDateTime.MIN;

    public TempOutBoard(String connectionName, String name) {
        super(connectionName, name);

        cmdList.addAll(Arrays.asList("ro", "do0", "do1", "do2", "do3", "do4", "do5", "do6", "do7", "do8", "do9"));
        for (String s : cmdList) {
            propertyMap.put(s, new DeviceProperty());
        }
    }

    @Override
    public synchronized void setDataByKey(String key, String data) {

        //System.out.println("Recv:" + key + ":" + data);
        Object parsedData = null;
        boolean isChangedorTimeouted = false;

        try {

            switch (key) {
                case "t0" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t1" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t2" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t3" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t4" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t5" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t6" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "t7" ->
                    parsedData = Float.parseFloat(data) / 10.0;
                case "type" ->
                    parsedData = data;
                case "version" ->
                    parsedData = data;
                case "pwr" ->
                    parsedData = Float.parseFloat(data);
                case "pwrOut" ->
                    parsedData = Float.parseFloat(data);

            }

        } catch (NumberFormatException e) {
            System.err.println("NumberFormatException" + key + ":" + data);
        }

        // save to map and check if is changed or timeouted
        if (parsedData != null) {

            DeviceProperty property;
            // exist? - update
            if (propertyMap.containsKey(key)) {
                property = propertyMap.get(key);
            } // or create new class
            else {
                property = new DeviceProperty();
                propertyMap.put(key, property);
            }

            property.lastData = property.data;
            property.data = parsedData;

            if (!property.lastData.equals(parsedData)) {
                isChangedorTimeouted = true;
                //System.out.println("changed" + property.lastData + property.data);
            }
            if (Duration.between(property.lastSend, LocalDateTime.now()).toMillis() > 5000) {
                isChangedorTimeouted = true;
                //System.out.println("timeout" + property.lastData + parsedData);
            }

            // send MQTT
            if (isChangedorTimeouted) {
                property.lastSend = LocalDateTime.now();
                infFromDevice(key, getDataByKey(key));
            }
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
            msg += ",msg:status,";
            lastStatusReq = now1;
        } else {
            msg += ",msg:data,";
        }

        msg += "do:" + serializeDO() + ",";

        msg += "ro:" + getDataByKey("ro");

        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());

        msg += String.format(",crc:%02x\r\n", (int) crc);

        return msg;
    }

    @Override
    public synchronized void cmdToDevice(String key, String value) {

        System.out.println("----- Temp Out Board Cmd:" + key + ":" + value);
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
}
