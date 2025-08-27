/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package SerialCom;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortMessageListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

/**
 *
 * @author husak
 */
public abstract class SerialConnection {
    
    private SerialPort serialPort = null;
    private InputStream rxStream;
    private OutputStream txStream;
    String portName;
    private final byte[] delimiter = new byte[]{(byte) '\r', (byte) '\n'};
    //Timer readTimer = new Timer("serial read timer");
    //Scanner lineScanner;
    
    public ArrayList<String> getAvailablePorts() {
        ArrayList<String> portList = new ArrayList<>();

        for (SerialPort port : SerialPort.getCommPorts()) {
            portList.add(port.getSystemPortName());
        }

        return portList;
    }

    public void connect(String portDescriptor, int baudRate) {
        serialPort = SerialPort.getCommPort(portDescriptor);
        serialPort.setBaudRate(baudRate);
        serialPort.openPort();
        //serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
        portName = portDescriptor;

        //rxStream = serialPort.getInputStream();
        MessageListener listener = new MessageListener();
        serialPort.addDataListener(listener);
        txStream = serialPort.getOutputStream();
        rxStream = serialPort.getInputStream();
        //lineScanner = new Scanner(rxStream);
        //lineScanner.useDelimiter("\r\n");
        
//        readTimer.schedule(new TimerTask() {
//            @Override
//            public void run() {
//                
//                
//                String line = lineScanner.nextLine();
//                received(line);
//                
//                //System.out.print(line + "\r\n");
//            }
//        }, 0, 2); 
    }

    public void disconnect() {
        //lineScanner.close();
        //readTimer.cancel();
        //readTimer.purge();
        serialPort.closePort();
    }
    
    public String getPortName()
    {
        return portName;
    }

    public void send(String data) {
        
        if(txStream == null) return;
        
        try {
            txStream.write(data.getBytes());
            txStream.flush();
        } catch (IOException ex) {
            System.err.println(ex);
        }
    }
    
    public abstract void received(String data);
    
    
    
    private final class MessageListener implements SerialPortMessageListener {

        @Override
        public int getListeningEvents() {
            return SerialPort.LISTENING_EVENT_DATA_RECEIVED;
        }

        @Override
        public byte[] getMessageDelimiter() {
            return delimiter;
        }

        @Override
        public boolean delimiterIndicatesEndOfMessage() {
            return true;
        }

        @Override
        public void serialEvent(SerialPortEvent spe) {
            //try {
            //System.out.print((char)rxStream.read());
                byte[] delimitedMessage = spe.getReceivedData();
                String str = new String(delimitedMessage);
            //System.out.println("Received the following delimited message: " + str);
            //send(str);           
            //SerialCommunication.getInstance().receivedData(portName, str);
                received(str);
//            } catch (IOException ex) {
//                Logger.getLogger(SerialConnection.class.getName()).log(Level.SEVERE, null, ex);
//            }
        }
    }
    
}
