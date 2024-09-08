
#include <string.h>

#include "device_memory.h"

/*
 * EEPROM STRUCTURE
 * (1024 Bytes)
 * 
 * 0 - 31 B: string Device Name
 
 
 
 */

t_eeprom_struct eeprom_memory;

static uint32_t device_id_low;
static uint32_t device_id_high;

void device_memory_eeprom_load();
void device_memory_device_eeprom_save();

void device_memory_init()
{
    // Read MUI 
    uint32_t start_addr = 0x3F0000;
    int i;
    uint16_t mui_array[6];
    
    for (i = 0; i < 6; i++) {
        mui_array[i] = FLASH_ReadWord(start_addr + (i *2));

    }
    device_id_low = mui_array[1] << 16;
    device_id_low = (device_id_low << 16) | mui_array[0];
    
    device_id_high = mui_array[3] << 16;
    device_id_high = (device_id_high << 16) | mui_array[2];
        
    // Read EEPROM to struct
    device_memory_eeprom_load();
    
    // init data if is not initilised
    if(eeprom_memory.memory_struct_version != DEVICE_STRUCT_VERSION){
        
        eeprom_memory.memory_struct_version = DEVICE_STRUCT_VERSION;
        
        eeprom_memory.device_name[0] = 't';
        eeprom_memory.device_name[1] = 'e';
        eeprom_memory.device_name[2] = 's';
        eeprom_memory.device_name[3] = 't';
        eeprom_memory.device_name[4] = '\0';

        device_memory_device_eeprom_save();
    }
    
}

bool device_has_external_oscilator()
{
    uint8_t data;
    
    data = FLASH_ReadByte(0x300000);
    
    data = (data >> 4) & 0x7;
    
    if(data == 2)
    {
        return true;
    }
    
    return false;
    
}

void device_memory_get_device_ID(uint32_t *low, uint32_t *high)
{
    *low = device_id_low;
    *high = device_id_high;
}

void device_memory_eeprom_load()
{
    uint8_t *data_ptr = (uint8_t*) &eeprom_memory;
    memset(data_ptr, 0, sizeof(t_eeprom_struct));
    
    int i;
    for (i = 0; i < sizeof(t_eeprom_struct); i++) {
        data_ptr[i] = DATAEE_ReadByte(i);
    }
  
}

void device_memory_device_eeprom_save()
{
    uint8_t *data_ptr = (uint8_t*) &eeprom_memory;
    int i;
    for (i = 0; i < sizeof(t_eeprom_struct); i++) {
        DATAEE_WriteByte(i, data_ptr[i]);
    }
    
}

