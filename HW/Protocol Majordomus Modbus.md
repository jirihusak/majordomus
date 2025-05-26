# Protocol Majordomus Modbus
This document describes the communication protocol between Majordomus devices and Main computer. The communication is based on RS-485 physical layer. 
|Serial settings| value|
|--|--|
| Baud rate | 115 200 kbps|
| Data bits | 8 |
| Stop bits | 1 |
| Parity | None |

On the bus is one master computer and the master requests data from all devices. The device sends no data without previous request.
Every Device has Modbus Address (1 - 247). See more information about Modbus protocol: https://en.wikipedia.org/wiki/Modbus

## RoomSensor

| Logical address | Register | Action | Size B | Type | Note |
|--|--|--|--|--|--|
| 0x401001  | 0x1000 | Read | 2 | INT16 | Temperature from front cover [*10 °C] |
| 0x401002  | 0x1001 | Read | 2 | INT16 | Temperature from humidity sensor [*10 °C] |
| 0x401003  | 0x1002 | Read | 2 | UINT16 | Relative humidity [*10 %] |
| 0x401004  | 0x1003 | Read | 2 | INT16 | Temperature from 1-Wire sensor 0 [*10 °C] |
| 0x401005  | 0x1004 | Read | 2 | INT16 | Temperature from 1-Wire sensor 1 [*10 °C] |
| 0x401006  | 0x1005 | Read | 2 | UINT16 | Air Quality VOC index [0-500] |
| 0x401007  | 0x1006 | Read | 2 | UINT16 | CO2 [ppm] |
| 0x401008  | 0x1007 | Read | 4 | UINT32 | Light intensity [lux] |
| 0x401009  | 0x1008 | Read | 2 | UINT16 | Noise level [db] |
| 0x40100A  | 0x1009 | Read | 1 | UINT8 | Motion detector PIR [0/1] |
| 0x402001  | 0x2000 | Read | 1 | UINT8 | Digital inputs state bit array |
| 0x402002  | 0x2001 | Read | 1 | UINT8 | Button inputs bit array rising edge detection |
| 0x401003  | 0x1002 | Read | 2 | UINT16 | ADC0 Analog input voltage [mV] |
| 0x401004  | 0x1003 | Read | 2 | UINT16 | ADC1 Analog input voltage [mV] |
| 0x401005  | 0x1004 | Read | 2 | UINT16 | Power voltage voltage [mV] |
| 0x401006  | 0x1005 | Read | 2 | UINT16 | Power voltage for outputs voltage [mV] |
| 0x603001  | 0x3000 | Write | 1 | UINT8 | Digital outputs array |
| 0x603002  | 0x3001 | Write | 2 | UINT16 | DAC0 analog outputs [mV] |
| 0x603003  | 0x3002 | Write | 2 | UINT16 | DAC1 analog outputs [mV] |
| 0x603004  | 0x3003 | Write | 1 | UINT8 | Beep tone [0-4] |
| 0x603005  | 0x3004 | Write | 1 | UINT8 | LED light [0/1] |
| 0x603006  | 0x3005 | Write | 2 | INT16 | LCD Requested temperature [*10 °C] |
