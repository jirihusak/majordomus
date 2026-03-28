/*
 * MajordomusControl - Home Automation Gateway
 * Device registry and message router: instantiates all configured devices,
 * routes incoming RS-485 messages to the correct device, and dispatches
 * incoming MQTT commands (topic: <prefix>/<device>/cmd/<key>) to cmdToDevice().
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

import Configuration.ConfXmlObject.Device;
import Configuration.ConfXmlObject.HASettings;
import Configuration.Configuration;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.lang.annotation.Retention;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.concurrent.ConcurrentHashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author husak
 */
public class DeviceInterface {

    static private DeviceInterface singletonInstance = null;
    private ConcurrentHashMap<String, DeviceGeneric> deviceMap;
    Timer isRunningTimer;

    static public DeviceInterface getInstance() {
        if (singletonInstance == null) {
            singletonInstance = new DeviceInterface();
        }

        return singletonInstance;
    }

    private DeviceInterface() {
        isRunningTimer = new Timer();
        deviceMap = new ConcurrentHashMap<>();

        initDevices();

        isRunningTimer.schedule(new TimerTask() {
            @Override
            public void run() {

                for (String key : deviceMap.keySet()) {
                    deviceMap.get(key).checkIsRunning();
                }
            }
        }, 0, 3000);

    }
    
    private void initDevices()
    {
        List<Device> devicesList = Configuration.getXMLObj().getDevices();

        for (Device d : devicesList) {
            switch (d.type) {
                case "RoomIO" -> {
                    deviceMap.put(d.name, new RoomIO(d.connection, d.name, d.type));
                }
                case "RoomSensor" -> {
                    deviceMap.put(d.name, new RoomSens(d.connection, d.name, d.type));
                }
                case "TempOutBoard" -> {
                    deviceMap.put(d.name, new TempOutBoard(d.connection, d.name, d.type));
                }
                case "RoomIR" -> {
                    deviceMap.put(d.name, new RoomIR(d.connection, d.name, d.type));
                }
                default ->
                    System.out.println("Unknown device type");
            }
        }
    }

    public synchronized void reloadConfiguration() {
        System.out.println("Reloading DeviceInterface configuration...");

        try {
            // Zastavit timer
            isRunningTimer.cancel();
            isRunningTimer = new Timer();

            // Vyčistit starou mapu zařízení
            deviceMap.clear();

            initDevices();

            // Restartovat timer pro kontrolu zařízení
            isRunningTimer.schedule(new TimerTask() {
                @Override
                public void run() {
                    for (String key : deviceMap.keySet()) {
                        deviceMap.get(key).checkIsRunning();
                    }
                }
            }, 0, 3000);

            System.out.println("DeviceInterface configuration reloaded successfully. Loaded " + deviceMap.size() + " devices");

        } catch (Exception e) {
            System.err.println("Error reloading DeviceInterface: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public String getSendSerialData(String name) {
        return deviceMap.get(name).sendMsg();
    }

    public DeviceGeneric getDeviceByName(String name) {
        return deviceMap.get(name);
    }
    
    public ConcurrentHashMap<String, DeviceGeneric> getDeviceMap() {
        return deviceMap;
    }

    public boolean parseIncomingSerialData(String data) {
        HashMap<String, Object> msgData = new LinkedHashMap<>();

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
        String deviceName = (String) msgData.get("id");   
        String continuingMsg = (String) msgData.get("cont");

        if (deviceMap.get(deviceName) != null) {
            deviceMap.get(deviceName).recvMsg(msgData);
        }
        
        // is not last message and it will be continued
        if(continuingMsg != null && continuingMsg.equals("1")){
            return false;
        }
        
        return true;
        
    }

    public void parseIncomingMQTTData(String topic, String data) {
        // Expected format: <prefix>/<device>/cmd/<key>
        String[] topicLevels = topic.split("/");

        if (topicLevels.length < 4) {
            return;
        }

        if ("cmd".equals(topicLevels[2])) {
            String deviceName = topicLevels[1];
            String key = topicLevels[3];
            String value = data.trim();
            System.out.println("MQTT cmd: device=" + deviceName + " key=" + key + " value=" + value);
            if (!deviceMap.containsKey(deviceName)) {
                System.err.println("MQTT cmd: unknown device '" + deviceName + "'");
                return;
            }
            try {
                deviceMap.get(deviceName).cmdToDevice(key, value);
            } catch (NumberFormatException e) {
                System.err.println("MQTT cmd number format error: " + key + "=" + value);
            } catch (Exception e) {
                System.err.println("MQTT cmd exception: " + key + "=" + value + " : " + e.getMessage());
            }
        }
    }

    public void publishHomeAssistantConfig()
    {
        HASettings haSet;
        if(Configuration.getXMLObj().getHASettings().size() > 0)
        {
            haSet = Configuration.getXMLObj().getHASettings().get(0);
            if(haSet.enable) {

                for (String deviceName : DeviceInterface.getInstance().getDeviceMap().keySet()) {
                    DeviceGeneric device = DeviceInterface.getInstance().getDeviceByName(deviceName);
                    device.publishHomeAssistentConfig(haSet.topic);
                }
            }
        }
    }
}
