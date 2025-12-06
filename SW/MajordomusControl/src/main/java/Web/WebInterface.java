/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package Web;

import Configuration.ConfXmlObject;
import Configuration.Configuration;
import Devices.DeviceGeneric;
import Devices.DeviceInterface;
import MQTT.MQTTinterface;
import SerialCom.SerialCommunication;
import io.javalin.Javalin;
import io.javalin.http.staticfiles.Location;
import java.nio.file.DirectoryStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author husak
 */
public class WebInterface {

    private static WebInterface singletonObj = null;

    public static synchronized WebInterface getInstance() {
        if (singletonObj == null) {
            singletonObj = new WebInterface();
        }

        return singletonObj;
    }

    private WebInterface() {
        // Create Javalin app
        Javalin app = Javalin.create(config -> {
            // Serve static files (HTML, CSS, JS) from resources/public
            config.staticFiles.add("/Web", Location.CLASSPATH);
        }).start(8899);

        // Serve main page
        app.get("/", ctx -> ctx.redirect("/index.html"));

        // REST API endpoints
        setupRestEndpoints(app);
        setupRestEndpointsConfig(app);
        setupRestEndpointsService(app);

        // WebSocket for real-time updates
        //setupWebSocket(app);
        // Listen to GUI model changes and broadcast to web clients
        //setupModelListener();
        System.out.println("Majordomus Control Web Interface started at: http://localhost:8899");
    }

    private void setupRestEndpoints(Javalin app) {

        app.get("/api/devices", ctx -> {
            List<Map<String, Object>> devicesResponse = new ArrayList<>();

            for (String deviceName : DeviceInterface.getInstance().getDeviceMap().keySet()) {
                DeviceGeneric device = DeviceInterface.getInstance().getDeviceByName(deviceName);

                Map<String, Object> deviceData = new HashMap<>();
                deviceData.put("id", device.getName());
                deviceData.put("connection", device.getConnectionName());
                deviceData.put("type", device.getType());
                deviceData.put("online", device.isOnline() ? 1 : 0);

                // Přidej properties jako array - v pořadí dle getInfList()
                List<Map<String, Object>> properties = new ArrayList<>();
                List<String> infList = device.getInfList();

                for (String propKey : infList) {
                    Map<String, Object> prop = new HashMap<>();
                    prop.put("key", propKey);
                    prop.put("value", device.getDataByKey(propKey));
                    properties.add(prop);
                }

                deviceData.put("properties", properties);
                devicesResponse.add(deviceData);
            }

            ctx.json(devicesResponse);
        });

        app.get("/api/commands", ctx -> {
            Map<String, List<String>> commandsResponse = new HashMap<>();

            // Získej seznam všech zařízení z DeviceInterface
            for (String deviceName : DeviceInterface.getInstance().getDeviceMap().keySet()) {
                DeviceGeneric device = DeviceInterface.getInstance().getDeviceByName(deviceName);

                // Vytvoř klíč ve formátu "connection/deviceId"
                String key = device.getConnectionName() + "/" + device.getName();

                // Získej seznam příkazů pro zařízení
                List<String> commands = device.getCmdList();

                // Přidej do mapy
                commandsResponse.put(key, commands);
            }

            ctx.json(commandsResponse);
        });

        // New endpoint for sending device commands
        app.post("/api/device/send-command", ctx -> {
            Map<String, String> data = ctx.bodyAsClass(Map.class);

            String deviceId = data.get("deviceId");
            String command = data.get("command");
            String value = data.get("value");

            // Lookup connection name for device
            DeviceGeneric device = DeviceInterface.getInstance().getDeviceByName(deviceId);
            if (device == null) {
                ctx.status(400).json(Map.of("error", "Device not found"));
                return;
            }

            device.cmdToDevice(command, value);

            //String connection = device.getConnectionName();
            // Construct command message string
//            String msg = "id:" + deviceId + ",msg:data,"+command+":"+value;
//            int crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
//            msg += String.format(",crc:%02x\r\n", (int) crc);
//            System.out.println(msg);
//            SerialCommunication.getInstance().sendCommand(connection, msg);
            ctx.status(200).json(Map.of("success", true, "message", "Command sent"));
        });
    }

