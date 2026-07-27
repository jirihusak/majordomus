/*
 * MajordomusControl - Home Automation Gateway
 * Auto-detection of RS-485 devices on all available serial ports.
 *
 * Two-phase scan:
 *   Phase A (broadcast) - sends "id:all,msg:status" repeatedly; every device
 *       answers after its own random back-off, so frames collide and some
 *       devices can stay unheard for several rounds. Rounds therefore repeat
 *       adaptively (until nothing new appears) with randomized spacing so the
 *       same pair of devices does not collide over and over again.
 *   Phase B (unicast) - every id that was ever glimpsed (even inside a garbled
 *       frame) plus every id known from the configuration is asked directly.
 *       Unicast answers cannot collide, so this deterministically confirms the
 *       device and its exact type.
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

import Configuration.ConfXmlObject;
import Configuration.Configuration;
import com.fazecast.jSerialComm.SerialPort;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
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

    /** Minimum number of broadcast rounds per port. */
    private static final int MIN_ROUNDS = 3;
    /** Hard upper limit of broadcast rounds per port. */
    private static final int MAX_ROUNDS = 10;
    /** Stop broadcasting after this many consecutive rounds without a new device. */
    private static final int STABLE_ROUNDS = 3;
    /** Wait time (ms) after each broadcast for devices to respond. */
    private static final int ROUND_WAIT_MS = 1200;
    /** Extra randomized spacing (0..N ms) before each broadcast - breaks repeating collisions. */
    private static final int ROUND_JITTER_MS = 250;
    /** Wait time (ms) for a single unicast answer. */
    private static final int UNICAST_WAIT_MS = 400;
    /** Attempts per unicast candidate. */
    private static final int UNICAST_TRIES = 3;
    private static final int BAUD_RATE = 115200;

    private static final Random RND = new Random();

    /** How a device id was confirmed - CRC-valid frame vs. recovery from garbled data. */
    private enum Source { CRC, REGEX }

    private static final class Entry {
        String type;
        Source source;
        int seen;
        String firstRound;
    }

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

        // Give the OS a moment to release the closed port handles (mainly Windows)
        sleep(500);

        List<ScanResult> results = new ArrayList<>();
        boolean isLinux = System.getProperty("os.name").toLowerCase().contains("linux");

        for (SerialPort portInfo : SerialPort.getCommPorts()) {
            String portName = portInfo.getSystemPortName();

            // On Linux skip non-USB serial ports (/dev/ttyS*, etc.)
            if (isLinux && !portName.startsWith("ttyUSB") && !portName.startsWith("ttyACM")) {
                continue;
            }

            log(portName, "=== scan start (rounds " + MIN_ROUNDS + ".." + MAX_ROUNDS
                    + ", wait " + ROUND_WAIT_MS + " ms, unicast verify on) ===");
            List<DiscoveredDevice> found = scanPort(portName, sc);
            if (!found.isEmpty()) {
                results.add(new ScanResult(portName, found));
            }
        }

        // Restore connections with existing configuration
        sc.restartAllConnections();

        return results;
    }

    private static List<DiscoveredDevice> scanPort(String portName, SerialCommunication sc) {
        // id -> entry, LinkedHashMap preserves discovery order
        Map<String, Entry> deviceMap = new LinkedHashMap<>();
        // ids glimpsed anywhere (even without a readable type) - verified later by unicast
        Set<String> candidates = new LinkedHashSet<>();

        SerialPort port = SerialPort.getCommPort(portName);
        port.setBaudRate(BAUD_RATE);
        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 100, 0);

        // Retry a few times - Windows releases a just-closed port handle with a delay
        boolean opened = false;
        for (int attempt = 1; attempt <= 5 && !opened; attempt++) {
            opened = port.openPort();
            if (!opened && attempt < 5) {
                log(portName, "port busy, retry " + attempt + "/5");
                sleep(400);
            }
        }
        if (!opened) {
            log(portName, "cannot open - skipped (jSerialComm error " + port.getLastErrorCode()
                    + " at line " + port.getLastErrorLocation() + ")");
            return Collections.emptyList();
        }

        try {
            port.flushIOBuffers();
            OutputStream out = port.getOutputStream();

            // ---------- Phase A: broadcast rounds ----------
            int stable = 0;
            int round = 0;
            while (round < MAX_ROUNDS) {
                round++;
                String tag = "R" + round;

                // randomized spacing so devices do not repeat the same collision pattern
                sleep(RND.nextInt(ROUND_JITTER_MS + 1));

                int before = deviceMap.size();
                send(out, "id:all,msg:status", sc, portName, tag);

                List<String> lines = collect(port, ROUND_WAIT_MS);
                int crcOk = 0;
                int garbled = 0;
                for (String line : lines) {
                    log(portName, tag + " RX \"" + esc(line) + "\"");
                    if (parseLine(line, deviceMap, candidates, sc, portName, tag)) {
                        crcOk++;
                    } else {
                        garbled++;
                    }
                }

                int added = deviceMap.size() - before;
                log(portName, tag + " summary: lines=" + lines.size() + " crcOk=" + crcOk
                        + " garbled=" + garbled + " known=" + deviceMap.size()
                        + " (+" + added + " new) candidates=" + candidates.size());

                if (added == 0) {
                    stable++;
                } else {
                    stable = 0;
                }
                if (round >= MIN_ROUNDS && stable >= STABLE_ROUNDS) {
                    break;
                }
            }
            log(portName, "broadcast phase done after " + round + " round(s): "
                    + deviceMap.size() + " device(s), " + candidates.size() + " candidate id(s)");

            // ---------- Phase B: unicast verification ----------
            // Candidates = everything glimpsed on the bus + everything already in the
            // configuration for this port. Unicast answers cannot collide, so a device
            // that lost every broadcast round is still found here.
            Set<String> toVerify = new LinkedHashSet<>(candidates);
            toVerify.addAll(deviceMap.keySet());
            for (String cfgId : configuredIdsForPort(portName)) {
                if (toVerify.add(cfgId)) {
                    log(portName, "candidate from configuration: " + cfgId);
                }
            }

            log(portName, "unicast verify of " + toVerify.size() + " id(s): " + toVerify);
            for (String id : toVerify) {
                Entry e = deviceMap.get(id);
                if (e != null && e.source == Source.CRC) {
                    log(portName, "unicast " + id + " -> skipped (already CRC confirmed as "
                            + e.type + ")");
                    continue;
                }

                boolean confirmed = false;
                for (int t = 1; t <= UNICAST_TRIES && !confirmed; t++) {
                    port.flushIOBuffers();
                    send(out, "id:" + id + ",msg:status", sc, portName, "U" + t);

                    for (String line : collect(port, UNICAST_WAIT_MS)) {
                        log(portName, "U" + t + " RX \"" + esc(line) + "\"");
                        parseLine(line, deviceMap, candidates, sc, portName, "U" + t);
                        // unicast cannot collide - only a clean CRC answer counts
                        Entry now = deviceMap.get(id);
                        if (now != null && now.source == Source.CRC) {
                            confirmed = true;
                        }
                    }
                    if (!confirmed) {
                        log(portName, "unicast " + id + " try " + t + "/" + UNICAST_TRIES
                                + " -> no valid answer");
                    }
                }
                if (confirmed) {
                    Entry now = deviceMap.get(id);
                    log(portName, "unicast " + id + " -> OK type:" + (now != null ? now.type : "?"));
                } else if (deviceMap.containsKey(id)) {
                    log(portName, "unicast " + id + " -> no answer, but seen in broadcast - kept");
                } else {
                    log(portName, "unicast " + id + " -> no answer, dropped (probably garbled id)");
                }
            }
        } catch (IOException e) {
            System.err.println("AutoDetect[" + portName + "] IO error: " + e.getMessage());
        } finally {
            port.closePort();
        }

        List<DiscoveredDevice> result = new ArrayList<>();
        for (Map.Entry<String, Entry> en : deviceMap.entrySet()) {
            result.add(new DiscoveredDevice(en.getKey(), en.getValue().type));
        }

        log(portName, "=== result: " + result.size() + " device(s) ===");
        for (Map.Entry<String, Entry> en : deviceMap.entrySet()) {
            Entry e = en.getValue();
            log(portName, String.format("   %-20s type=%-12s seen=%dx via=%s first=%s",
                    en.getKey(), e.type, e.seen, e.source, e.firstRound));
        }
        return result;
    }

    // ------------------------------------------------------------------ IO --

    private static void send(OutputStream out, String msg, SerialCommunication sc,
                             String portName, String tag) throws IOException {
        int crc = sc.crc8(0, msg.toCharArray(), msg.length());
        String fullMsg = msg + String.format(",crc:%02x\r\n", crc);
        log(portName, tag + " TX \"" + esc(fullMsg) + "\"");
        out.write(fullMsg.getBytes());
        out.flush();
    }

    /**
     * Reads raw bytes for the given time and splits them into lines itself.
     * Deliberately does not use SerialPortMessageListener: a collision that
     * destroys a CRLF would make that listener glue frames together (or hold
     * them in its internal buffer forever). Here a partial trailing frame is
     * still flushed at the end of the window, so its id can be recovered.
     */
    private static List<String> collect(SerialPort port, long waitMs) {
        List<String> lines = new ArrayList<>();
        StringBuilder acc = new StringBuilder();
        byte[] buf = new byte[512];
        long deadline = System.currentTimeMillis() + waitMs;

        while (System.currentTimeMillis() < deadline) {
            int avail = port.bytesAvailable();
            if (avail < 0) {
                break; // port disappeared
            }
            if (avail == 0) {
                sleep(5);
                continue;
            }
            int n = port.readBytes(buf, Math.min(buf.length, avail));
            for (int i = 0; i < n; i++) {
                char c = (char) (buf[i] & 0xFF);
                if (c == '\r' || c == '\n') {
                    if (acc.length() > 0) {
                        lines.add(acc.toString());
                        acc.setLength(0);
                    }
                } else {
                    acc.append(c);
                }
            }
        }
        if (acc.length() > 0) {
            lines.add(acc.toString()); // frame without terminator - still worth parsing
        }
        return lines;
    }

    // -------------------------------------------------------------- parsing --

    // Matches "id:NAME,type:TYPE" anywhere in a line, even surrounded by garbage bytes.
    private static final Pattern ID_TYPE_PATTERN =
            Pattern.compile("id:([A-Za-z0-9_-]+),type:([A-Za-z0-9]+)(?=[,\\r\\n]|$)");
    // Any id token - used only to build unicast candidates from garbled frames.
    private static final Pattern ID_PATTERN =
            Pattern.compile("id:([A-Za-z0-9_-]{2,})");

    /**
     * Parses one received line. Strict CRC validation first; on mismatch falls
     * back to regex extraction of id/type pairs, and finally collects bare ids
     * as unicast candidates.
     *
     * @return true if the line passed CRC validation
     */
    private static boolean parseLine(String line, Map<String, Entry> deviceMap,
                                     Set<String> candidates, SerialCommunication sc,
                                     String portName, String tag) {
        String trimmed = line.trim();
        if (trimmed.isEmpty()) {
            return false;
        }

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
                        collectCandidates(payload, candidates);
                        extractIdType(payload, deviceMap, Source.CRC, portName, tag);
                        return true;
                    }
                    log(portName, tag + " CRC mismatch (rx=" + String.format("%02x", receivedCrc)
                            + " calc=" + String.format("%02x", calcCrc) + ") -> regex fallback");
                } catch (NumberFormatException ignored) {
                }
            }
        }

        // --- Fallback: regex scan for id/type in garbled data ---
        Matcher m = ID_TYPE_PATTERN.matcher(trimmed);
        while (m.find()) {
            put(deviceMap, m.group(1), m.group(2), Source.REGEX, portName, tag);
        }

        // --- Last resort: remember every id seen, verify it by unicast later ---
        collectCandidates(trimmed, candidates);
        return false;
    }

    private static void collectCandidates(String text, Set<String> candidates) {
        Matcher m = ID_PATTERN.matcher(text);
        while (m.find()) {
            String id = m.group(1);
            if (!"all".equalsIgnoreCase(id)) {
                candidates.add(id);
            }
        }
    }

    private static void extractIdType(String payload, Map<String, Entry> deviceMap,
                                      Source source, String portName, String tag) {
        String id = null;
        String type = null;
        for (String token : payload.split(",")) {
            if (token.startsWith("id:")) {
                id = token.substring(3).trim();
            } else if (token.startsWith("type:")) {
                type = token.substring(5).trim();
            }
        }
        if (id != null && !id.isEmpty() && !"all".equalsIgnoreCase(id)
                && type != null && !type.isEmpty()) {
            put(deviceMap, id, type, source, portName, tag);
        }
    }

    /** Adds/updates a device; a CRC-confirmed type always wins over a regex-recovered one. */
    private static void put(Map<String, Entry> deviceMap, String id, String type,
                            Source source, String portName, String tag) {
        Entry e = deviceMap.get(id);
        if (e == null) {
            e = new Entry();
            e.type = type;
            e.source = source;
            e.seen = 1;
            e.firstRound = tag;
            deviceMap.put(id, e);
            log(portName, tag + " + NEW id:" + id + " type:" + type + " (" + source + ")");
            return;
        }

        e.seen++;
        boolean better = (source != Source.REGEX && e.source == Source.REGEX)
                || (e.source == source && type.length() > e.type.length());
        if (better && !type.equals(e.type)) {
            log(portName, tag + " ~ id:" + id + " type " + e.type + " -> " + type
                    + " (" + e.source + " -> " + source + ")");
            e.type = type;
            e.source = source;
        } else if (source != Source.REGEX && e.source == Source.REGEX) {
            e.source = source;
        }
    }

    // --------------------------------------------------------------- config --

    /** Device ids configured for connections bound to this port (unicast seed). */
    private static List<String> configuredIdsForPort(String portName) {
        List<String> ids = new ArrayList<>();
        try {
            Set<String> connNames = new LinkedHashSet<>();
            for (ConfXmlObject.Connection c : Configuration.getXMLObj().getConnections()) {
                if (portName.equals(c.portName)) {
                    connNames.add(c.name);
                }
            }
            for (ConfXmlObject.Device d : Configuration.getXMLObj().getDevices()) {
                if (connNames.contains(d.connection) && d.name != null && !d.name.isEmpty()) {
                    ids.add(d.name);
                }
            }
        } catch (Exception e) {
            System.err.println("AutoDetect[" + portName + "] cannot read configuration: " + e);
        }
        return ids;
    }

    // -------------------------------------------------------------- helpers --

    private static void log(String portName, String msg) {
        System.out.println("AutoDetect[" + portName + "] " + msg);
    }

    /** Escapes control/non-printable bytes so garbled frames stay readable in the log. */
    private static String esc(String s) {
        StringBuilder sb = new StringBuilder(s.length() + 8);
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if (c == '\r') {
                sb.append("\\r");
            } else if (c == '\n') {
                sb.append("\\n");
            } else if (c < 0x20 || c > 0x7E) {
                sb.append(String.format("\\x%02X", (int) c & 0xFF));
            } else {
                sb.append(c);
            }
        }
        return sb.toString();
    }

    private static void sleep(long ms) {
        if (ms <= 0) {
            return;
        }
        try {
            Thread.sleep(ms);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
