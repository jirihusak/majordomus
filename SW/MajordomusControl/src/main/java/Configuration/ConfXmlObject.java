/*
 * MajordomusControl - Home Automation Gateway
 * JAXB-mapped XML schema for config.xml:
 * defines serial connections, device list, MQTT broker and Home Assistant settings.
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

