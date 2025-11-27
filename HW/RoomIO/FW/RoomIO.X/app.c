/*
 * Room IO application
 * Author: Jiri Husak
 * Majordomus
 */

#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>

#include "app.h"
#include "mcc_generated_files/mcc.h"
#include "drivers/tick.h"
#include "drivers/i2c_BitBang.h"
#include "device_memory.h"
#include "serial_interface.h"
#include "digital_io.h"
#include "drivers/temp_ds18B20.h"
#include "drivers/temp_TMP1075.h"
#include "drivers/dac_pwm.h"
#include "drivers/fram_MB85RC.h"


meas_struct_t meas_values;

void app_Init(){
    
    LED_WORK_SetHigh();
    
    tick_Init();
    i2c_bb_init();
    device_memory_init();
    dac_pwm_set_milivolts_CH0(0);
    dac_pwm_set_milivolts_CH1(0);
  
    // wait for startup all peripheries
    __delay_ms(50);
        
    temp_DS18B20_Init();
    
    serialInterface_init();
    
    digital_io_init();
    
    CPUDOZEbits.ROI = 1; // disable low power mode on interupts
    TMR6_StartTimer();

}

void app_Task(){
    
    static uint32_t ledTimer = 0;
    static uint32_t adcTimer = 0;
    static uint32_t digiTimer = 0;
    
    SYSTEM_CPU_LOW_POWER;
    
    // read MUI
    //uint32_t id_low, id_high;
    //device_memory_get_device_ID(&id_low, &id_high);
    
    
    // no delay tasks
    SYSTEM_CPU_HIGH_SPEED;
    serialInterface_process();
    SYSTEM_CPU_LOW_POWER;
          
    // high speed tasks
    if((tick_getTick()-digiTimer) >= 10*TICK_MILISECOND){
        digiTimer = tick_getTick();
       
        SYSTEM_CPU_HIGH_SPEED;
        digital_io_task();
        //temp_DS18B20_Task();
        SYSTEM_CPU_LOW_POWER;
    }
    
    // ADC, DAC
    if((tick_getTick()-adcTimer) >= 1000*TICK_MILISECOND){
        adcTimer = tick_getTick();
        
        SYSTEM_CPU_HIGH_SPEED;
                  
        meas_values.power = ADCC_Get_Millivolts(ADC_V_IN) * 11; // 11 -> 1:10 R divider
        meas_values.power_outputs = ADCC_Get_Millivolts(ADC_V_IN_IO) * 11; // 11 -> 1:10 R divider
        meas_values.adc0 = ADCC_Get_Millivolts(ADC_0) * 11; // 11 -> 1:10 R divider
        meas_values.adc1 = ADCC_Get_Millivolts(ADC_1) * 11; // 11 -> 1:10 R divider
        //meas_values.temperature_ext_0 = temp_DS18B20_Read(0);
        //meas_values.temperature_ext_1 = temp_DS18B20_Read(1);
        digital_io_counter_save(); // periodically save to FRAM
        //printf("power %d, outputs %d, adc0 %d, adc1 %d \r\n", meas_values.power, meas_values.power_outputs, meas_values.adc0, meas_values.adc1);
        
        
        // debug
//        uint64_t data;
//        fram_MB85_read64(0, &data);
//        
//        data += 1;
//        fram_MB85_write64(0, data);
//        
//        printf("%d\r\n", (uint16_t)data);
    
        SYSTEM_CPU_LOW_POWER;
    }
    
    // LED blik
    if((tick_getTick()-ledTimer) >= 200*TICK_MILISECOND){
        ledTimer = tick_getTick();
        
//        LED_WORK_SetLow();
//        LED_WORK_SetHigh();
        CLRWDT(); // clear WDG
    }
}
