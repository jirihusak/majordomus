/*
 * MajordomusControl - Home Automation Gateway
 * Singleton configuration manager: loads and saves config.xml using JAXB.
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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import jakarta.xml.bind.JAXBContext;
import jakarta.xml.bind.JAXBException;
import jakarta.xml.bind.Marshaller;
import jakarta.xml.bind.Unmarshaller;

/**
 *
 * @author husak
 */
public class Configuration {
    
    // load config file
    private static Configuration singletonInstance = null;
    private static final String CONFIG_XML_PATH =
            System.getenv("CONFIG_PATH") != null ? System.getenv("CONFIG_PATH") : "config.xml";
    private ConfXmlObject configXMLObject = new ConfXmlObject();
    private JAXBContext context;
    private Unmarshaller um = null;
    private Marshaller m = null;
    
    public static Configuration getInstance()
    {
        if(singletonInstance == null)
            singletonInstance = new Configuration();
        
        return singletonInstance;
    }
    
    public static ConfXmlObject getXMLObj()
    {
        return getInstance().configXMLObject;
    }
    
    private Configuration()
    {
        try {
            context = JAXBContext.newInstance(ConfXmlObject.class);
            um = context.createUnmarshaller();
            m = context.createMarshaller();
        }
        catch(JAXBException e) {e.printStackTrace();}
        
        loadFromFile();
        applyEnvVarOverrides();

        //saveToFile();
    }

    private void applyEnvVarOverrides()
    {
        String mqttBroker = System.getenv("MQTT_BROKER");
        if (mqttBroker == null) return;

        // Ensure at least one broker entry exists
        if (configXMLObject.getMQTTBroker().isEmpty())
            configXMLObject.getMQTTBroker().add(new ConfXmlObject.MQTTBroker());

        ConfXmlObject.MQTTBroker broker = configXMLObject.getMQTTBroker().get(0);
        broker.address = mqttBroker;

        String mqttUsername = System.getenv("MQTT_USERNAME");
        if (mqttUsername != null) broker.username = mqttUsername;

        String mqttPassword = System.getenv("MQTT_PASSWORD");
        if (mqttPassword != null) broker.passwd = mqttPassword;

        String mqttTopic = System.getenv("MQTT_TOPIC");
        if (mqttTopic != null) broker.topic = mqttTopic;

        String haDiscovery = System.getenv("HA_DISCOVERY");
        String haTopic     = System.getenv("HA_TOPIC");
        if (haDiscovery != null || haTopic != null) {
            if (configXMLObject.getHASettings().isEmpty())
                configXMLObject.getHASettings().add(new ConfXmlObject.HASettings());

            ConfXmlObject.HASettings ha = configXMLObject.getHASettings().get(0);
            if (haDiscovery != null) ha.enable = Boolean.parseBoolean(haDiscovery);
            if (haTopic     != null) ha.topic  = haTopic;
        }
    }
    
    private void loadFromFile()
    {
        try {
            configXMLObject = (ConfXmlObject) um.unmarshal(new FileReader(CONFIG_XML_PATH));
        } 
        catch(JAXBException e) {e.printStackTrace();}
        catch(FileNotFoundException  e) { e.printStackTrace();}
    }
    
    public void saveToFile()
    {
        try {
            m.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, Boolean.TRUE);
            // Write to System.out DEBUG ONLY
            // TODO
            m.marshal(configXMLObject, System.out);
            // Write to File
            m.marshal(configXMLObject, new File(CONFIG_XML_PATH));
        }
        catch(JAXBException e ){} 
    }   
}