    private void setupRestEndpointsConfig(Javalin app) {

        // GET
        app.get("/api/config", ctx -> {

            // GET XML configuration
            ConfXmlObject xmlConf = Configuration.getXMLObj();
            // result
            Map<String, Object> config = new HashMap<>();

            // MQTT
            Map<String, Object> mqtt = new HashMap<>();
            if (!xmlConf.getMQTTBroker().isEmpty()) {
                mqtt.put("address", xmlConf.getMQTTBroker().get(0).address);
                mqtt.put("username", xmlConf.getMQTTBroker().get(0).username);
                mqtt.put("password", xmlConf.getMQTTBroker().get(0).passwd);
                mqtt.put("topic", xmlConf.getMQTTBroker().get(0).topic);
                mqtt.put("selfsigned", xmlConf.getMQTTBroker().get(0).selfsigned);
                mqtt.put("cert", xmlConf.getMQTTBroker().get(0).cert);
            }
            config.put("mqtt", mqtt);

            // Home Assistant
            Map<String, Object> ha = new HashMap<>();
            if (!xmlConf.getHASettings().isEmpty()) {
                ha.put("enabled", xmlConf.getHASettings().get(0).enable);
                ha.put("topic", xmlConf.getHASettings().get(0).topic);
            }
            config.put("homeassistant", ha);

            // Connections
            List<Map<String, String>> connections = new ArrayList<>();
            for (ConfXmlObject.Connection connectionItem : xmlConf.getConnections()) {
                connections.add(Map.of("name", connectionItem.name, "portName", connectionItem.portName));
            }
            config.put("connections", connections);

            // Devices
            List<Map<String, String>> devices = new ArrayList<>();
            for (ConfXmlObject.Device deviceItem : xmlConf.getDevices()) {
                devices.add(Map.of("connection", deviceItem.connection, "name", deviceItem.name, "type", deviceItem.type));
            }
            config.put("devices", devices);

            ctx.json(config);
        });

        app.post("/api/config", ctx -> {
            try {
                Map<String, Object> config = ctx.bodyAsClass(Map.class);

                ConfXmlObject xmlObj = Configuration.getXMLObj();

                // ===== MQTT Configuration =====
                if (config.containsKey("mqtt")) {
                    Map<String, Object> mqttData = (Map<String, Object>) config.get("mqtt");
                    List<ConfXmlObject.MQTTBroker> brokers = new ArrayList<>();

                    ConfXmlObject.MQTTBroker broker = new ConfXmlObject.MQTTBroker();
                    broker.address = (String) mqttData.get("address");
                    broker.username = (String) mqttData.get("username");
                    broker.passwd = (String) mqttData.get("password");
                    broker.topic = (String) mqttData.get("topic");
                    broker.selfsigned = String.valueOf(mqttData.get("selfsigned"));
                    broker.cert = (String) mqttData.getOrDefault("cert", "");

                    brokers.add(broker);
                    xmlObj.setMQTTBroker(brokers);
                }

                // ===== Home Assistant Configuration =====
                if (config.containsKey("homeassistant")) {
                    Map<String, Object> haData = (Map<String, Object>) config.get("homeassistant");
                    List<ConfXmlObject.HASettings> haSettingsList = new ArrayList<>();

                    ConfXmlObject.HASettings haSetting = new ConfXmlObject.HASettings();
                    haSetting.enable = (Boolean) haData.get("enabled");
                    haSetting.topic = (String) haData.get("topic");

                    haSettingsList.add(haSetting);
                    xmlObj.setHASettings(haSettingsList);
                }

                // ===== Connections Configuration =====
                if (config.containsKey("connections")) {
                    List<Map<String, String>> connectionsData = (List<Map<String, String>>) config.get("connections");
                    List<ConfXmlObject.Connection> connList = new ArrayList<>();

                    for (Map<String, String> connData : connectionsData) {
                        ConfXmlObject.Connection conn = new ConfXmlObject.Connection();
                        conn.name = connData.get("name");
                        conn.portName = connData.get("portName");
                        connList.add(conn);
                    }

                    xmlObj.setConnections(connList);
                }

                // ===== Devices Configuration =====
                if (config.containsKey("devices")) {
                    List<Map<String, String>> devicesData = (List<Map<String, String>>) config.get("devices");
                    List<ConfXmlObject.Device> devList = new ArrayList<>();

                    for (Map<String, String> devData : devicesData) {
                        ConfXmlObject.Device device = new ConfXmlObject.Device();
                        device.name = devData.get("name");
                        device.connection = devData.get("connection");
                        device.type = devData.get("type");
                        devList.add(device);
                    }

                    xmlObj.setDevices(devList);
                }

                // Save to XML file
                Configuration.getInstance().saveToFile();

                ctx.status(200);
                ctx.json(Map.of("success", true, "message", "Configuration saved successfully"));

                // reload
                Devices.DeviceInterface.getInstance().reloadConfiguration();
                SerialCom.SerialCommunication.getInstance().reloadConfiguration();
                try {
                    MQTTinterface.getInstance().reloadConfiguration();
                } catch (Exception e) {
                    System.err.println("MQTT reload error: " + e.getMessage());
                }

            } catch (Exception e) {
                e.printStackTrace();
                ctx.status(500);
                ctx.json(Map.of("success", false, "message", "Error saving configuration: " + e.getMessage()));
            }
        });
    }

