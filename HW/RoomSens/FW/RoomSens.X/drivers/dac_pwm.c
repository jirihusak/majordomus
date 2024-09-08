
#include <stdio.h>
#include "i2c_BitBang.h"
#include "../mcc_generated_files/pwm7.h"
#include "../mcc_generated_files/pwm8.h"

#define DAC_MV_MAX          10000 // max 10V
#define PWM_MAX_VALUE       (DAC_MV_MAX / 1023.0) // specific for this settings in MCC based on TMR4 100kHz


static float temp = (float)DAC_MV_MAX / (float)PWM_MAX_VALUE;

void dac_pwm_set_milivolts_CH0(uint16_t mv)
{
    if(mv > DAC_MV_MAX) mv = DAC_MV_MAX;
    
    PWM7_LoadDutyValue(mv / PWM_MAX_VALUE);
}

void dac_pwm_set_milivolts_CH1(uint16_t mv)
{
    if(mv > DAC_MV_MAX) mv = DAC_MV_MAX;
    
    PWM8_LoadDutyValue(mv / PWM_MAX_VALUE);
}