/* 
 * File:   app.h
 * Author: jiri
 *
 * Created on 14. prosince 2017, 22:20
 */

#ifndef APP_H
#define	APP_H

#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>

#define BOARD_TYPE              "RoomSens"
#define APP_VERSION             "0.3"
#define BUILD_DATE              __DATE__ 


#define SYSTEM_CPU_HIGH_SPEED { CPUDOZEbits.DOZE = 0; CPUDOZEbits.DOZEN = 0; }
#define SYSTEM_CPU_LOW_POWER { CPUDOZEbits.DOZE = 0; CPUDOZEbits.DOZEN = 0;} // 5/1

typedef struct {
    uint16_t power;
    uint16_t power_outputs;
    uint16_t adc0;
    uint16_t adc1;
    
    int16_t temperature;  
    int16_t temperature2;
    int16_t humidity;
    int16_t co2;
    int16_t voc;
    uint16_t air_quality_index; // 1-5
    int32_t lux;
    int16_t motion;
    int16_t noise;
    int16_t temperature_ext_0;  
    int16_t temperature_ext_1;  

} meas_struct_t;

typedef struct {
    int16_t req_temp;
    uint16_t heater_active;
    uint16_t light_active;
    uint16_t fan_active;
    
    bool req_temp_trigger;
    int16_t new_req_temp;

} control_struct_t;

extern meas_struct_t meas_values;
extern control_struct_t control_struct;

void app_Init();
void app_Task();




#endif	/* APP_H */

