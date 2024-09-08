/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Configuration;

import java.util.ArrayList;
import java.util.List;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlElementWrapper;
import javax.xml.bind.annotation.XmlRootElement;
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
    
    public List<Connection> getConnectionsList() {return connections;}
    public void setConnectionsList(List<Connection> newConnList) { connections = newConnList;}
        
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
    
    public List<Device> getDevicesList() {return devices;}
    public void setDevicesList(List<Device> newDevList) { devices = newDevList;}
        
    public static class Device
    {
        @XmlAttribute(name = "name")
        public String name;
        @XmlAttribute(name = "connection")
        public String connection;
        @XmlAttribute(name = "type")
        public String type;
    }
    //----------------- END CONNECTIONS ---------------------
    
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
    }
    
    public List<MQTTBroker> getMQTTBroker() {return brokers;}
        
    //----------------- END CONNECTIONS ---------------------
}

