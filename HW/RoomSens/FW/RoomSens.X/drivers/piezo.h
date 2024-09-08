/* 
 * File:   piezo.h
 * Author: husak
 *
 * Created on January 13, 2020, 9:43 PM
 */

#ifndef PIEZO_H
#define	PIEZO_H

#include <stdint.h> 

#ifdef	__cplusplus
extern "C" {
#endif
    
    enum piezo_beep_type {NONE, TOUCH, SHORT_BEEP, DOUBLE_BEEP, BELL, CONTINUOUS, TYPE_END};
    
    void piezo_beep_start(int type);
    void piezo_beep_stop();
    void piezo_Task();
    uint8_t piezo_is_active();




#ifdef	__cplusplus
}
#endif

#endif	/* PIEZO_H */

