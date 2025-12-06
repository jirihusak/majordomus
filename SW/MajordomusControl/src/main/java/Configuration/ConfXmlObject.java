/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Configuration;

import java.util.ArrayList;
import java.util.List;
import jakarta.xml.bind.annotation.XmlAttribute;
import jakarta.xml.bind.annotation.XmlElement;
import jakarta.xml.bind.annotation.XmlElementWrapper;
import jakarta.xml.bind.annotation.XmlRootElement;
import jakarta.xml.bind.annotation.XmlTransient;

/**
 *
 * @author husak
 */
@XmlRootElement(name="config")
public class ConfXmlObject {
    
    //----------------- CONNECTIONS ---------------------
    @XmlElementWrapper(name = "connections")
    @XmlElement(name = "connection")
    private List<Connection> connections = new ArrayList<>();
    
    @XmlTransient
    public List<Connection> getConnections() {return connections;}
    public void setConnections(List<Connection> newConnList) { connections = newConnList;}
        
    public static class Connection
    {
        @XmlAttribute(name = "name")
        public String name;
        @XmlAttribute(name = "portName")
        public String portName;
    }
    //----------------- END CONNECTIONS ---------------------
    
    //----------------- DEVICES ---------------------
    @XmlElementWrapper(name = "devices")
    @XmlElement(name = "device")
    private List<Device> devices = new ArrayList<>();
    
    @XmlTransient
    public List<Device> getDevices() {return devices;}
    public void setDevices(List<Device> newDevList) { devices = newDevList;}
        
    public static class Device
    {
        @XmlAttribute(name = "name")
        public String name;
        @XmlAttribute(name = "connection")
        public String connection;
        @XmlAttribute(name = "type")
        public String type;
    }
    //----------------- END DEVICE ---------------------
    
    //----------------- MQTT ---------------------
    @XmlElementWrapper(name = "MQTT")
    @XmlElement(name = "broker")
    private List<MQTTBroker> brokers = new ArrayList<>();
    
    public static class MQTTBroker
    {
        @XmlAttribute(name = "address")
        public String address;
        @XmlAttribute(name = "username")
        public String username;
        @XmlAttribute(name = "passwd")
        public String passwd;
        @XmlAttribute(name = "topic")
        public String topic;
        @XmlAttribute(name = "selfsigned")
        public String selfsigned;
        @XmlAttribute(name = "cert")
        public String cert;
    }
    
    @XmlTransient
    public List<MQTTBroker> getMQTTBroker() {return brokers;}
    public void setMQTTBroker(List<MQTTBroker> newBroker) {brokers = newBroker;}
        
    //----------------- END MQTT ---------------------
    
    //----------------- HOME ASSISTANT ---------------------
    @XmlElementWrapper(name = "HomeAssistant")
    @XmlElement(name = "settings")
    private List<HASettings> haSettings = new ArrayList<>();
    
    public static class HASettings
    {
        @XmlAttribute(name = "enable")
        public boolean enable;
        @XmlAttribute(name = "topic")
        public String topic;
    }

    @XmlTransient
    public List<HASettings> getHASettings() {return haSettings;}
    public void setHASettings(List<HASettings> newHaSettings) {haSettings = newHaSettings;}
        
    //----------------- HOME ASSISTANT ---------------------
}

