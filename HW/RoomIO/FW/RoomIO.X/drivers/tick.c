
#include <stdint.h>
#include "../mcc_generated_files/tmr0.h"
#include "tick.h"

uint32_t ticks = 0;

void tick_Init(){
    TMR0_SetInterruptHandler(&tick_Increment);
}

void tick_Increment(){
    ticks++;
}

uint32_t tick_getTick(){
    return ticks;
}
