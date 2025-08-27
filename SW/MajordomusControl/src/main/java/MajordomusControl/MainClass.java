/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package MajordomusControl;

import GUI.AppGUI;
import javafx.application.Application;
import Configuration.Configuration;
import MQTT.MQTTinterface;
import SerialCom.SerialCommunication;
import java.util.ArrayList;
import java.util.List;
/**
 *
 * @author husak
 */
public class MainClass {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
                
        Thread.currentThread().setName("Main");
        
        // init configuration
        Configuration.getInstance();
        
        // device interface
        Devices.DeviceInterface.getInstance();
        
        // init serial port
        SerialCommunication.getInstance();
        
        // init MQTT client
        MQTTinterface.getInstance();
        
        // start GUI
        if(args.length > 0 && args[0].equals("-gui")){
            Application.launch(AppGUI.class, args);
        }
    }
    
}
