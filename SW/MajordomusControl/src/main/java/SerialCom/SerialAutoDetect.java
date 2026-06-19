/*
 * MajordomusControl - Home Automation Gateway
 * Auto-detection of RS-485 devices on all available serial ports.
 * Sends a broadcast status request (id:all,msg:status) on each port,
 * repeats it SCAN_ROUNDS times to handle random-delay collisions,
 * and collects the id/type from every responding device.
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
 */
package SerialCom;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortMessageListener;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SerialAutoDetect {

    public static final class DiscoveredDevice {
        public final String id;
        public final String type;

        public DiscoveredDevice(String id, String type) {
            this.id = id;
            this.type = type;
        }
    }

    public static final class ScanResult {
        public final String portName;
        public final List<DiscoveredDevice> devices;

        public ScanResult(String portName, List<DiscoveredDevice> devices) {
            this.portName = portName;
            this.devices = Collections.unmodifiableList(new ArrayList<>(devices));
        }
    }

    /** Number of broadcast rounds per port (handles collision between devices). */
    private static final int SCAN_ROUNDS = 5;
    /** Wait time (ms) after each broadcast for devices to respond. */
    private static final int ROUND_WAIT_MS = 1500;
    private static final int BAUD_RATE = 115200;

    /**
     * Scans all available serial ports for Majordomus RS-485 devices.
     * Temporarily stops existing serial connections, scans, then restores them.
     *
     * @return list of ScanResult, one entry per port where at least one device was found
     */
    public static List<ScanResult> scan() {
        SerialCommunication sc = SerialCommunication.getInstance();

        // Stop all active connections so their ports can be re-opened for scanning
        sc.stopAllConnections();

        List<ScanResult> results = new ArrayList<>();
        boolean isLinux = System.getProperty("os.name").toLowerCase().contains("linux");

        for (SerialPort portInfo : SerialPort.getCommPorts()) {
            String portName = portInfo.getSystemPortName();

            // On Linux skip non-USB serial ports (/dev/ttyS*, etc.)
            if (isLinux && !portName.startsWith("ttyUSB") && !portName.startsWith("ttyACM")) {
                continue;
            }

            System.out.println("AutoDetect: scanning " + portName);
            List<DiscoveredDevice> found = scanPort(portName, sc);
            if (!found.isEmpty()) {
                results.add(new ScanResult(portName, found));
                System.out.println("AutoDetect: " + portName + " -> " + found.size() + " device(s)");
            }
        }

        // Restore connections with existing configuration
        sc.restartAllConnections();

        return results;
    }

    private static List<DiscoveredDevice> scanPort(String portName, SerialCommunication sc) {
        // id -> type, LinkedHashMap preserves discovery order
        Map<String, String> deviceMap = new LinkedHashMap<>();
        CopyOnWriteArrayList<String> receivedLines = new CopyOnWriteArrayList<>();

        SerialPort port = SerialPort.getCommPort(portName);
        port.setBaudRate(BAUD_RATE);

        if (!port.openPort()) {
            System.out.println("AutoDetect: cannot open " + portName + " - skipped");
            return Collections.emptyList();
        }

        byte[] delimiter = {'\r', '\n'};
        port.addDataListener(new SerialPortMessageListener() {
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
            public void serialEvent(SerialPortEvent e) {
                receivedLines.add(new String(e.getReceivedData()));
            }
        });

        try {
            OutputStream out = port.getOutputStream();

            for (int round = 0; round < SCAN_ROUNDS; round++) {
                String msg = "id:all,msg:status";
                int crc = sc.crc8(0, msg.toCharArray(), msg.length());
                String fullMsg = msg + String.format(",crc:%02x\r\n", crc);

                out.write(fullMsg.getBytes());
                out.flush();

                Thread.sleep(ROUND_WAIT_MS);

                // Drain collected lines into local list for parsing
                List<String> batch = new ArrayList<>(receivedLines);
                receivedLines.clear();

                for (String line : batch) {
                    parseStatusResponse(line, deviceMap, sc);
                }
            }
        } catch (IOException e) {
            System.err.println("AutoDetect: IO error on " + portName + ": " + e.getMessage());
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        } finally {
            port.closePort();
        }

        List<DiscoveredDevice> result = new ArrayList<>();
        for (Map.Entry<String, String> entry : deviceMap.entrySet()) {
            result.add(new DiscoveredDevice(entry.getKey(), entry.getValue()));
        }
        return result;
    }

    // Matches "id:NAME,type:TYPE" anywhere in a line, even surrounded by garbage bytes.
    // NAME: alphanumeric, hyphens, underscores. TYPE: alphanumeric only, terminated by ',' or end-of-string.
    private static final Pattern ID_TYPE_PATTERN =
            Pattern.compile("id:([A-Za-z0-9_-]+),type:([A-Za-z0-9]+)(?=[,\\r\\n]|$)");

    /**
     * Parses a status response line and adds the device to deviceMap.
     * First tries strict CRC validation; on mismatch falls back to regex extraction
     * to handle garbled lines caused by simultaneous RS-485 transmissions.
     */
    private static void parseStatusResponse(String line, Map<String, String> deviceMap, SerialCommunication sc) {
        String trimmed = line.trim();

        // --- Primary path: CRC validation ---
        int crcIdx = trimmed.lastIndexOf(",crc:");
        if (crcIdx >= 0) {
            String payload = trimmed.substring(0, crcIdx);
            String crcHex = trimmed.substring(crcIdx + 5).replaceAll("[^0-9a-fA-F]", "");
            if (!crcHex.isEmpty()) {
                try {
                    int receivedCrc = Integer.parseInt(crcHex, 16) & 0xFF;
                    int calcCrc = sc.crc8(0, payload.toCharArray(), payload.length()) & 0xFF;
                    if (receivedCrc == calcCrc) {
                        extractIdType(payload, deviceMap);
                        return;
                    }
                } catch (NumberFormatException ignored) {
                }
            }
        }

        // --- Fallback: regex scan for id/type in garbled data ---
        Matcher m = ID_TYPE_PATTERN.matcher(trimmed);
        while (m.find()) {
            String id   = m.group(1);
            String type = m.group(2);
            String existing = deviceMap.get(id);
            // Only store if this device was not seen yet, or the new type is longer (more complete match)
            if (existing == null || type.length() > existing.length()) {
                System.out.println("AutoDetect: regex recovery -> id:" + id + " type:" + type);
                deviceMap.put(id, type);
            }
        }
    }

    private static void extractIdType(String payload, Map<String, String> deviceMap) {
        String id = null;
        String type = null;
        for (String token : payload.split(",")) {
            if (token.startsWith("id:")) {
                id = token.substring(3).trim();
            } else if (token.startsWith("type:")) {
                type = token.substring(5).trim();
            }
        }
        if (id != null && !id.isEmpty() && type != null && !type.isEmpty()) {
            deviceMap.put(id, type);
        }
    }
}
