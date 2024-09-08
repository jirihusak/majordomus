/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package SerialCom;

import GUI.P120_UpdateController;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Base64;
import java.util.Map;
import java.util.Scanner;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 *
 * @author husak
 */
public class SerialUpdater {
    
    Thread updateWorker;
    
    Map<Long, byte[]> memoryBlocks; // block with 128 B data
    final int memoryBlockSize = 128;
    long actualAddressOffset = 0;
    Pattern linePattern;
   
    
    public SerialUpdater()
    {
        memoryBlocks = new TreeMap<>();
        linePattern = Pattern.compile(":([0-9A-Fa-f]{2})([0-9A-Fa-f]{4})([0-9]{2})([0-9A-Fa-f]*)([0-9A-Fa-f]{2})([\\r\\n]?)");
        
        //startUpdate("d:\\Prace\\Development\\Systems\\SmartBuilding\\FW\\PIC18\\DemoApp\\DemoApp.X\\dist\\Release\\production\\DemoApp.X.production.hex", "1-NP", "test");
    }
    
    
    public void startUpdate(String file, String connection, String deviceName)
    {
        updateWorker = new Thread(){
            @Override
            public void run(){
                //String file = "d:\\Prace\\Development\\Systems\\SmartBuilding\\FW\\PIC18\\DemoApp\\DemoApp.X\\dist\\Release\\production\\DemoApp.X.production.hex";
                parseHexFile(file);
                SerialCommunication.getInstance().stopDataPooling();
                sendFileOverSerial(connection, deviceName);
                SerialCommunication.getInstance().startDataPooling();
            }           
        };
        updateWorker.start();
    }
    
