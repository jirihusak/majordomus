/* 
 * File:   dac_pwm.h
 * Author: husak

 */

#ifndef DAC_PWM_H
#define	DAC_PWM_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    void dac_pwm_set_milivolts_CH0(uint16_t mv);
    void dac_pwm_set_milivolts_CH1(uint16_t mv);




#ifdef	__cplusplus
}
#endif

#endif	/* DAC_PWM_H */