    private void setupRestEndpointsService(Javalin app) {
        app.post("/api/device/change-id", ctx -> {
            Map<String, String> data = ctx.bodyAsClass(Map.class);
            String connection = data.get("connection");
            String currentID = data.get("currentID");
            String newID = data.get("newID");
            String mode = data.get("mode"); // "full" nebo "50-50"

            if ("full".equals(mode)) {
                String msg = "id:" + currentID + ",msg:config,newId:" + newID;
                int crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
                msg += String.format(",crc:%02x\r\n", (int) crc);
                System.out.println(msg);
                SerialCommunication.getInstance().sendCommand(connection, msg);
            } else if ("50-50".equals(mode)) {
                String msg = "id:" + currentID + ",msg:config,newId5050:" + newID;
                int crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
                msg += String.format(",crc:%02x\r\n", (int) crc);
                System.out.println(msg);
                SerialCommunication.getInstance().sendCommand(connection, msg);
            }

            // Implementuj logiku pro změnu ID
            // mode: 'full' - normální změna
            // mode: '50-50' - bidirectionální komunikace s obě ID
            ctx.status(200);

        });

        ///api/firmware/list
        app.get("/api/firmware/list", ctx -> {

            Path fwBase = Paths.get("updateFW"); // working direcotry

            List<Map<String, Object>> devicesResponse = new ArrayList<>();

            if (Files.exists(fwBase) && Files.isDirectory(fwBase)) {

                // search in folders
                try (DirectoryStream<Path> deviceDirs = Files.newDirectoryStream(fwBase)) {
                    for (Path deviceDir : deviceDirs) {
                        if (!Files.isDirectory(deviceDir)) {
                            continue;
                        }

                        String deviceType = deviceDir.getFileName().toString();

                        List<String> versions = new ArrayList<>();

                        // search .hex in direcotry
                        try (DirectoryStream<Path> fwFiles = Files.newDirectoryStream(deviceDir, "*.hex")) {
                            for (Path fwFile : fwFiles) {
                                // název souboru bez přípony — např "01"
                                String filename = fwFile.getFileName().toString();
                                String version = filename;
                                versions.add(version);
                            }
                        }

                        // add result to result
                        if (!versions.isEmpty()) {
                            Map<String, Object> entry = new HashMap<>();
                            entry.put("deviceType", deviceType);
                            entry.put("versions", versions);

                            devicesResponse.add(entry);
                        }
                    }
                }
            }

            ctx.json(devicesResponse);
        });

        app.post("/api/firmware/update", ctx -> {
            Map<String, Object> body = ctx.bodyAsClass(Map.class);
            
            ctx.status(200);

            String deviceId = body.get("deviceId").toString();
            String deviceType = body.get("deviceType").toString();
            String version = body.get("version").toString();
            String connection = body.get("connection").toString();

            // Cesta k FW souboru
            Path fwPath = Paths.get("updateFW", deviceType, version );
            
            SerialCommunication.getInstance().getSerialUpdater().startUpdate(fwPath.toString(), connection, deviceId);

//            if (!Files.exists(fwPath)) {
//                ctx.status(400).json(Map.of(
//                        "error", "Firmware file not found: " + fwPath
//                ));
//                return;
//            }

            // --- SEM přijde LOGIKA skutečné aktualizace ---
            System.out.println("Updating device " + deviceId + " FW: " + fwPath );

            // mock na úspěch
            boolean success = true;

//            if (success) {
//                
//                ctx.json(Map.of(
//                        "status", "ok",
//                        "message", "Firmware update started",
//                        "file", fwPath.toString()
//                ));
//            } else {
//                ctx.status(500).json(Map.of(
//                        "error", "Update failed"
//                ));
//            }
        });
    
        app.get("api/firmware/progress", ctx -> {

            ctx.json(Map.of(
                "progress", SerialCommunication.getInstance().getSerialUpdater().getProgress(),
                "done", SerialCommunication.getInstance().getSerialUpdater().getDone(),
                "error", SerialCommunication.getInstance().getSerialUpdater().getError()
            ));
        });
    
    }

}
