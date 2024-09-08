

#include "piezo.h"
#include "../mcc_generated_files/mcc.h"
#include "tick.h"

//enum piezo_beep_type {NONE, TOUCH, SHORT_BEEP, DOUBLE_BEEP, BELL, CONTINUOUS, TYPE_END};
typedef struct{
    uint16_t frequency;
    uint16_t delay_ms;
} t_note;

const t_note sheets[TYPE_END][20] = { 
    { {0,0} }, //NONE
    { {0,0}}, //TOUCH
    { {100,300}, {0,0}}, //SHORT_BEEP
    { {120,300}, {0,300}, {120,300}, {0,0}}, //DOUBLE_BEEP
    { {250,200}, {0,200}, {250,200}, {0,200}, {250,200}, {0,200},/**/ 
        {200,200}, {0,200}, {200,200}, {0,200}, {200,200}, {0,200},/**/
        {150,200}, {0,200}, {150,200}, {0,200}, {150,200}, {0,200},/**/{0,0}, }, //BELL
    { {80,5000}, {0,0} }, //CONTINUOUS
};

static enum {
    PIEZO_SOUND,
    PIEZO_IDLE,
} state = PIEZO_IDLE;
static uint32_t fsm_timer = 0;

//
//typedef struct{
//    t_note struct_array[20];
//} t_sheet;

static struct {
    int actual_type;
    uint32_t timer;
    uint16_t actual_index_in_sheet;
} piezo_control;
//= {
//    .actual_type = NONE,
//    .actual_index_in_sheet = 0
//};

void piezo_beep_start(int type)
{
    PWM6_LoadDutyValue(0);
    CCPTMRS1bits.P6TSEL = 3;
    TMR6_StartTimer();
    
    if(type == TOUCH){
        RD2_SetDigitalOutput();
        TMR6_Period8BitSet(0x40);
        PWM6_LoadDutyValue(199);
        __delay_us(500);
        PWM6_LoadDutyValue(0);
        RD2_SetDigitalInput();
    }
    else
    {
        piezo_control.actual_type = type;
        piezo_control.actual_index_in_sheet = 0;
        piezo_control.timer = 0;
        state = PIEZO_SOUND;
        // set first note
    }
}

void piezo_beep_stop()
{
    PWM6_LoadDutyValue(0);
    TMR6_Period8BitSet(0x30);
    TMR6_StopTimer();
    RD2_SetDigitalInput();
    
    state = PIEZO_IDLE;
}

void piezo_Task()
{
    t_note *actual = &(sheets[piezo_control.actual_type][piezo_control.actual_index_in_sheet]);
    
    switch(state)
    {
        case PIEZO_SOUND:
            
            // is in the stop
            if(actual->delay_ms == 0 && actual->frequency == 0 && ((tick_getTick() - piezo_control.timer) > actual->delay_ms)){
                piezo_beep_stop();
                piezo_control.actual_type = NONE;
            }

            else if( (piezo_control.actual_index_in_sheet == 0) && (piezo_control.timer == 0)){

                RD2_SetDigitalOutput();
                TMR6_Period8BitSet(actual->frequency);
                PWM6_LoadDutyValue(actual->frequency > 0 ? 199 : 0);
                piezo_control.timer = tick_getTick();
            }


            else if( (tick_getTick() - piezo_control.timer) > actual->delay_ms){
                piezo_control.actual_index_in_sheet++;
                actual = &(sheets[piezo_control.actual_type][piezo_control.actual_index_in_sheet]);

                TMR6_Period8BitSet(actual->frequency);
                PWM6_LoadDutyValue(actual->frequency > 0 ? 199 : 0);
                piezo_control.timer = tick_getTick();
            }
            break;
            
        case PIEZO_IDLE:
            break;
    }
    
    
}

uint8_t piezo_is_active()
{
    return state == PIEZO_SOUND ? 1 : 0;
}