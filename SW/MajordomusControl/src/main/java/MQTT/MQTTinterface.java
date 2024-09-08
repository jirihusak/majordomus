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

/**
 *
 * @author husak
 */
public class MQTTinterface extends Thread {

    private static MQTTinterface singletonInstace = null;
    private MqttClient client;
    private final Semaphore eventMutex = new Semaphore(0);
    private MQTTBroker mqttConfig = null;
    private Devices.DeviceInterface devices = DeviceInterface.getInstance();

    private class MqttData {

        String topic;
        String data;
        int qos;
        boolean retain;
    }
    private LinkedBlockingQueue<MqttData> publishQueue = new LinkedBlockingQueue<>();
    private LinkedBlockingQueue<MqttData> subscribeQueue = new LinkedBlockingQueue<>();

    static public MQTTinterface getInstance() {
        if (singletonInstace == null) {
            singletonInstace = new MQTTinterface();
        }
        return singletonInstace;
    }

    private MQTTinterface() {

        super();

        if (Configuration.getXMLObj().getMQTTBroker().size() >= 1) {
            mqttConfig = Configuration.getXMLObj().getMQTTBroker().get(0);
        } else {
            return;
        }

        start();

    }

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

    @Override
    public void run() {

        System.out.println("Run thread.....");

        try {
            client = new MqttClient(mqttConfig.address, MqttClient.generateClientId(), new MemoryPersistence());

//            java.util.Properties sslClientProperties = null;
//            try {
//                java.security.Security.addProvider(new AcceptAllProvider());
//                sslClientProperties = new Properties();
//                sslClientProperties.setProperty("com.ibm.ssl.trustManager", "TrustAllCertificates");
//                sslClientProperties.setProperty("com.ibm.ssl.trustStoreProvider", "AcceptAllProvider");
//            } catch (NoSuchAlgorithmException ex) {
//                Logger.getLogger(MQTTinterface.class.getName()).log(Level.SEVERE, null, ex);
//            } catch (KeyManagementException e) {
//
//            } catch (KeyStoreException e) {
//
//            }

            MqttConnectOptions options = new MqttConnectOptions();
            options.setUserName(mqttConfig.username);
            options.setPassword(mqttConfig.passwd.toCharArray());
            options.setConnectionTimeout(30);
            options.setKeepAliveInterval(5);
            options.setAutomaticReconnect(true);

//            if (sslClientProperties != null) {
//                options.setSSLProperties(sslClientProperties);
//            }

            client.setCallback(new MqttCallback() {

                @Override
                public void connectionLost(Throwable cause) {
                    System.out.println("connectionLost: " + cause.getMessage());
                }

                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    devices.parseIncomingMQTTData(topic, new String(message.getPayload()));
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken imdt) {
                    //throw new UnsupportedOperationException("Not supported yet."); // Generated from nbfs://nbhost/SystemFileSystem/Templates/Classes/Code/GeneratedMethodBody
                }
            });

            // connect
            client.connect(options);
            client.subscribe(mqttConfig.topic + "#", 0);

        } catch (MqttException e) {
            Logger.getLogger(MQTTinterface.class.getName()).log(Level.SEVERE, null, e);
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

                    System.out.println("Publish " + data.topic + ": " + msg);
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

    public boolean isConnected() {
        return client.isConnected();
    }

    public void publish(String topic, String data, boolean retain) {
        MqttData msg = new MqttData();
        msg.qos = 0;
        msg.topic = mqttConfig.topic + topic;
        msg.retain = retain;
        msg.data = data;

        publishQueue.add(msg);
        eventMutex.release();
    }

}
