# MajordomusControl — MQTT Protocol Reference

**Version:** 1.1
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
7. [Button Events](#7-button-events)
8. [Device Types and Topic Maps](#8-device-types-and-topic-maps)
   - [RoomIO](#81-roomio)
   - [BoxIO](#82-boxio)
   - [RoomSensor](#83-roomsensor)
   - [TempOutBoard](#84-tempoutboard)
   - [RoomIR](#85-roomir)
9. [Home Assistant Discovery](#9-home-assistant-discovery)
10. [Connection and Configuration](#10-connection-and-configuration)
11. [Quick Reference Cheatsheet](#11-quick-reference-cheatsheet)

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
| `<category>` | One of: `tele`, `evt`, `state`, `cmd` | `tele` |
| `<key>` | Specific property name | `temperature0` |

**Full example:**
```
majordomus/living_room/tele/temperature0
```

> **Note:** command routing is positional — `cmd` must be the **third** topic level.
> Configure the prefix as a single segment (`majordomus/`), not a multi-level path.

---

## 2. Topic Categories

| Category | Trigger | Retained | Purpose |
|---|---|---|---|
| `tele` | On measured value **change** | **Yes** | Sensor measurements |
| `evt` | On every event (even repeated) | **No** | Events: button gestures, motion |
| `state` | On change | **Yes** | Device metadata and output states |
| `cmd` | Published by the client | — | Commands to devices |

Retained topics (`tele`, `state`) are **not** re-published on every poll cycle — only when the
value differs from the last one sent. Consumers always get the current value from the retained
message right after subscribing. `evt` topics are published on every event, even if the payload
value is unchanged.

Device metadata (`version`, `power`, `powerOut`) comes from a status request the gateway sends
to each device once every **30 seconds**; all other data is read on every poll cycle.

---

## 3. QoS and Retain Policy

- **QoS:** `0` (at most once) for all messages
- **Retain:**
  - `tele/` → `true`
  - `state/` → `true`
  - `evt/` → `false` (exception: `evt/lastMotion` is retained)
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
- Level `[1]` is the device name (must be a configured device)
- Level `[3]` is the command key
- Payload is the command value

**Example — turn on digital output 0 on device `living_room`:**
```
Topic:   majordomus/living_room/cmd/do0
Payload: 1
```

### Command echo to `state/`

Every accepted command is immediately re-published to a retained topic in the `state/`
category. These are the topics Home Assistant uses as `state_topic` for switches and numbers.

| Command | Echo topic |
|---|---|
| `cmd/do0` … `cmd/do9` | `state/output0` … `state/output9` |
| `cmd/dac0`, `cmd/dac1` | `state/dac0`, `state/dac1` |
| `cmd/setCnt0` … `cmd/setCnt7` | `state/setCounter0` … `state/setCounter7` |
| `cmd/reqT` | `state/requestedTemperature` |
| `cmd/reqL` | `state/requestedLight` |
| `cmd/light` | `state/light` |
| `cmd/beep` | `state/beep` (not retained) |
| `cmd/ro` | `state/ro` |

The echo is published when the gateway **receives** the command; the value reaches the device
on the next RS-485 cycle. For `requestedTemperature` and `requestedLight`, the gateway also
publishes to the same topic when the user changes the setpoint locally on the device.

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

- Retained: **Yes**, published on state transitions only
- This topic is used as `availability_topic` in Home Assistant discovery configs.

---

## 6. Sensor Value Conventions

### Payload format

All payloads are UTF-8 encoded strings, with one exception: `tele/irImagePng` carries raw
binary PNG data.

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

## 7. Button Events

Buttons and capacitive pads do not report a pressed/released state — they report a
**recognised gesture**. The device firmware distinguishes short press, double click,
triple click and long hold; the gateway publishes each gesture as a one-shot pulse
on a dedicated topic.

| Topic (`evt/`) | Gesture |
|---|---|
| `button<i>` | Short press |
| `button<i>Double` | Double click |
| `button<i>Triple` | Triple click |
| `button<i>Long` | Long hold |

- Payload is always `1`, retain **false**. No `0` message is ever sent.
- Index range: RoomIO and BoxIO `0`–`7`, RoomSensor `0`–`3`.
- For a level state of a digital input (rather than a gesture), use `tele/input<i>`.

---

## 8. Device Types and Topic Maps

### 8.1 RoomIO

General-purpose I/O module with temperature sensors, digital inputs/outputs, analog inputs, and pulse counters.

#### Published — `tele/` (retained)

| Topic key | Unit | Type | Notes |
|---|---|---|---|
| `temperature0`, `temperature1` | °C | float | `NaN` if unavailable |
| `temperature2` | °C | float | Optional, only if the device reports it |
| `humidity` | % | float | Optional relative humidity |
| `voc` | index | float | Optional VOC air quality index |
| `co2` | ppm | float | Optional CO₂ concentration |
| `illuminance` | lux | float | Optional ambient light |
| `distance` | — | float | Optional distance sensor |
| `analog0`, `analog1` | V | float | ADC channels |
| `counter0` – `counter7` | pulses | integer | Pulse counters 0–7 |
| `input0` – `input7` | — | `0`/`1` | Digital input states (decoded from bitfield) |
| `apparentTemperature` | °C | float | Calculated; published when `temperature0` and `humidity` are valid |

#### Published — `evt/` (not retained)

`button0` – `button7` plus the `…Double`, `…Triple`, `…Long` variants — see [Button Events](#7-button-events).

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version string |
| `power` | V | Supply voltage |
| `powerOut` | V | Output supply voltage |
| `online` | `online`/`offline` | Device availability |
| `output0` – `output7` | `0`/`1` | Digital output states (command echo) |
| `dac0`, `dac1` | V | DAC output level (command echo) |
| `setCounter0` – `setCounter7` | integer | Last counter preset (command echo) |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do7` | `0` or `1` | Set digital output state |
| `dac0`, `dac1` | `0`–`10` (float) | Set analog output voltage |
| `setCnt0` – `setCnt7` | integer string | Preset pulse counter value |
| `reboot` | any | Reboot the device |

---

### 8.2 BoxIO

DIN-rail module with 8 digital inputs, 6 power outputs, 2 analog inputs and 2 analog outputs.
Topic set is identical to RoomIO except for the output count.

#### Published — `tele/` (retained)

`temperature0`, `temperature1`, optionally `temperature2`, `humidity`, `voc`, `co2`,
`illuminance`, `distance`; plus `analog0`, `analog1`, `counter0`–`counter7`,
`input0`–`input7`, `apparentTemperature`.

#### Published — `evt/` (not retained)

`button0` – `button7` plus the `…Double`, `…Triple`, `…Long` variants.

#### Published — `state/` (retained)

`version`, `power`, `powerOut`, `online`, `output0`–`output5`, `dac0`, `dac1`,
`setCounter0`–`setCounter7`.

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do5` | `0` or `1` | Set digital output state |
| `dac0`, `dac1` | `0`–`10` (float) | Set analog output voltage |
| `setCnt0` – `setCnt7` | integer string | Preset pulse counter value |
| `reboot` | any | Reboot the device |

---

### 8.3 RoomSensor

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

#### Published — `evt/`

| Topic key | Type | Retained | Trigger |
|---|---|---|---|
| `motion` | `0`/`1` | No | Motion detector (PIR) state change |
| `lastMotion` | `yyyy-MM-dd HH:mm:ss` | **Yes** | Timestamp of last motion event |
| `button0` – `button3` | `1` | No | Short press of a capacitive button |
| `button0Double` – `button3Long` | `1` | No | Double / triple click, long hold |

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version |
| `power` | V | Supply voltage |
| `powerOut` | V | Output supply voltage |
| `requestedTemperature` | °C | Current setpoint (floor heating / HVAC) |
| `requestedLight` | % | Requested backlight level |
| `light` | `0`/`1` | Backlight on/off |
| `output0` – `output3` | `0`/`1` | Digital output states (command echo) |
| `dac0`, `dac1` | V | DAC output level (command echo) |
| `online` | `online`/`offline` | Device availability |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do3` | `0` or `1` | Set digital output |
| `dac0`, `dac1` | `0`–`10` (float) | Set analog output voltage |
| `beep` | integer | Trigger piezo beep (duration or pattern code) |
| `reqT` | float `10`–`35` | Set requested temperature setpoint (step 0.5 °C) |
| `reqL` | `0`–`100` | Set requested backlight level (%) |
| `light` | `0` or `1` | Switch the backlight on/off |
| `reboot` | any | Reboot the device |

---

### 8.4 TempOutBoard

Outdoor temperature board with up to 8 temperature sensors (DS18B20) and digital outputs.

#### Published — `tele/` (retained)

| Topic key | Unit | Type | Notes |
|---|---|---|---|
| `temperature0` – `temperature7` | °C | float | DS18B20 sensors, raw value / 10; `NaN` if unavailable |

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version |
| `power` | V | Supply voltage |
| `powerOut` | V | Output supply voltage |
| `output0` – `output9` | `0`/`1` | Digital output states (command echo) |
| `ro` | `0`–`255` | Resistor/PWM output level (command echo) |
| `online` | `online`/`offline` | Device availability |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `do0` – `do9` | `0` or `1` | Set digital output |
| `ro` | `0`–`255` | Set resistor/PWM output level |

---

### 8.5 RoomIR

Thermal infrared camera module (MLX90642, 32×24 = 768 pixels). Besides the raw frame it
publishes the results of image processing performed by the gateway.

#### Published — `tele/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `irImage` | JSON array string | 768 float values (32×24 pixels), temperature in °C |
| `irImagePng` | binary PNG | 320×240 heatmap (iron-bow palette) for the HA MQTT camera |
| `maxTemperature` | float | Highest temperature in the frame (°C, 1 decimal) |
| `minTemperature` | float | Lowest temperature in the frame (°C, 1 decimal) |
| `fireAlarm` | `0`/`1` | `1` when `maxTemperature` exceeds the configured fire threshold |
| `personCount` | integer | Number of detected persons |
| `zone0Presence` – `zone3Presence` | `0`/`1` | Presence inside a configured zone (enabled zones only) |

**IR image format:**
```json
[25.1, 25.3, 24.8, 26.0, ...]
```
- Array length: **768 elements** (32 columns × 24 rows)
- Row-major order, top-left first
- Values in °C (float)

**Frame transport:** one frame arrives as 6 base64-encoded blocks of 128 pixels together with
`tmin`/`tmax` (in 0.01 °C units) and a sequence number `seq`. Pixels are normalized to 0..255,
the gateway converts them back with `T = tmin + px × (tmax − tmin) / 255`. The frame is published
once all 6 blocks of the same `seq` have been received. The PNG is published at most **once every
2 seconds** to avoid flooding the broker.

#### Published — `state/` (retained)

| Topic key | Type | Notes |
|---|---|---|
| `version` | string | Firmware version |
| `online` | `online`/`offline` | Device availability |

#### Commands — `cmd/` (subscribe)

| Command key | Payload | Effect |
|---|---|---|
| `reboot` | any | Reboot the device |

#### Image processing configuration

Per-device, in the `<irConfig>` section of `config.xml` (or via the web UI):

| Parameter | Default | Meaning |
|---|---|---|
| `fireThreshold` | 100 °C | Threshold for `fireAlarm` |
| `personMinTemp` | 28 °C | Lower safety bound of the human temperature band |
| `personMaxTemp` | 38 °C | Upper bound of the human temperature band |
| `minClusterSize` | 8 px | Smaller clusters are treated as noise |
| `zones` | — | Up to 4 rectangular zones (`x`, `y`, `w`, `h`, `name`, `enabled`) |

The person-detection threshold is derived dynamically from the scene background
(frame median + 2 °C); `personMinTemp` acts only as a lower safety bound, since a clothed
person may be just 2–4 °C above background. Persons merged into one cluster are separated
by counting temperature peaks with sufficient prominence.

---

## 9. Home Assistant Discovery

When the gateway connects to the broker, it automatically publishes
MQTT discovery messages for all configured devices.
This allows Home Assistant to auto-configure entities without manual YAML.

### Discovery topic format

```
<ha_prefix>/<entity_type>/<unique_id>/config
```

| Segment | Example |
|---|---|
| `<ha_prefix>` | `homeassistant/` (configured separately from MQTT prefix) |
| `<entity_type>` | `sensor`, `binary_sensor`, `switch`, `number`, `camera`, `device_automation` |
| `<unique_id>` | `living_room_t0` |

### Entity types created

| Entity type | Used for |
|---|---|
| `sensor` | Temperatures, humidity, VOC/NOx/CO₂, illuminance, noise, analog inputs, counters, supply voltage (diagnostic), `personCount`, `min`/`maxTemperature` |
| `binary_sensor` | Digital inputs, motion, `fireAlarm`, RoomIR zone presence |
| `switch` | Digital outputs (`output<i>`), RoomSensor backlight |
| `number` | DAC outputs (0–10 V, step 0.1), `requestedTemperature` (10–35 °C, step 0.5), `requestedLight` (0–100 %) |
| `camera` | RoomIR heatmap (`tele/irImagePng`) |
| `device_automation` | Button gestures as device triggers |

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

### Button device triggers

Button gestures are exposed as stateless device triggers (they appear in HA automations,
not as entities):

```json
{
  "automation_type": "trigger",
  "type": "button_double_press",
  "subtype": "button_3",
  "topic": "majordomus/living_room/evt/button2Double",
  "payload": "1",
  "device": { "identifiers": ["living_room"], "...": "..." }
}
```

Types: `button_short_press`, `button_double_press`, `button_triple_press`, `button_long_press`.
`subtype` is 1-based (`button_1` corresponds to topic `button0`).

### Re-publish and cleanup

- Discovery configs are published on every MQTT connect and reconnect
  (`connectComplete` callback) to handle broker restarts.
- RoomIR zones that are no longer enabled are removed by publishing an **empty retained
  payload** to their discovery topic, so the entity disappears from Home Assistant.

---

## 10. Connection and Configuration

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
| Initial connect retry | Every 30 s until successful |
| Gateway subscription | `<prefix>/#`, re-subscribed after every reconnect |

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

## 11. Quick Reference Cheatsheet

```
READ sensor value:
  Subscribe: <prefix>/<device>/tele/<key>
  Subscribe: <prefix>/<device>/state/<key>

LISTEN for events:
  Subscribe: <prefix>/<device>/evt/<key>

SEND command:
  Publish:   <prefix>/<device>/cmd/<key>   payload: <value>

OUTPUT STATE (command echo):
  Subscribe: <prefix>/<device>/state/output<i>

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
| `tele/input0` | Digital input 0 state | `0`/`1` |
| `evt/motion` | Motion detection | `0`/`1` |
| `evt/button0` | Button 0 short press | `1` |
| `evt/button0Long` | Button 0 long hold | `1` |
| `state/online` | Availability | `online`/`offline` |
| `state/version` | Firmware version | string |
| `state/output0` | Digital output 0 state | `0`/`1` |
| `cmd/do0` | Set digital output 0 | `0`/`1` |
| `cmd/reboot` | Reboot device | any |

---

*Generated from MajordomusControl source code analysis.*
