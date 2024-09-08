/* 
 * File:   gui.h
 * Author: husak
 *
 * Created on 17. listopadu 2020, 18:29
 */

#ifndef GUI_H
#define	GUI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool minusActive;
    bool enterActive;
    bool plusActive;
    
    bool minusRisingEdge;
    bool enterRisingEdge;
    bool plusRisingEdge;
    
    bool minusPressHold;
    bool enterPressHold;
    bool plusPressHold;
    
    uint32_t minusRisingEdgeTime;
    uint32_t enterRisingEdgeTime;
    uint32_t plusRisingEdgeTime;
    
    uint32_t minusRepeaterTime;
    uint32_t enterRepeaterTime;
    uint32_t plusRepeaterTime;
    
} button_control_t;


void gui_Init();
void gui_Task(button_control_t *button_control);

#endif	/* GUI_H */

