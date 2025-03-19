/* 
 * File:   tempHum_SHT41.h
 * Author: jiri
 *
 */

#include <stdint.h>

#ifndef TEMPHUM_SHT41_H
#define	TEMPHUM_SHT41_H

int tempHum_SHT41_meas(int32_t *temperature, uint32_t *humidity);

void tempHum_SHT41_Task();

#endif	/* TEMPHUM_SHT41_H */

