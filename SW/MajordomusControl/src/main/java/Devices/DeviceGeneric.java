
package Devices;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 *
 * @author husak
 */
public abstract class DeviceGeneric {
    
    String connectionName;
    String name;
    String type;
    boolean isRunning;
    LocalDateTime lastResponse;
    
    HashMap<String, DeviceProperty> propertyMap;
    protected List<String> cmdList = new ArrayList<>();
    protected List<String> infList = new ArrayList<>();
    
    protected class DeviceProperty
    {

        public DeviceProperty() {
            data = 0;
            lastData = 0;
            lastSend = LocalDateTime.now();
        }
        
        Object data;
        Object lastData;
        LocalDateTime lastSend;
    }	

    public DeviceGeneric(String connectionName, String name, String type)
    {
        this.connectionName = connectionName;
        this.name = name;
        this.type = type;
        //this.isRunningTimer = new Timer(name+"_running_timer");
        this.lastResponse = LocalDateTime.now();
        //this.logTimer = new Timer(name+"_log_timer");
        
        propertyMap = new HashMap<>();
    }
    
    // generic acces to device states
    public String getConnectionName() {return connectionName;}
    public String getName() {return name;}
    public String getType() {return type;}
    public boolean isOnline() {return isRunning;}
    public abstract  void setDataByKey(String key, String data);
    public abstract String getDataByKey(String key);
    public List<String> getCmdList() {return cmdList; }
    public List<String> getInfList() {return infList; }
    public abstract void publishHomeAssistentConfig(String topic);
    
    // For Mqtt interface
    public abstract void cmdToDevice(String key, String value);
    public void infFromDevice(String key, Object value, boolean retain)
    {
        boolean changedAndSend = false;
        
        DeviceProperty property;
        // exist? - update
        if (propertyMap.containsKey(key)) {
            property = propertyMap.get(key);
        } // or create new class
        else {
            property = new DeviceProperty();
            propertyMap.put(key, property);
            changedAndSend = true;
        }

        if (!property.lastData.equals(value)) {
            changedAndSend = true;
            //System.out.println("changed" + property.lastData + property.data);
        }
        property.lastData = property.data;
        property.data = value;

        // send MQTT        
        if(changedAndSend) {
            property.lastSend = LocalDateTime.now();
            MQTT.MQTTinterface.getInstance().publish(name+"/"+key, getDataByKey(key), retain);
        }
    }
    
    
    // For serial interface
    public abstract String sendMsg();
    // incoming data from serial
    public void recvMsg(HashMap<String,Object> data)
    {
        for (HashMap.Entry<String,Object> entry : data.entrySet())
        {
            setDataByKey(entry.getKey(), entry.getValue().toString());
            //GuiModels.getInstance().updateDeviceMap(name, entry.getKey(), entry.getValue().toString());
        }
        updateLastResponse();
    }
    
    // Check if is device running
    public void updateLastResponse()
    {
        this.lastResponse = LocalDateTime.now();
        this.isRunning = true;
    }
    
    public void checkIsRunning()
    {
        LocalDateTime now1 = LocalDateTime.now();
        long duration = Duration.between(this.lastResponse, now1).toMillis();
        if(duration >= 3000){
            this.isRunning = false;
        }
    }
    
}