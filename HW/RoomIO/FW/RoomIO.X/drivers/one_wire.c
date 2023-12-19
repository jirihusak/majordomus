
#include "one_wire.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../mcc_generated_files/device_config.h"

void ow_write_bit(int port, char b);
char ow_read_bit(int port);

//#define OWOUT   {UNI_0A_SetLow(); UNI_0A_SetDigitalOutput();}
//#define OWIN    UNI_0A_SetDigitalInput()
//#define OWPIN   UNI_0A_GetValue()


/**
 * Drive bus low, delay 480 ?s.
 * Release bus, delay 70 ?s.
 * Sample bus: 0 = device(s) present, 1 = no device present
 * Delay 410 ?s.
 */
char ow_reset(int port) {
    oneWirePorts[port].owOut();
    __delay_us(480); // wait for 480us+

    oneWirePorts[port].owIn();
    __delay_us(70);

    char response = (oneWirePorts[port].owPin() == 0);
    __delay_us(410);

    return response;
}

/*
 * Drive bus low.
 * Write 1: delay 6 ?s. Release bus, delay 64 ?s.
 * Write 0: delay 60 ?s. Release bus, delay 10 ?s.
 */
void ow_write_bit(int port, char b) {
    oneWirePorts[port].owOut();

    if (b) {
        __delay_us(6);
        oneWirePorts[port].owIn();
        __delay_us(64);
    } else {
        __delay_us(60);
        oneWirePorts[port].owIn();
        __delay_us(10);
    }
}

/*
 * Drive bus low, delay 6 ?s.
 * Release bus, delay 9 ?s.
 * Sample bus to read bit from slave.
 * Delay 55 ?s.
 */
char ow_read_bit(int port) {
    oneWirePorts[port].owOut();
    __delay_us(6);

    oneWirePorts[port].owIn();
    __delay_us(9);

    char response = oneWirePorts[port].owPin();

    __delay_us(55);

    return response;
}

char ow_read_byte(int port) {
    char c, r = 0;

    for (c = 0; c < 8; c++) {
        if (ow_read_bit(port)) {
            r |= 1 << c;
        }
    }

    return r;
}

void ow_write_byte(int port, char B) {
    char c;

    for (c = 0; c < 8; c++) {
        ow_write_bit(port, (B >> c) & 1);
    }
}
