/* 
 * File:   microphone.h
 * Author: husak
 *
 * Created on December 6, 2020, 10:42 PM
 */

#include <stdint.h>

#ifndef MICROPHONE_H
#define	MICROPHONE_H

void microphone_task();
uint32_t microphone_get_noise_level();
uint8_t microphone_is_sampling();

#endif	/* MICROPHONE_H */

