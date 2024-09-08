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
        
        cmdList.addAll(Arrays.asList("dac0", "dac1", "do"));
        for(String s:cmdList)
        {
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
                    parsedData = Float.parseFloat(data);
                case "t1" ->
                    parsedData = Float.parseFloat(data);
                case "di" ->
                    parsedData = Integer.valueOf(data);
                case "btn" ->
                    parsedData = Integer.valueOf(data);
                case "adc0" ->
                    parsedData = Float.parseFloat(data);
                case "adc1" ->
                    parsedData = Float.parseFloat(data);
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
        
        if(Duration.between(lastStatusReq, now1).toSeconds() > 10)
        {
            msg += ",msg:status,";
            lastStatusReq = now1;
        }
        else {
            msg += ",msg:data,";    
        }
        
        
        msg += "do:" + getDataByKey("do") + ",";

        msg += "dac0:" + getDataByKey("dac0") + ",";
        msg += "dac1:" + getDataByKey("dac1");
        
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());

        msg += String.format(",crc:%02x\r\n", (int)crc);
        return msg;
    }

    @Override
    public synchronized void cmdToDevice(String key, String value) {
        
        //System.out.println("Cmd:" + key + ":" + value);
        GuiModels.getInstance().updateDeviceMap(name, key, value);
        
        switch (key) {
            case "do" ->
                propertyMap.get(key).data = Integer.valueOf(value);
            case "dac0" ->
                propertyMap.get(key).data = Float.valueOf(value);
            case "dac1" ->
                propertyMap.get(key).data = Float.valueOf(value);
        }
    }
}
