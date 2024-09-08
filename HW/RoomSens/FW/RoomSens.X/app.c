/*
 * Room Board application
 * 
 * Date: 19.12.2017
 * Author: Jiri Husak
 */

#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>

#include "app.h"
#include "mcc_generated_files/mcc.h"
#include "drivers/tick.h"
#include "drivers/motion_EKMC.h"
#include "drivers/lcd_NHDC12832.h"
#include "drivers/i2c_BitBang.h"
#include "drivers/light_VEML7700.h"
#include "device_memory.h"
#include "serial_interface.h"
#include "drivers/piezo.h"
#include "digital_io.h"
#include "gui.h"
#include "drivers/adc_internal.h"
#include "drivers/temp_ds18B20.h"
#include "drivers/temp_TMP1075.h"
#include "drivers/voc_SGP40.h"
#include "drivers/tempHum_SHT41.h"
#include "drivers/microphone.h"
#include "drivers/dac_pwm.h"

//#define APP_ENTRY_TERMINAL
#define APP_ROOM_SENSOR

meas_struct_t meas_values;
control_struct_t control_struct;
button_control_t button_control;

/*
 * PWM5 - LCD Backlight
 * PWM6 - Piezo
 * PWM7 - DAC 0
 * PWM8 - DAC 1
 */

void app_Init(){
    
    LED_WORK_SetHigh();
    LED_AMBIENT_SetLow();
    
    tick_Init();
    i2c_bb_init();
    device_memory_init();
    //piezo_beep_stop();
  
    // nabehnuti vsech zarizeni, nez se zacne komunikovat..
    __delay_ms(10);
    
    temp_TMP1075_init();
    
    temp_DS18B20_Init();
    
    serialInterface_init();
    
    digital_io_init();
    
    CPUDOZEbits.ROI = 1; // disable low power mode on interupts
    
      
    // init display
    gui_Init();

    //init lux meter
    light_VEML7700_init();   
    
    voc_SGP40_Init();
    
    motion_Init();
    
    TMR4_StartTimer(); // for pwm 5 - LCD backlight
    dac_pwm_set_milivolts_CH0(0);
    dac_pwm_set_milivolts_CH1(0);
    //PWM5_LoadDutyValue(50);
    
}

