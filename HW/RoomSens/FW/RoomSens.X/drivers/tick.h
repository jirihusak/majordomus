/* 
 * File:   tick.h
 * Author: jiri
 *
 * Created on December 30, 2017, 12:06 PM
 */

#ifndef TICK_H
#define	TICK_H

#include <stdint.h>


#define TICK_MILISECOND     (uint32_t)1
#define TICK_SECOND         (uint32_t)1000
#define TICK_MINUTE         (uint32_t)60000
#define TICK_HOUR           (uint32_t)3600000




void tick_Init();
void tick_Increment();
uint32_t tick_getTick();

#endif	/* TICK_H */

