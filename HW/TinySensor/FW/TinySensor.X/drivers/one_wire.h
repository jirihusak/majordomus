/* 
 * File:   one_wire.h
 * Author: husak
 *
 * Created on September 13, 2021, 10:25 PM
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>


#ifndef ONE_WIRE_H
#define	ONE_WIRE_H

typedef void (*owPortPtr)();
typedef uint8_t (*owPortPtrRet)();

#define ONE_WIRE_MAX_PORTS          2

typedef struct {
    owPortPtr owOut;
    owPortPtr owIn;
    owPortPtrRet owPin;
    int16_t temperature;
} t_one_wire_port;

extern t_one_wire_port oneWirePorts[];

char ow_reset(int port);
void ow_write_byte(int port, char byte);
char ow_read_byte(int port);

#endif	/* ONE_WIRE_H */

