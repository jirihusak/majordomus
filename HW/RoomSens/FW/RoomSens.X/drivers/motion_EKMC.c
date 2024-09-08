
#include "stdbool.h"
#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/pin_manager.h"
#include "tick.h"


static uint16_t counter;
static uint32_t timer;
static enum {
    STATE_IDLE,
    STATE_RISING_EDGE,
    STATE_DETECTING_HUMAN
} state = STATE_IDLE;


void motion_Init()
{
    PIR_SIGNAL_SetDigitalInput();
}

void motion_Task()
{
    bool movement = (PIR_SIGNAL_GetValue() == 1);
    
    switch(state)
    {
        case STATE_IDLE:
            if(movement)
            {
                state = STATE_RISING_EDGE;
                counter = 0;
                timer = tick_getTick();
            }
            break;
            
        case STATE_RISING_EDGE:
            if(movement) {
                counter++;
            }
            if(counter > 2) {
                counter = 0;
                timer = tick_getTick();
                state = STATE_DETECTING_HUMAN;
            }
            
            if((tick_getTick()-timer) >= 3*TICK_SECOND){
                state = STATE_IDLE;
            }
            break;
            
        case STATE_DETECTING_HUMAN:
            
            if(movement){
                timer = tick_getTick();
            }
            
            if((tick_getTick()-timer) >= 5*TICK_SECOND){
                state = STATE_IDLE;
            }
            
            break;
    }
}

uint8_t motion_get_detector(){
    
    if(state == STATE_DETECTING_HUMAN)
    {
        return 1;
    }
    return 0;
        
}
