
package Devices;

import GUI.GuiModels;
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
    boolean isRunning;
    LocalDateTime lastResponse;
    
    HashMap<String, DeviceProperty> propertyMap;
    protected List<String> cmdList = new ArrayList<>();
    
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

    public DeviceGeneric(String connectionName, String name)
    {
        this.connectionName = connectionName;
        this.name = name;
        //this.isRunningTimer = new Timer(name+"_running_timer");
        this.lastResponse = LocalDateTime.now();
        //this.logTimer = new Timer(name+"_log_timer");
        
        propertyMap = new HashMap<>();
    }
    
    // generic acces to device states
    public String getConnectionName() {return connectionName;}
    public String getName() {return name;}
    public boolean isOnline() {return isRunning;}
    public abstract  void setDataByKey(String key, String data);
    public abstract String getDataByKey(String key);
    public List<String> getCmdList() {return cmdList; }
    
    // For Mqtt interface
    public abstract void cmdToDevice(String key, String value);
    public void infFromDevice(String key, String value)
    {
        MQTT.MQTTinterface.getInstance().publish(name+"/"+key, value, true);
    }
    
    // For serial interface
    public abstract String sendMsg();
    // incoming data from serial
    public void recvMsg(HashMap<String,Object> data)
    {
        for (HashMap.Entry<String,Object> entry : data.entrySet())
        {
            setDataByKey(entry.getKey(), entry.getValue().toString());
            GuiModels.getInstance().updateDeviceMap(name, entry.getKey(), entry.getValue().toString());
        }
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