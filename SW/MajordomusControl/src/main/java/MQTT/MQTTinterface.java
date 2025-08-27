/*
http://www.mqtt-dashboard.com/#  -test mqtt broker

 */
package MQTT;

import Configuration.ConfXmlObject.MQTTBroker;
import Configuration.Configuration;
import Devices.DeviceInterface;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.util.Properties;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManagerFactory;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import javax.net.ssl.SSLSocketFactory;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;



/**
 *
 * @author husak
 */
public class MQTTinterface extends Thread {

    // Singleton instance to ensure only one MQTTinterface is created
    private static MQTTinterface singletonInstace = null;
    private MqttClient client;
    private final Semaphore eventMutex = new Semaphore(0); // Semaphore for synchronizing publishing events
    private MQTTBroker mqttConfig = null;
    private Devices.DeviceInterface devices = DeviceInterface.getInstance();

    // Class to store MQTT data that will be published
    private class MqttData {

        String topic;
        String data;
        int qos;
        boolean retain;
    }

    // Queue for storing messages to be published or subscribed to
    private LinkedBlockingQueue<MqttData> publishQueue = new LinkedBlockingQueue<>();
    private LinkedBlockingQueue<MqttData> subscribeQueue = new LinkedBlockingQueue<>();

    // Get singleton instance of MQTTinterface
    static public MQTTinterface getInstance() {
        if (singletonInstace == null) {
            singletonInstace = new MQTTinterface();
        }
        return singletonInstace;
    }

    // Constructor initializes the MQTT client configuration
    private MQTTinterface() {

        super();

        if (Configuration.getXMLObj().getMQTTBroker().size() >= 1) {
            mqttConfig = Configuration.getXMLObj().getMQTTBroker().get(0);
        } else {
            return;
        }

        start(); // Start the thread to handle MQTT connection

    }

