# Majordomus - Open source system for Smart Home
Majordomus is open hardware and open source system not only for smart home. Majordomus HW devices communicating via RS-485 interface with a gateway (for example RPi). This gateway makes an MQTT interface for commonly used SW applications for Smart Home (for example node-red).
See diagram below. Majordomus can do the blue parts from the diagram. Enjoy Majordomus in your project!

![Nepojmenované](https://github.com/jirihusak/majordomus/assets/33901383/3a26aa7d-3c42-4a49-821c-4de11687ebca)

## List of Majordomus HW devices
- Majordomus RS4852USB - USB to 4 channel isolated RS-485 interface.

![obrazek](https://github.com/user-attachments/assets/1d46ba04-b7e9-4ab4-813b-fe0bf64ae502)

- Majordomus RoomIO - Compact device with MCU and RS-485 interface intended for junction box. Device has 8x 12V isolated inputs, 8x 12V digital outputs, 2x ADC 0 - 30V, 2x DAC 0-10V, 2x One-Wire interface for DS18B20 temperature sensor, 1x I2C.

![obrazek](https://github.com/user-attachments/assets/5a028f5a-e13e-42af-8ec7-fc6ec4342270)

- Majordomus RoomSensor - Compact device with MCU and RS-485 interface intended for junction box. Device has 4x 12V isolated inputs, 4x 12V digital outputs, 2x ADC 0 - 30V, 2x DAC 0-10V, 2x One-Wire interface for DS18B20 temperature sensor, 1x I2C, Temperature Sensor, Humidity Sensor, PIR motion detector, VOC sensor, Lux meter, LED light, 128x32 Display, 3X Capative touch button, MEMS Microphone noise level meter, Buzzer,

![obrazek](https://github.com/user-attachments/assets/d86854e0-54ac-4b06-8060-f4d75f83a3b8)

## Majordomus SW

### Majordomus Control
This application makes the bridge between RS-485 bus and MQTT. The application has also GUI for real time controling the state of all devices. You can update devices to newer firmware.

![obrazek](https://github.com/user-attachments/assets/ec6d1a99-08e1-468d-918d-efa711c0d951)


### Node-Red nodes

- Scheduler
  Node for week based scheduling. You can set the rules with priorities and the node will send value from current rule.
![obrazek](https://github.com/user-attachments/assets/eec50598-5b8e-41da-854a-8ed3e9546968)



## Website
www.majordomus.tech
