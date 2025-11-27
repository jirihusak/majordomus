/* 
 * File:   temp_ds18B20.h
 * Author: husak
 *
 * Created on September 13, 2021, 10:34 PM
 */

#ifndef TEMP_DS18B20_H
#define	TEMP_DS18B20_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
    
void temp_DS18B20_Init();
void temp_DS18B20_Task();
int16_t temp_DS18B20_Read(uint8_t index);


#ifdef	__cplusplus
}
#endif

#endif	/* TEMP_DS18B20_H */