    // Clean up and disconnect MQTT client when object is garbage collected
    @Override
    protected void finalize() {
        try {
            client.disconnect();
            // close client
            client.close();
        } catch (MqttException ex) {
            Logger.getLogger(MQTTinterface.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    // Main run method for the thread, sets up and manages MQTT connection
    @Override
    public void run() {

        System.out.println("Run thread.....");

        try {

            SSLSocketFactory socketFactory = null;
            // Check if the connection is SSL and set up SSL socket factory if needed
            if (mqttConfig.address.startsWith("ssl")) {
                System.out.println("SSL connection detected. Setting up SSL socket factory...");
                if ("true".equalsIgnoreCase(mqttConfig.selfsigned)) {
                    System.out.println("Using self-signed SSL certificate.");
                    socketFactory = getSelfSignedSocketFactory(mqttConfig.cert);
                } else {
                    System.out.println("Using default SSL certificate validation.");
                    socketFactory = (SSLSocketFactory) SSLSocketFactory.getDefault();
                }
            } else {
                System.out.println("TCP connection detected.");
            }

            // Create the MQTT client
            System.out.println("Creating MQTT client...");
            client = new MqttClient(mqttConfig.address, MqttClient.generateClientId(), new MemoryPersistence());


            MqttConnectOptions options = new MqttConnectOptions();
            options.setUserName(mqttConfig.username);
            options.setPassword(mqttConfig.passwd.toCharArray());
            options.setConnectionTimeout(30);
            options.setKeepAliveInterval(5);
            options.setAutomaticReconnect(true);

            if (socketFactory != null) {
                options.setSocketFactory(socketFactory);
                System.out.println("SSL Socket Factory set successfully.");
            }
            
            client.setCallback(new MqttCallback() {

                @Override
                public void connectionLost(Throwable cause) {
                    System.out.println("connectionLost: " + cause.getMessage());
                }

                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    //System.out.println("Incoming msg: " + topic + " : " + message);
                    devices.parseIncomingMQTTData(topic, new String(message.getPayload()));
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken imdt) {
                    //throw new UnsupportedOperationException("Not supported yet."); // Generated from nbfs://nbhost/SystemFileSystem/Templates/Classes/Code/GeneratedMethodBody
                }
            });

            // Connect to the broker with the configured options
            System.out.println("Connecting to MQTT broker at: " + mqttConfig.address);
            client.connect(options);
            System.out.println("Connected to broker successfully.");

            // Subscribe to the topic specified in the configuration
            System.out.println("Subscribing to topic: " + mqttConfig.topic + "#");
            client.subscribe(mqttConfig.topic + "#", 0);
            System.out.println("Subscription successful.");

        } catch (MqttException e) {

            System.err.println("MQTT Exception occurred: " + e.getMessage());
            e.printStackTrace(); // Handle MQTT specific exceptions
            Logger.getLogger(MQTTinterface.class.getName()).log(Level.SEVERE, null, e);

        } catch (Exception e) {

            System.err.println("General Exception occurred: " + e.getMessage());
            e.printStackTrace(); // Handle general exceptions

        }

        while (true) {
            try {
                eventMutex.acquire();

                while (!publishQueue.isEmpty()) {
                    MqttData data = publishQueue.take();

                    MqttMessage msg = new MqttMessage();
                    msg.setQos(data.qos);
                    msg.setRetained(data.retain);
                    msg.setPayload(data.data.getBytes());

                    //System.out.println("Publish " + data.topic + ": " + msg);
                    client.publish(data.topic, msg);
                }

            } catch (InterruptedException ex) {
                //Logger.getLogger(LightingModule.class.getName()).log(Level.SEVERE, null, ex);
            } catch (NullPointerException e) {

            } catch (MqttException ex) {
                Logger.getLogger(MQTTinterface.class.getName()).log(Level.SEVERE, null, ex);
            }

        }
    }

    // Method to check if the MQTT client is connected
    public boolean isConnected() {
        return client.isConnected();
    }

    // Method to publish a message to the MQTT broker
    public void publish(String topic, String data, boolean retain) {
        MqttData msg = new MqttData();
        msg.qos = 0;
        msg.topic = mqttConfig.topic + topic;
        msg.retain = retain;
        msg.data = data;

        publishQueue.add(msg);
        eventMutex.release();
    }

    // Method to create an SSL socket factory for self-signed certificates
    public static SSLSocketFactory getSelfSignedSocketFactory(final String caCrtFile) throws Exception {
        System.out.println("Loading CA certificate from: " + caCrtFile);
        CertificateFactory cf = CertificateFactory.getInstance("X.509"); // Create certificate factory for X.509
        FileInputStream fis = new FileInputStream(caCrtFile); // Load the CA certificate file
        BufferedInputStream bis = new BufferedInputStream(fis);
        X509Certificate caCert = null;
        if (bis.available() > 0) {
            caCert = (X509Certificate) cf.generateCertificate(bis); // Generate CA certificate from file
            System.out.println("CA Certificate loaded successfully.");
        }
        bis.close();

        if (caCert == null) {
            throw new IllegalArgumentException("CA Certificate could not be loaded"); // Error if CA certificate is not loaded
        }

        // Create a KeyStore to hold the CA certificate
        System.out.println("Creating KeyStore and adding CA certificate...");
        KeyStore caKs = KeyStore.getInstance(KeyStore.getDefaultType());
        caKs.load(null, null); // Load the empty keystore
        caKs.setCertificateEntry("ca-certificate", caCert); // Set the CA certificate entry in the keystore

        // Initialize a TrustManagerFactory with the CA keystore
        System.out.println("Initializing TrustManagerFactory with CA certificate...");
        TrustManagerFactory tmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        tmf.init(caKs);

        // Create SSL context and initialize it with the TrustManager
        System.out.println("Creating SSL context...");
        SSLContext context = SSLContext.getInstance("TLSv1.2"); // Use TLS v1.2 for secure communication
        context.init(null, tmf.getTrustManagers(), null);

        System.out.println("SSL context initialized successfully.");
        // Return the configured SSL socket factory
        return context.getSocketFactory();
    }

}
