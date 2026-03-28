/*
 * MajordomusControl - Home Automation Gateway
 * Application entry point: initialises and wires all subsystems
 * (Configuration, DeviceInterface, SerialCommunication, MQTT, WebInterface).
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
package MajordomusControl;

//import GUI.AppGUI;
//import javafx.application.Application;
import Configuration.Configuration;
import MQTT.MQTTinterface;
import SerialCom.SerialCommunication;
import Web.WebInterface;
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
        
        System.out.println("Starting");
                
        Thread.currentThread().setName("Main");
        
        // init configuration
        Configuration.getInstance();
        
        // device interface
        Devices.DeviceInterface.getInstance();
        
        // init serial port
        SerialCommunication.getInstance();
        
        // init MQTT client
        MQTTinterface.getInstance();
        
        // start we interface
        WebInterface.getInstance();
        
        // start GUI
//        if(args.length > 0 && args[0].equals("-gui")){
//            Application.launch(AppGUI.class, args);
//        }
    }
    
}
