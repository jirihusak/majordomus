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

#define BOARD_TYPE              "TinySensor"
#define APP_VERSION             "0.1"
#define BUILD_DATE              __DATE__ 


#define SYSTEM_CPU_HIGH_SPEED { CPUDOZEbits.DOZE = 0; CPUDOZEbits.DOZEN = 0; }
#define SYSTEM_CPU_LOW_POWER { CPUDOZEbits.DOZE = 5; CPUDOZEbits.DOZEN = 1;}

typedef struct {
    uint16_t power;
    uint16_t power_outputs;
    int32_t temperature;
    int32_t rHumidity;
   
    int16_t temperature_ext_0;  
    int16_t temperature_ext_1;  

} meas_struct_t;

extern meas_struct_t meas_values;

void app_Init();
void app_Task();




#endif	/* APP_H */

