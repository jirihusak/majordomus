/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Devices;

import Configuration.ConfXmlObject.Device;
import Configuration.Configuration;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author husak
 */
public class DeviceInterface {

    static private DeviceInterface singletonInstance = null;
    private HashMap<String, DeviceGeneric> deviceMap;
    Timer isRunningTimer;

    static public DeviceInterface getInstance() {
        if (singletonInstance == null) {
            singletonInstance = new DeviceInterface();
        }

        return singletonInstance;
    }

    private DeviceInterface() {
        isRunningTimer = new Timer();
        deviceMap = new HashMap<>();

        List<Device> devicesList = Configuration.getXMLObj().getDevicesList();

        for (Device d : devicesList) {
            switch (d.type) {
                case "RoomIO" -> {
                    deviceMap.put(d.name, new RoomIO(d.connection, d.name));
                }
                case "RoomSens" -> {
                    deviceMap.put(d.name, new RoomSens(d.connection, d.name));
                }
                case "TempOutBoard" -> {
                    deviceMap.put(d.name, new TempOutBoard(d.connection, d.name));
                }
                default ->
                    System.out.println("Unknown device type");
            }
        }

        isRunningTimer.schedule(new TimerTask() {
            @Override
            public void run() {

                for (String key : deviceMap.keySet()) {
                    deviceMap.get(key).checkIsRunning();
                }
            }
        }, 0, 3000);

    }

    public String getSendSerialData(String name) {
        return deviceMap.get(name).sendMsg();
    }
    
    public DeviceGeneric getDeviceByName(String name)
    {
        return deviceMap.get(name);
    }

    public void parseIncomingSerialData(String data) {
        HashMap<String, Object> msgData = new HashMap<>();

        String[] keyValuePairs = data.split(",");
        for (String pair : keyValuePairs) {
            // split value :
            String[] entry = pair.split(":");

            String key = entry[0];
            Object value = entry.length > 1 ? entry[1] : 0; // Pokud není hodnota k dispozici, nastavte na null

            // Uložte hodnotu do mapy, můžete také provést konverzi hodnoty podle potřeby
            msgData.put(key, value);

        }
        
        // TODO check crc
        //System.out.println(msgData);

        String deviceName = msgData.get("id").toString();

        if (deviceMap.get(deviceName) != null) {
            deviceMap.get(deviceName).recvMsg(msgData);
        }
    }

    public void parseIncomingMQTTData(String topic, String data) {
        String topicLevels[] = topic.split("/");

        if (topicLevels.length < 3) {
            return;
        }

        if(deviceMap.containsKey(topicLevels[1]))
            deviceMap.get(topicLevels[1]).cmdToDevice(topicLevels[2], data);
    }

}
