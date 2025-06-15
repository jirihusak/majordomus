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
| 0x41001  | 0x1000 | Read | 2 | INT16 | Temperature from front cover [*10 °C] |
| 0x41002  | 0x1001 | Read | 2 | INT16 | Temperature from humidity sensor [*10 °C] |
| 0x41003  | 0x1002 | Read | 2 | UINT16 | Relative humidity [*10 %] |
| 0x41004  | 0x1003 | Read | 2 | INT16 | Temperature from 1-Wire sensor 0 [*10 °C] |
| 0x41005  | 0x1004 | Read | 2 | INT16 | Temperature from 1-Wire sensor 1 [*10 °C] |
| 0x41006  | 0x1005 | Read | 2 | UINT16 | Air Quality VOC index [0-500] |
| 0x41007  | 0x1006 | Read | 2 | UINT16 | CO2 [ppm] |
| 0x41008  | 0x1007 | Read | 4 | UINT32 | Light intensity [lux] |
| 0x4100A  | 0x1009 | Read | 2 | UINT16 | Noise level [db] |
| 0x4100B  | 0x100A | Read | 2 | UINT16 | Motion detector PIR [0/1] |
| 0x4100C  | 0x100B | Read | 2 | UINT16 | Digital inputs state bit array |
| 0x4100D  | 0x100C | Read | 2 | UINT16 | Button inputs bit array rising edge detection |
| 0x4100E  | 0x100D | Read | 2 | UINT16 | ADC0 Analog input voltage [mV] |
| 0x4100F  | 0x100E | Read | 2 | UINT16 | ADC1 Analog input voltage [mV] |
| 0x41010  | 0x100F | Read | 2 | UINT16 | Power voltage voltage [mV] |
| 0x41011  | 0x1010 | Read | 2 | UINT16 | Power voltage for outputs voltage [mV] |
| 0x62001  | 0x2000 | Write | 2 | UINT16 | Digital outputs array |
| 0x62002  | 0x2001 | Write | 2 | UINT16 | DAC0 analog outputs [mV] |
| 0x62003  | 0x2002 | Write | 2 | UINT16 | DAC1 analog outputs [mV] |
| 0x62004  | 0x2003 | Write | 2 | UINT16 | Beep tone [0-4] |
| 0x62005  | 0x2004 | Write | 2 | UINT16 | LED light [0/1] |
| 0x62006  | 0x2005 | Write | 2 | INT16 | LCD Requested temperature [*10 °C] |