void app_Task(){
    
    static uint32_t ledTimer = 0;
    static uint32_t motionTimer = 0;
    static uint32_t measTimer = 0;
    static uint32_t adcTimer = 0;
    static uint32_t displayTimer = 0;
    static uint32_t buttonTimer = 0;
    static uint32_t serialTimer = 0;
    static uint32_t digiTimer = 0;
    static uint32_t micTimer = 0;
    
    SYSTEM_CPU_LOW_POWER;
    
    // read MUI
    uint32_t id_low, id_high;
    device_memory_get_device_ID(&id_low, &id_high);
    
    
    // touch buttons
    
    if((tick_getTick()-buttonTimer) >= 1*TICK_MILISECOND){
        buttonTimer = tick_getTick();
    
        SYSTEM_CPU_HIGH_SPEED;
        if(!MTOUCH_Service_Mainloop()) {
            
            //printf("%d %d %d\r\n", MTOUCH_Button_Reading_Get(BTN_MINUS), MTOUCH_Button_Reading_Get(BTN_ENTER), MTOUCH_Button_Reading_Get(BTN_PLUS));

            if(button_control.plusActive == false && MTOUCH_Button_isPressed(BTN_PLUS) && (tick_getTick() - button_control.plusRisingEdgeTime > (50*TICK_MILISECOND) )){
                piezo_beep_start(TOUCH);
                button_control.plusRisingEdge = true;
                button_control.plusPressHold = false;
                button_control.plusRisingEdgeTime = tick_getTick();
                button_control.plusRepeaterTime = tick_getTick();
            }
            if(button_control.plusActive == true && (tick_getTick() - button_control.plusRisingEdgeTime > (1500*TICK_MILISECOND)) && (tick_getTick() - button_control.plusRepeaterTime > (400*TICK_MILISECOND)))
            {
                piezo_beep_start(TOUCH);
                button_control.plusPressHold = true;
                button_control.plusRepeaterTime = tick_getTick();
            }

            if(button_control.minusActive == false && MTOUCH_Button_isPressed(BTN_MINUS) && (tick_getTick() - button_control.minusRisingEdgeTime > (50*TICK_MILISECOND)) ){
                piezo_beep_start(TOUCH);
                button_control.minusRisingEdge = true;
                button_control.minusPressHold = false;
                button_control.minusRisingEdgeTime = tick_getTick();
                button_control.minusRepeaterTime = tick_getTick();
            }
            if(button_control.minusActive == true && (tick_getTick() - button_control.minusRisingEdgeTime > (1500*TICK_MILISECOND)) && (tick_getTick() - button_control.minusRepeaterTime > (400*TICK_MILISECOND)))
            {
                piezo_beep_start(TOUCH);
                button_control.minusPressHold = true;
                button_control.minusRepeaterTime = tick_getTick();
            }

            if(button_control.enterActive == false && MTOUCH_Button_isPressed(BTN_ENTER) && (tick_getTick() - button_control.enterRisingEdgeTime > (50*TICK_MILISECOND))){
                piezo_beep_start(TOUCH);
                button_control.enterRisingEdge = true;
                button_control.enterPressHold = false;
                button_control.enterRisingEdgeTime = tick_getTick();
                button_control.enterRepeaterTime = tick_getTick();
                //printf("enter rising edge\r\n");
            }
            if(button_control.enterActive == true && (tick_getTick() - button_control.enterRisingEdgeTime > (1500*TICK_MILISECOND)) && (tick_getTick() - button_control.enterRepeaterTime > (400*TICK_MILISECOND)))
            {
                piezo_beep_start(TOUCH);
                button_control.enterPressHold = true;
                button_control.enterRepeaterTime = tick_getTick();
                //printf("enter repeater\r\n");
            }

            if(MTOUCH_Button_isPressed(BTN_PLUS)) {
                button_control.plusActive = true;
            }
            else {
                button_control.plusActive = false;
                button_control.plusPressHold = false;
            }

            if(MTOUCH_Button_isPressed(BTN_MINUS)) button_control.minusActive = true;
            else { 
                button_control.minusActive = false;
                button_control.minusPressHold = false;
            }

            if(MTOUCH_Button_isPressed(BTN_ENTER)) {
                //printf("enter pressed\r\n");
                button_control.enterActive = true;
            }
            else { 
                button_control.enterActive = false;
                button_control.enterPressHold = false;
                //printf("enter released\r\n");
            }
        }
        SYSTEM_CPU_LOW_POWER;
    }
    
    
    // no delay tasks
    SYSTEM_CPU_HIGH_SPEED;
    motion_Task();
    serialInterface_process();
    tempHum_SHT41_Task();
    if(!piezo_is_active()) {
        microphone_task();
    }
    //if(!microphone_is_sampling()){
    piezo_Task();
    //}
    
    SYSTEM_CPU_LOW_POWER;
          
    // high speed tasks
    if((tick_getTick()-digiTimer) >= 10*TICK_MILISECOND){
        digiTimer = tick_getTick();
       
        SYSTEM_CPU_HIGH_SPEED;
        digital_io_task();
        if(!microphone_is_sampling()){
            gui_Task(&button_control);
        }
        temp_DS18B20_Task();
        voc_SGP40_Task();
        SYSTEM_CPU_LOW_POWER;
        
    }
    
    // high speed tasks
    if((tick_getTick()-micTimer) >= 50*TICK_MILISECOND){
        micTimer = tick_getTick();
       
        SYSTEM_CPU_HIGH_SPEED;
        meas_values.noise = microphone_get_noise_level();
        SYSTEM_CPU_LOW_POWER;  
    }
        
    // low speed tasks
    if((tick_getTick()- measTimer) >= 1000*TICK_MILISECOND){
        measTimer = tick_getTick();
        
        SYSTEM_CPU_HIGH_SPEED;
        
        uint8_t addr = 1;
        uint8_t data;

//        printf("Search I2C addr \r\n");
//
//        for(; addr < 127; addr++)
//        {
//            uint8_t retVal = i2c_bb_rx(addr, data, 1);
//
//            if(retVal != 0)
//            {
//                printf("I2C addr : %x\r\n", addr);
//            }
//        }
        
        
        light_VEML7700_meas(&(meas_values.lux));
        meas_values.temperature = temp_TMP1075_read_temperature()/10;
        tempHum_SHT41_meas(&meas_values.temperature2, &meas_values.humidity);
        meas_values.temperature_ext_0 = temp_DS18B20_Read(0);
        meas_values.temperature_ext_1 = temp_DS18B20_Read(1);
        meas_values.motion = motion_get_detector();
        meas_values.voc = voc_SGP40_get_air_quality();
        meas_values.noise = microphone_get_noise_level();
        meas_values.motion = motion_get_detector();
        
        //printf("temp:%d, lux:%ld, voc:%d, t:%d, h:%d, mot: %d\r\n", 
        //        meas_values.temperature, meas_values.lux, meas_values.voc, meas_values.temperature2, meas_values.humidity, meas_values.motion);
        
        //piezo_beep_start(DOUBLE_BEEP);
        
        SYSTEM_CPU_LOW_POWER;
        
        
        
    }
    
    // ADC 
    if((tick_getTick()-adcTimer) >= 1000*TICK_MILISECOND && MTOUCH_Sensor_isSampling() == false){
        adcTimer = tick_getTick();
        
        SYSTEM_CPU_HIGH_SPEED;
        
        ADC_Internal_Disable();
        ADC_Internal_Initialize();
          
        
        //meas_values.noise = microphone_get_noise_level();
        meas_values.power = ADC_Internal_Get_Millivolts(ADC_V_IN) * 11; // 11 -> 1:10 R divider
        meas_values.power_outputs = ADC_Internal_Get_Millivolts(ADC_V_IN_IO) * 11; // 11 -> 1:10 R divider
        meas_values.adc0 = ADC_Internal_Get_Millivolts(ADC_0) * 11; // 11 -> 1:10 R divider
        meas_values.adc1 = ADC_Internal_Get_Millivolts(ADC_1) * 11; // 11 -> 1:10 R divider
        
        //printf("power %d, outputs %d, adc0 %d, adc1 %d \r\n", meas_values.power, meas_values.power_outputs, meas_values.adc0, meas_values.adc1);
    
        ADC_Internal_Disable();
        MTOUCH_Sensor_Scan_Initialize();
        SYSTEM_CPU_LOW_POWER;
    }
    
       
    // LED blik
    if((tick_getTick()-ledTimer) >= 500*TICK_MILISECOND){
        ledTimer = tick_getTick();
        
//        LED_WORK_SetLow();
//        LED_WORK_SetHigh();
        //LED_WORK_Toggle();
        CLRWDT(); // clear WDG
    }
    
 
}
