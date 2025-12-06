/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
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
    private static final String CONFIG_XML_PATH = "config.xml";
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
        
        //saveToFile();
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
