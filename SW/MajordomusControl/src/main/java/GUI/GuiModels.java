/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package GUI;

import javafx.beans.property.SimpleMapProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableMap;

/**
 *
 * @author husak
 */
public class GuiModels {
    
    static private GuiModels singletonInstance = null;
    static public GuiModels getInstance() {
        if (singletonInstance == null) {
            singletonInstance = new GuiModels();
        }
        return singletonInstance;
    }
    
    private GuiModels()
    {
        
    }

    ObservableMap<String, ObservableMap<String, String>> deviceMap = FXCollections.observableHashMap();
    
    // Wrap the ObservableMap in a SimpleMapProperty
    SimpleMapProperty<String, ObservableMap<String, String>> deviceMapProperty = new SimpleMapProperty<>(deviceMap);

    public SimpleMapProperty<String, ObservableMap<String, String>> getDeviceMap()
    {
        return deviceMapProperty;
    }
    
    synchronized public void  updateDeviceMap(String device, String property, String value)
    {
        if(!deviceMap.containsKey(device)){
            ObservableMap<String, String> newDeviceMap = FXCollections.observableHashMap();
            deviceMap.put(device, newDeviceMap);
        }
        deviceMap.get(device).put(property, value);  
    }
    
    
    
}