    public void stopUpdate()
    {
        updateWorker.interrupt();
    }
    
    
    private void parseHexFile(String file){
        
        memoryBlocks.clear();
        actualAddressOffset = 0;
        
        try {
            FileInputStream fis = new FileInputStream(file);
            Scanner sc = new Scanner(fis);    //file to be scanned  
            //returns true if there is another line to read
            while(sc.hasNextLine())
            {
                String line = sc.nextLine();
                //System.out.println(line);      //returns the line that was skipped
                hexLineProcess(line);
            }   
            sc.close();     //closes the scanner  
        } catch (FileNotFoundException ex) {
            Logger.getLogger(SerialUpdater.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void hexLineProcess(String line)
    {
        // chceck if line is correct
        Matcher m = linePattern.matcher(line);  
        
        if(!m.matches()) {
            return;
        }
        //System.out.println(m.matches() + "size " +  m.group(1) + "address " + m.group(2) + "type " + m.group(3) + "data " + m.group(4) + "chksum " + m.group(5));
        int size = Integer.parseInt(m.group(1), 16);
        int address = Integer.parseInt(m.group(2), 16);
        int type = Integer.parseInt(m.group(3), 16);
        byte[] data = hexStringToByteArray(m.group(4));
        int checksum = Integer.parseInt(m.group(5), 16);
        
        long wholeAddress = (actualAddressOffset * 0x10000) + address;
        
        if(m.matches())
        {
            switch(type)
            {
                // DATA
                case 0:
                    
                    int processedSize = 0;
                    long mapIndex = wholeAddress / memoryBlockSize;
                    long offset = wholeAddress % memoryBlockSize;
                    
                    while(processedSize < size)
                    {
                        //new item
                        if(!memoryBlocks.containsKey(mapIndex))
                        {
                            byte[] emptyArray = new byte[memoryBlockSize];
                            for (int i = 0; i < memoryBlockSize; i++) {
                                emptyArray[i] = (byte)0xFF;
                            }
                            memoryBlocks.put(mapIndex, emptyArray);
                        }

//                        System.out.println(offset + "--------"+m.group(4));
//                        for(int i = 0; i < data.length; i++)
//                        {
//                            System.out.print(String.format("%02X ", data[i]));
//                        }
//                        System.out.println();

                        byte[] editArray = memoryBlocks.get(mapIndex);
                        for (int i = 0 ;processedSize < data.length && (offset + processedSize) < memoryBlockSize; processedSize++, i++) {
                            editArray[(int)offset + i] = data[processedSize];
                        }

                        // data continues to next block [128 B]
                        mapIndex++;
                        offset = 0;
                    }
                    break;
                    
                // EOF
                case 1:
                    break;
                    
                // extended address
                case 4:
                    actualAddressOffset = Long.parseLong(m.group(4), 16);
                    System.out.println("Change base address " + actualAddressOffset);
                    break;
            }
            
        }
        else {
            System.out.println("Hex File contains unexpected data");
        }  
    }
    
    /* s must be an even-length string. */
    private static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                                 + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }
    
    private final int[] crc8_table = {
    0xea, 0xd4, 0x96, 0xa8, 0x12, 0x2c, 0x6e, 0x50, 0x7f, 0x41, 0x03, 0x3d,
    0x87, 0xb9, 0xfb, 0xc5, 0xa5, 0x9b, 0xd9, 0xe7, 0x5d, 0x63, 0x21, 0x1f,
    0x30, 0x0e, 0x4c, 0x72, 0xc8, 0xf6, 0xb4, 0x8a, 0x74, 0x4a, 0x08, 0x36,
    0x8c, 0xb2, 0xf0, 0xce, 0xe1, 0xdf, 0x9d, 0xa3, 0x19, 0x27, 0x65, 0x5b,
    0x3b, 0x05, 0x47, 0x79, 0xc3, 0xfd, 0xbf, 0x81, 0xae, 0x90, 0xd2, 0xec,
    0x56, 0x68, 0x2a, 0x14, 0xb3, 0x8d, 0xcf, 0xf1, 0x4b, 0x75, 0x37, 0x09,
    0x26, 0x18, 0x5a, 0x64, 0xde, 0xe0, 0xa2, 0x9c, 0xfc, 0xc2, 0x80, 0xbe,
    0x04, 0x3a, 0x78, 0x46, 0x69, 0x57, 0x15, 0x2b, 0x91, 0xaf, 0xed, 0xd3,
    0x2d, 0x13, 0x51, 0x6f, 0xd5, 0xeb, 0xa9, 0x97, 0xb8, 0x86, 0xc4, 0xfa,
    0x40, 0x7e, 0x3c, 0x02, 0x62, 0x5c, 0x1e, 0x20, 0x9a, 0xa4, 0xe6, 0xd8,
    0xf7, 0xc9, 0x8b, 0xb5, 0x0f, 0x31, 0x73, 0x4d, 0x58, 0x66, 0x24, 0x1a,
    0xa0, 0x9e, 0xdc, 0xe2, 0xcd, 0xf3, 0xb1, 0x8f, 0x35, 0x0b, 0x49, 0x77,
    0x17, 0x29, 0x6b, 0x55, 0xef, 0xd1, 0x93, 0xad, 0x82, 0xbc, 0xfe, 0xc0,
    0x7a, 0x44, 0x06, 0x38, 0xc6, 0xf8, 0xba, 0x84, 0x3e, 0x00, 0x42, 0x7c,
    0x53, 0x6d, 0x2f, 0x11, 0xab, 0x95, 0xd7, 0xe9, 0x89, 0xb7, 0xf5, 0xcb,
    0x71, 0x4f, 0x0d, 0x33, 0x1c, 0x22, 0x60, 0x5e, 0xe4, 0xda, 0x98, 0xa6,
    0x01, 0x3f, 0x7d, 0x43, 0xf9, 0xc7, 0x85, 0xbb, 0x94, 0xaa, 0xe8, 0xd6,
    0x6c, 0x52, 0x10, 0x2e, 0x4e, 0x70, 0x32, 0x0c, 0xb6, 0x88, 0xca, 0xf4,
    0xdb, 0xe5, 0xa7, 0x99, 0x23, 0x1d, 0x5f, 0x61, 0x9f, 0xa1, 0xe3, 0xdd,
    0x67, 0x59, 0x1b, 0x25, 0x0a, 0x34, 0x76, 0x48, 0xf2, 0xcc, 0x8e, 0xb0,
    0xd0, 0xee, 0xac, 0x92, 0x28, 0x16, 0x54, 0x6a, 0x45, 0x7b, 0x39, 0x07,
    0xbd, 0x83, 0xc1, 0xff};

    private int crc8(int seed, String data)
    {
        int crc = seed;

        if (data == null)
            return 0;
        
        crc &= 0xff;
        
        for(int i = 0; i < data.length(); i++)
        {
            crc = crc8_table[crc ^ (data.getBytes()[i] & 0xFF )];
        }
        
        return crc & 0xFF;
    }

    
    
    private void sendFileOverSerial(String connection, String deviceName){
        
        String msg;
        
        msg = "id:" + deviceName + ",msg:config,cmd:reset";
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int)crc);
        System.out.println(msg);
        SerialCommunication.getInstance().sendAsyncMsg(connection, msg);
        
        try {
            Thread.sleep(500);
        } catch (InterruptedException ex) {
            return;
        }
        
        msg = "id:" + deviceName + ",msg:bl,cmd:erase";
        crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int)crc);
        System.out.println(msg);
        SerialCommunication.getInstance().sendAsyncMsg(connection, msg);
        
        try {
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
            return;
        }
        
        double counter = 0;
        for (long address : memoryBlocks.keySet()) {
            byte[] ba = memoryBlocks.get(address);
            
            msg = "id:" + deviceName + ",msg:bl,addr:"  + address + ",data:" + Base64.getEncoder().encodeToString(ba);
            crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
            msg += String.format(",crc:%02x\r\n", (int)crc);
            System.out.println(java.time.LocalDateTime.now() + ":" + msg);
            SerialCommunication.getInstance().sendAsyncMsg(connection, msg);
            
            try {
                Thread.sleep(200);
            } catch (InterruptedException ex) {
                return;
            }
            
            P120_UpdateController.getInstance().setUpdateProgessBar((counter / (double)memoryBlocks.size()) );
            counter++;
            
        }
        msg = "id:" + deviceName + ",msg:bl,cmd:jumpToApp";
        crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int)crc);
        System.out.println(msg);
        SerialCommunication.getInstance().sendAsyncMsg(connection, msg);
        
        P120_UpdateController.getInstance().setUpdateProgessBar(100);
                
    }
}
