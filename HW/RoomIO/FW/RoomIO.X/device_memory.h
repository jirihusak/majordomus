/* 
 * File:   device_config.h
 * Author: husak
 *
 * Created on October 3, 2019, 9:18 PM
 */

#ifndef DEVICE_MEMORY_H
#define	DEVICE_MEMORY_H

#include <stdint.h>
#include <pic18.h>
#include <stdbool.h>
#include "mcc_generated_files/mcc.h"
  
void device_memory_init();
void device_memory_get_device_ID(uint32_t *low, uint32_t *high);
void device_memory_eeprom_load();
void device_memory_device_eeprom_save();
bool device_has_external_oscilator();

#define DEVICE_STRUCT_VERSION               100
#define DEVICE_MEMORY_MAX_NAME_LENGTH       32

typedef struct {
    uint32_t bootloader;
    uint32_t memory_struct_version;
    uint8_t device_name[DEVICE_MEMORY_MAX_NAME_LENGTH];
    uint8_t modbusAddress;
} t_eeprom_struct;

extern t_eeprom_struct eeprom_memory;


#endif	/* DEVICE_MEMORY_H */

