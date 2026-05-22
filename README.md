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
 
### RoomSensor
<img width="500" alt="obrazek" src="https://github.com/user-attachments/assets/49e0f29c-1cdb-4bf7-9514-6cc1d3e47112" />


### BoxIO
<img width="500" alt="obrazek" src="https://github.com/user-attachments/assets/694fda46-37d7-421d-a3fd-76a13c89442a" />

### RoomIO

<img width="500" alt="obrazek" src="https://github.com/user-attachments/assets/8357177d-f048-438e-9133-dc0bc429cbbd" />

Schematics and BOMs live in [`hardware/`](hardware/).
 
## Software
 
### **Majordomus Control**
Bridge from HW modules to MQTT interface
<img width="500" alt="obrazek" src="https://github.com/user-attachments/assets/449e9a69-67f3-4fe1-a53f-36ccc8fc3cb1" />

### **Node-RED - Custom Node-RED nodes**
