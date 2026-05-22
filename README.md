<img width="212" height="67" alt="logo" src="https://github.com/user-attachments/assets/f73272f2-1461-44af-90d7-b82d8c60e8bf" />

# Majordmus - Open source system for Smart Home
 
## About
 
**Majordomus** is a modular automation system built on open standards. It fills the gap between closed, expensive commercial solutions and unreliable consumer-grade components — designed to be repairable, expandable, and to last for decades.
  
## Features
 
- 🧩 **Modular hardware** — base unit + interchangeable modules ( I/O, sensors)
- 🔌 **Standardized interface** — RS-485 / ModBus HW interface, MQTT SW interface
- 🔓 **Vendor lock-in free** — open hardware, open software, your choice
- 🧠 **Node-RED inside** — visual programming for automation flows
- 🧠 **Home Assistant interface** — MQTT integration for popular platform
- 📡 **Offline-first** — works without internet, cloud is optional
## Use Cases
 
| Domain | Examples |
|---|---|
| 🏠 Smart home | lighting, heating, blinds, security, energy management |
| 🏭 Industry | machine control, data acquisition, monitoring, alarms |
| 📊 Monitoring | weather, air quality, energy meters, IoT sensors |
 
## Architecture
 
```
   ┌─────────────────────┐
   │      node-RED       │
   │   Home Assistant    │
   └──────────┬──────────┘
              │
             MQTT
              │
   ┌──────────┴──────────┐
   │ Majordomus Control  │
   │    MQTT bridge      │
   └──────────┬──────────┘
              │
       Modbus / RS-485
              │
    ┌─────────┼─────────┐
    │         │         │
   Unit A   Unit B   Unit C
  (+ modules) (+ modules) (+ modules)
```
 
## Hardware
 
### 🌡️ RoomSensor
 
All-in-one ambient monitoring unit for indoor environments.
 
- 🌡️ Temperature & humidity
- 💨 CO₂, VOC & NOx measurement
- 🚶 PIR motion detection
- ☀️ Ambient light (lux)
- 🔊 Ambient noise measurement
- 🖥️ Display with 3 buttons
- 🔔 Piezo buzzer for acoustic signaling
- 🔌 Digital I/O for door/window contacts or local buttons
- 🔗 I²C port for external sensors
- 🏠 Wall-mount
<img width="500" alt="RoomSensor" src="https://github.com/user-attachments/assets/49e0f29c-1cdb-4bf7-9514-6cc1d3e47112" />

### 🔲 BoxIO
 
Compact relay & input unit for in-wall or junction-box installation.
 
- ⚡ Replaceable relay outputs (electromagnetic or SSR)
- 🔘 Digital inputs for buttons
- 📊 Hardware input counters for energy monitoring
- 🎛️ Manual control of all outputs (AUTO / MAN ON / MAN OFF)
- 🔗 I²C port for external sensors
<img width="500" alt="BoxIO" src="https://github.com/user-attachments/assets/694fda46-37d7-421d-a3fd-76a13c89442a" />

### 🎛️ RoomIO
 
Multi-purpose I/O unit for room-level automation and control.
 
- 🔘 Digital inputs & outputs
- 📥 Analog inputs (ADC) — 0–10 V
- 📤 Analog outputs (DAC) — 0–10 V for dimming, fan speed, valves
- 📊 Hardware input counters for energy monitoring
- 🔗 I²C port for external sensors
<img width="500" alt="RoomIO" src="https://github.com/user-attachments/assets/8357177d-f048-438e-9133-dc0bc429cbbd" />


Schematics and BOMs live in [`hardware/`](hardware/).
 
## Software
 
### **Majordomus Control**
Bridge from HW modules to MQTT interface

<img width="800" alt="obrazek" src="https://github.com/user-attachments/assets/449e9a69-67f3-4fe1-a53f-36ccc8fc3cb1" />

### **Node-RED - Custom Node-RED nodes**

## Documentation
 
- 

## Contributing
 
Contributions welcome — hardware, firmware, Node-RED flows, or docs.

## Contact
 
- **Web:** [majordomus.tech](https://majordomus.tech)
