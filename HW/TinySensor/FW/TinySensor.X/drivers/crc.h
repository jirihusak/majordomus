

#ifndef CRC_H
#define	CRC_H

#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>
#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif

uint8_t crc8(uint8_t seed, unsigned char const *data, size_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* CRC_H */

