# MajordomusControl — MQTT Protocol Reference

**Version:** 1.0
**System:** MajordomusControl — RS-485 to MQTT Gateway
**Protocol:** MQTT v3.1.1

---

## Overview

MajordomusControl is a gateway that bridges RS-485 sensor devices to an MQTT broker.
It reads sensor data from physical devices over serial RS-485 and publishes it to MQTT topics.
It also subscribes to command topics and forwards received commands back to devices via RS-485.

```
[RS-485 Devices] <--serial--> [MajordomusControl] <--MQTT--> [Broker] <--> [HA / AI / Client]
```

---

## Table of Contents

1. [Topic Structure](#1-topic-structure)
2. [Topic Categories](#2-topic-categories)
3. [QoS and Retain Policy](#3-qos-and-retain-policy)
4. [Sending Commands](#4-sending-commands)
5. [Availability](#5-availability)
6. [Sensor Value Conventions](#6-sensor-value-conventions)
7. [Device Types and Topic Maps](#7-device-types-and-topic-maps)
   - [RoomIO](#71-roomio)
   - [RoomSensor](#72-roomsensor)
   - [TempOutBoard](#73-tempoutboard)
   - [RoomIR](#74-roomir)
8. [Home Assistant Discovery](#8-home-assistant-discovery)
9. [Connection and Configuration](#9-connection-and-configuration)
10. [Quick Reference Cheatsheet](#10-quick-reference-cheatsheet)

---

## 1. Topic Structure

Every topic follows this pattern:

```
<prefix>/<device_name>/<category>/<key>
```

| Segment | Description | Example |
|---|---|---|
| `<prefix>` | Configured MQTT topic prefix, including trailing `/` | `majordomus/` |
| `<device_name>` | Device name as defined in `config.xml` | `living_room` |
| `<category>` | One of: `tele`, `evt`, `state` | `tele` |
| `<key>` | Specific property name | `temperature0` |

**Full example:**
```
majordomus/living_room/tele/temperature0
```

---

## 2. Topic Categories

| Category | Trigger | Retained | Purpose |
|---|---|---|---|
| `tele` | Periodic polling (every RS-485 poll cycle) | **Yes** | Sensor measurements |
| `evt` | On-change only | **No** | Events: button presses, motion |
| `state` | On change | **Yes** | Device metadata and output states |

---

## 3. QoS and Retain Policy

- **QoS:** `0` (at most once) for all messages
- **Retain:**
  - `tele/` → `true`
  - `state/` → `true`
  - `evt/` → `false`
  - Home Assistant discovery → `true`

---

## 4. Sending Commands

To control a device, publish to:

```
<prefix>/<device_name>/cmd/<command_key>
```

**Payload:** plain UTF-8 string (numeric value or string constant)

The gateway subscribes to `<prefix>/#` to receive all commands under the prefix.

**Command routing:**
- Topic must have at least 4 levels
- Level `[2]` must be exactly `cmd`
- Level `[1]` is the device name
- Level `[3]` is the command key
- Payload is the command value

**Example — turn on digital output 0 on device `living_room`:**
```
Topic:   majordomus/living_room/cmd/do0
Payload: 1
```

---

## 5. Availability

Every device publishes its online status to:

```
<prefix>/<device_name>/state/online
```

| Payload | Meaning |
|---|---|
| `online` | Device is responding to RS-485 polls |
| `offline` | No response for more than 3 seconds |

- Retained: **Yes**
- This topic is used as `availability_topic` in Home Assistant discovery configs.

---

## 6. Sensor Value Conventions

### Payload format

All payloads are UTF-8 encoded strings.

| Data type | Example payload |
|---|---|
| Float | `22.5` |
| Integer | `420` |
| Unavailable sensor | `NaN` |
| JSON array | `[25.1, 25.3, ...]` |

### NaN sentinel

The firmware uses `INT16_MIN` (`-32768`) to indicate that a sensor is not available
(hardware not connected, read error, etc.).
The gateway converts this value to the string `"NaN"` before publishing.

**Rule for consumers:** If the payload is `"NaN"`, the reading should be treated as unavailable/unknown.

### Scaled values

Some devices (e.g., TempOutBoard) transmit raw integer values that must be divided by a scaling factor.
This conversion is done by the gateway before publishing — consumers always receive the final scaled float.

### Apparent temperature

When both `temperature0` (key `t0`) and `humidity` (key `rh`) are valid,
the gateway automatically calculates and publishes an apparent ("feels like") temperature using the Steadman formula:

```
e  = (rh / 100) × 6.105 × exp(17.27 × T / (237.7 + T))
AT = T + 0.33 × e − 4.00
```

Published to: `<prefix>/<device_name>/tele/apparentTemperature`

---

## 7. Device Types and Topic Maps

### 7.1 RoomIO

General-purpose I/O module with temperature sensors, digital inputs/outputs, analog inputs, and pulse counters.

#### Published — `tele/` (retained)

| Topic key | Unit | Type | Notes |
|---|---|---|---|
| `temperature0` | °C | float | Sensor 0; `NaN` if unavailable |
| `temperature1` | °C | float | Sensor 1; `NaN` if unavailable |
| `temperature2` | °C | float | Sensor 2 (optional); `NaN` if unavailable |
| `humidity` | % | float | Relative humidity; `NaN` if unavailable |
| `voc` | index | integer | VOC air quality index; `NaN` if unavailable |
| `co2` | ppm | integer | CO₂ concentration; `NaN` if unavailable |
| `illuminance` | lux | integer | Ambient light; `NaN` if unavailable |
| `distance` | unit | float | Distance sensor; `NaN` if unavailable |
| `analog0` | V | float | ADC channel 0 |
| `analog1` | V | float | ADC channel 1 |
| `counter0` – `counter7` | — | integer | Pulse counters 0–7 |
| `input0` – `input7` | — | `0`/`1` | Digital input states (decoded from bitfield) |
| `apparentTemperature` | °C | float | Calculated; published when `temperature0` and `humidity` are valid |

#### Published — `evt/` (not retained)

| Topic key | Type | Trigger |
|---|---|---|
| `button0` – `button7` | `0`/`1` | Published on button state change (decoded from bitfield) |

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version string |
| `power` | V | Supply voltage |
| `powerOut` | V | Output supply voltage |
| `online` | `online`/`offline` | Device availability |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do7` | `0` or `1` | Set digital output state |
| `dac0`, `dac1` | `0`–`255` | Set DAC/PWM analog output level |
| `setCnt0` – `setCnt7` | integer string | Preset pulse counter value |
| `reboot` | any | Reboot the device |

---

### 7.2 RoomSensor

Room sensor unit with temperature, humidity, air quality, motion, light, and noise sensors,
plus capacitive touch buttons and optional heating/lighting outputs.

#### Published — `tele/` (retained)

| Topic key | Unit | Type | Notes |
|---|---|---|---|
| `temperature0` – `temperature3` | °C | float | Up to 4 temperature sensors; `NaN` if unavailable |
| `humidity` | % | float | Relative humidity; `NaN` if unavailable |
| `voc` | index | integer | VOC index (Sensirion SGP40/41); `NaN` if unavailable |
| `nox` | index | integer | NOx index (Sensirion SGP41); `NaN` if unavailable |
| `co2` | ppm | integer | CO₂ concentration; `NaN` if unavailable |
| `illuminance` | lux | integer | Ambient light (VEML7700); `NaN` if unavailable |
| `noise` | dB | integer | Ambient noise level; `NaN` if unavailable |
| `analog0`, `analog1` | V | float | ADC channels |
| `input0` – `input3` | — | `0`/`1` | Digital input states |
| `apparentTemperature` | °C | float | Calculated; published when `temperature0` and `humidity` are valid |

#### Published — `evt/` (not retained)

| Topic key | Type | Trigger |
|---|---|---|
| `motion` | `0`/`1` | Motion detector (PIR) state change |
| `lastMotion` | ISO datetime string | Timestamp of last motion event |
| `button0` – `button3` | `0`/`1` | Capacitive touch button state change |

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version |
| `power` | V | Supply voltage |
| `powerOut` | V | Output supply voltage |
| `requestedTemperature` | °C | Current setpoint (floor heating / HVAC) |
| `online` | `online`/`offline` | Device availability |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do3` | `0` or `1` | Set digital output |
| `dac0`, `dac1` | `0`–`255` | Set DAC output level |
| `beep` | integer | Trigger piezo beep (duration or pattern code) |
| `reqT` | float `10`–`35` | Set requested temperature setpoint |
| `light` | `0`–`100` | Set light level (%) |
| `reboot` | any | Reboot the device |

---

### 7.3 TempOutBoard

Outdoor temperature board with up to 8 temperature sensors (DS18B20) and digital outputs.

#### Published — `tele/` (retained)

| Topic key | Unit | Type | Notes |
|---|---|---|---|
| `temperature0` – `temperature7` | °C | float | DS18B20 sensors; `NaN` if unavailable |

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version |
| `power` | V | Supply voltage |
| `powerOut` | V | Output supply voltage |
| `online` | `online`/`offline` | Device availability |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do9` | `0` or `1` | Set digital output |
| `ro` | `0`–`255` | Set resistor/PWM output level |

---

### 7.4 RoomIR

Thermal infrared camera module (MLX90640, 32×24 pixels).

#### Published — `tele/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `irImage` | JSON array string | 768 float values (32×24 pixels) representing temperature in °C |

**IR image format:**
```json
[25.1, 25.3, 24.8, 26.0, ...]
```
- Array length: **768 elements** (32 columns × 24 rows)
- Row-major order, top-left first
- Values in °C (float)
- Raw bytes from device are received as base64-encoded RS-485 chunks,
  assembled in memory, scaled by the device's resolution factor,
  then published as a single JSON array string.

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version |
| `resolution` | float | Scaling factor used for raw→°C conversion |
| `offset` | integer | Byte offset (internal assembly parameter) |
| `online` | `online`/`offline` | Device availability |

---

## 8. Home Assistant Discovery

When the gateway connects to the broker, it automatically publishes
MQTT discovery messages for all configured devices.
This allows Home Assistant to auto-configure entities without manual YAML.

### Discovery topic format

```
<ha_prefix>/<entity_type>/<unique_id>/config
```

| Segment | Example |
|---|---|
| `<ha_prefix>` | `homeassistant` (configured separately from MQTT prefix) |
| `<entity_type>` | `sensor`, `binary_sensor`, `switch`, `number` |
| `<unique_id>` | `living_room_t0` |

### Discovery payload (JSON, retained)

```json
{
  "name": "temperature0",
  "unique_id": "living_room_t0",
  "state_topic": "majordomus/living_room/tele/temperature0",
  "unit_of_measurement": "°C",
  "device_class": "temperature",
  "state_class": "measurement",
  "availability_topic": "majordomus/living_room/state/online",
  "payload_available": "online",
  "payload_not_available": "offline",
  "device": {
    "identifiers": ["living_room"],
    "manufacturer": "Majordomus",
    "model": "RoomSensor",
    "name": "living_room"
  }
}
```

### Re-publish trigger

Discovery configs are re-published on every MQTT reconnect
(`connectComplete` callback) to handle broker restarts.

---

## 9. Connection and Configuration

### Broker URL formats

| Scheme | Description |
|---|---|
| `tcp://host:port` | Plain TCP (default port: 1883) |
| `ssl://host:port` | TLS/SSL (default port: 8883) |

### Connection parameters

| Parameter | Value |
|---|---|
| MQTT version | 3.1.1 |
| QoS (all messages) | 0 |
| Connection timeout | 30 s |
| Keep-alive interval | 5 s |
| Automatic reconnect | Yes |

### SSL/TLS options

| Setting | Description |
|---|---|
| Default CA | Uses system Java TrustStore |
| Self-signed CA | Path to a PEM-encoded CA certificate file |

### config.xml structure

```xml
<MQTT>
  <broker
    address="tcp://broker.example.com:1883"
    username="user"
    passwd="password"
    topic="majordomus/"
    selfsigned="false"
    cert="/path/to/ca.crt"
  />
</MQTT>

<HomeAssistant>
  <settings
    enable="true"
    topic="homeassistant/"
  />
</HomeAssistant>
```

---

## 10. Quick Reference Cheatsheet

```
READ sensor value:
  Subscribe: <prefix>/<device>/tele/<key>
  Subscribe: <prefix>/<device>/state/<key>

LISTEN for events:
  Subscribe: <prefix>/<device>/evt/<key>

SEND command:
  Publish:   <prefix>/<device>/cmd/<key>   payload: <value>

CHECK availability:
  Subscribe: <prefix>/<device>/state/online   → "online" | "offline"

WILDCARD (all devices, all data):
  Subscribe: <prefix>/#
```

### Common sensor keys across device types

| MQTT key | Description | Unit |
|---|---|---|
| `tele/temperature0` | Primary temperature | °C |
| `tele/humidity` | Relative humidity | % |
| `tele/voc` | VOC air quality index | — |
| `tele/co2` | CO₂ concentration | ppm |
| `tele/illuminance` | Ambient light | lux |
| `tele/noise` | Ambient noise | dB |
| `tele/apparentTemperature` | Feels-like temperature | °C |
| `evt/motion` | Motion detection | `0`/`1` |
| `evt/button0` | Button 0 press | `0`/`1` |
| `state/online` | Availability | `online`/`offline` |
| `state/version` | Firmware version | string |
| `cmd/do0` | Digital output 0 | `0`/`1` |
| `cmd/reboot` | Reboot device | any |

---

*Generated from MajordomusControl source code analysis.*
