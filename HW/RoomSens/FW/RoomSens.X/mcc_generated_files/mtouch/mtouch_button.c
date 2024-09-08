/*
    MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:

    You may use this software, and any derivatives created by any person or
    entity by or on your behalf, exclusively with Microchip's products.
    Microchip and its subsidiaries ("Microchip"), and its licensors, retain all
    ownership and intellectual property rights in the accompanying software and
    in all derivatives hereto.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products.  You agree
    that you are solely responsible for testing the software and determining
    its suitability.  Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH MICROCHIP'S
    PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
    (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY,
    INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF
    ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
    FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL
    LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
    THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
    THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF
    THESE TERMS.
*/
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mtouch.h"
#include "mtouch_button.h"


enum mtouch_button_state
{
    MTOUCH_BUTTON_STATE_initializing = 0,
    MTOUCH_BUTTON_STATE_notPressed,
    MTOUCH_BUTTON_STATE_pressDebounce,
    MTOUCH_BUTTON_STATE_pressed,    
    MTOUCH_BUTTON_STATE_releaseDebounce
};


    enum mtouch_button_hysteresis_thresholds
    {
        HYST_50_PERCENT     = 1,
        HYST_25_PERCENT     = 2,
        HYST_12_5_PERCENT   = 3,
        HYST_6_25_PERCENT   = 4,
        HYST_MAX            = 5
    };


/* 
 * =======================================================================
 * Button Type Structure
 * =======================================================================
 */
    typedef struct
    {
        const enum mtouch_button_names      name;
        const enum mtouch_sensor_names      sensor;
        enum mtouch_button_state            state;
        mtouch_button_reading_t         reading;
        mtouch_button_baseline_t        baseline;
        mtouch_button_deviation_t       deviation;
        mtouch_button_statecounter_t    counter;
        mtouch_button_baselinecounter_t baseline_count;
        mtouch_button_threshold_t       threshold;
        mtouch_button_scaling_t         scaling;
    } mtouch_button_t;

    const mtouch_button_t mtouch_button_init[MTOUCH_BUTTONS] =
    {
        /* BTN_PLUS */
        {   BTN_PLUS, 
            MTOUCH_BUTTON_SENSOR_BTN_PLUS,
            MTOUCH_BUTTON_STATE_initializing,
            0,0,0,0,0, /* non-const variables */
            (mtouch_button_deviation_t)MTOUCH_BUTTON_THRESHOLD_BTN_PLUS, /* threshold */
            (mtouch_button_scaling_t)MTOUCH_BUTTON_SCALING_BTN_PLUS /* scaling */
        },
        /* BTN_ENTER */
        {   BTN_ENTER, 
            MTOUCH_BUTTON_SENSOR_BTN_ENTER,
            MTOUCH_BUTTON_STATE_initializing,
            0,0,0,0,0, /* non-const variables */
            (mtouch_button_deviation_t)MTOUCH_BUTTON_THRESHOLD_BTN_ENTER, /* threshold */
            (mtouch_button_scaling_t)MTOUCH_BUTTON_SCALING_BTN_ENTER /* scaling */
        },
        /* BTN_MINUS */
        {   BTN_MINUS, 
            MTOUCH_BUTTON_SENSOR_BTN_MINUS,
            MTOUCH_BUTTON_STATE_initializing,
            0,0,0,0,0, /* non-const variables */
            (mtouch_button_deviation_t)MTOUCH_BUTTON_THRESHOLD_BTN_MINUS, /* threshold */
            (mtouch_button_scaling_t)MTOUCH_BUTTON_SCALING_BTN_MINUS /* scaling */
        }
    };
    
static mtouch_button_t mtouch_button[MTOUCH_BUTTONS];

/*
 * =======================================================================
 *  Local Functions
 * =======================================================================
 */
static void                     Button_Service              (enum mtouch_button_names name);  
static void                     Button_Deviation_Update     (mtouch_button_t* button);
static void                     Button_Reading_Update       (mtouch_button_t* button);
static void                     Button_Reading_Update_Helper(mtouch_button_t* button);
static void                     Button_Baseline_Initialize  (mtouch_button_t* button);
static void                     Button_Baseline_Update      (mtouch_button_t* button);
static mtouch_button_reading_t  Button_Baseline_Get_helper  (enum mtouch_button_names button);

static void                     Button_State_Initializing   (mtouch_button_t* button);
static void                     Button_State_NotPressed     (mtouch_button_t* button);
static void                     Button_State_Pressed        (mtouch_button_t* button);
static void                     Button_State_PressDebounce  (mtouch_button_t* button);
static void                     Button_State_ReleaseDebounce(mtouch_button_t* button);



/*
 * =======================================================================
 * Button State machine
 * =======================================================================
 */
typedef void (*button_statemachine_state_t)(mtouch_button_t*);
button_statemachine_state_t Button_StateMachine[] = 
{
    Button_State_Initializing,
    Button_State_NotPressed,
    Button_State_PressDebounce,
    Button_State_Pressed,    
    Button_State_ReleaseDebounce
};
#define BUTTON_STATEMACHINE_COUNT (uint8_t)(sizeof(Button_StateMachine)/sizeof(button_statemachine_state_t))

/*
 * =======================================================================
 *  MTOUCH_Button_Initialize
 * =======================================================================
 */
void MTOUCH_Button_Initialize(enum mtouch_button_names name)
{
    mtouch_button_t* button = &mtouch_button[name];

    button->state           = MTOUCH_BUTTON_STATE_initializing;
    button->baseline        = (mtouch_button_baseline_t)0;
    button->counter         = (mtouch_button_statecounter_t)0;
    button->baseline_count  = (mtouch_button_baselinecounter_t)0;
    
    MTOUCH_Sensor_Calibrate(button->sensor);
}

void MTOUCH_Button_InitializeAll(void)
{
    enum mtouch_button_names button;    
    
    memcpy(mtouch_button,mtouch_button_init,sizeof(mtouch_button_init));

    for (button = 0; button < MTOUCH_BUTTONS; button++)
    {
        MTOUCH_Button_Initialize(button);
    }
}

/*
 * =======================================================================
 * Button Service
 * =======================================================================
 */

 void MTOUCH_Button_ServiceAll(void)
{
    enum mtouch_button_names button; 
    
    
    for (button = 0; button < MTOUCH_BUTTONS; button++)
    {  
        Button_Service(button);
    }
}
 
static void Button_Service(enum mtouch_button_names name)
{
    mtouch_button_t* button = &mtouch_button[name];
    
    if (MTOUCH_Sensor_wasSampled(button->sensor) && MTOUCH_Sensor_isCalibrated(button->sensor))
    {
        Button_Reading_Update(button);
        Button_Deviation_Update(button);

        if (button->state >= BUTTON_STATEMACHINE_COUNT)
        {
            MTOUCH_Button_Initialize(button->name);
        }
        Button_StateMachine[(uint8_t)button->state](button);
    }
}

static void Button_State_Initializing(mtouch_button_t* button) 
{
    /* Initialize the baseline. */
    if ((button->counter) <= (mtouch_button_statecounter_t)MTOUCH_BUTTON_BASELINE_INIT/2)
    { 
        Button_Baseline_Initialize(button); /* Rough resolution */
    } else { 
        Button_Baseline_Update(button); /* Fine resolution */
    }
    
    /* Exit after a fixed number of baseline updates */
    (button->counter)++;
    if ((button->counter) >= (mtouch_button_statecounter_t)MTOUCH_BUTTON_BASELINE_INIT)
    {
        button->reading = (mtouch_button_reading_t)((button->baseline)>>MTOUCH_BUTTON_BASELINE_GAIN);
        button->state   = MTOUCH_BUTTON_STATE_notPressed;
        button->counter = (mtouch_button_statecounter_t)0;
    }
    
}
static void Button_State_NotPressed(mtouch_button_t* button)
{
    /* Negative Capacitance check */
    if ((button->deviation) < (mtouch_button_deviation_t)(-button->threshold))
    {
        (button->counter)++;
        if ((button->counter) > (mtouch_button_statecounter_t)MTOUCH_BUTTON_NEGATIVEDEVIATION)
        {
            button->counter = (mtouch_button_statecounter_t)0;
            MTOUCH_Button_Initialize(button->name);
        }
    }
    /* Threshold check */
    else if ((button->deviation) > (button->threshold))
    {

        button->state   = MTOUCH_BUTTON_STATE_pressDebounce;
        button->counter = (mtouch_button_statecounter_t)0;
    }
    else
    {
        /* Delta is positive, but not past the threshold. */
        button->counter = (mtouch_button_statecounter_t)0;
    }
    
    
    /* Baseline Update check */
    (button->baseline_count)++; 
    if ((button->baseline_count) == MTOUCH_BUTTON_BASELINE_RATE)
    {
        button->baseline_count = (mtouch_button_baselinecounter_t)0;
        Button_Baseline_Update(button);
    }
}

static void Button_State_Pressed(mtouch_button_t* button)
{
    
    /* Threshold check */
    if ((button->deviation) < (mtouch_button_deviation_t)((button->threshold)-((button->threshold) >> MTOUCH_BUTTON_COMMON_HYSTERESIS)))
    {
        button->state   = MTOUCH_BUTTON_STATE_releaseDebounce;
        button->counter = (mtouch_button_statecounter_t)0;
    }
}

static void Button_State_PressDebounce  (mtouch_button_t* button)
{
    
    if((button->deviation) > (button->threshold))
    {
        (button->counter)++;
        if ((button->counter) >= MTOUCH_BUTTON_DEBOUNCE_COUNT)
        {
            button->state   = MTOUCH_BUTTON_STATE_pressed;
            button->counter = (mtouch_button_statecounter_t)0;
        }
    }
    else
    {
        button->state   = MTOUCH_BUTTON_STATE_notPressed;
        button->counter = (mtouch_button_statecounter_t)0;
    }
}

static void Button_State_ReleaseDebounce(mtouch_button_t* button)
{
    if ((button->deviation) < (mtouch_button_deviation_t)((button->threshold)-((button->threshold) >> MTOUCH_BUTTON_COMMON_HYSTERESIS)))
    {
        (button->counter)++;
        if ((button->counter) >= MTOUCH_BUTTON_DEBOUNCE_COUNT)
        {
            button->state   = MTOUCH_BUTTON_STATE_notPressed;
            button->counter = 0;
            button->baseline_count = (mtouch_button_baselinecounter_t)MTOUCH_BUTTON_BASECOUNTER_MAX-MTOUCH_BUTTON_BASELINE_HOLD;
        }
    }
    else
    {
        button->state   = MTOUCH_BUTTON_STATE_pressed;
        button->counter = (mtouch_button_statecounter_t)0;
    }
}

/*
 * =======================================================================
 * Button State and Deviation
 * =======================================================================
 */

mtouch_button_threshold_t MTOUCH_Button_Threshold_Get(enum mtouch_button_names name)
{
    if(name < MTOUCH_BUTTONS)
        return mtouch_button[name].threshold;
    else
        return (mtouch_button_threshold_t)0;
}

void MTOUCH_Button_Threshold_Set(enum mtouch_button_names name,mtouch_button_threshold_t threshold)
{
    if(name < MTOUCH_BUTTONS)
    {
        if(threshold >= MTOUCH_BUTTON_THRESHOLD_MIN && threshold <= MTOUCH_BUTTON_THRESHOLD_MAX)
           mtouch_button[name].threshold = threshold;
    }
}

mtouch_button_scaling_t MTOUCH_Button_Scaling_Get(enum mtouch_button_names name)
{
    if(name < MTOUCH_BUTTONS)
        return mtouch_button[name].scaling;
    else
        return (mtouch_button_scaling_t)0;
}

void MTOUCH_Button_Scaling_Set(enum mtouch_button_names name,mtouch_button_scaling_t scaling)
{
    if(name < MTOUCH_BUTTONS)
    {
        if(scaling <= MTOUCH_BUTTON_SCALING_MAX)
            mtouch_button[name].scaling = scaling;
    }
}

uint8_t MTOUCH_Button_Oversampling_Get(enum mtouch_button_names name)
{
    if(name < MTOUCH_BUTTONS)
        return (uint8_t)MTOUCH_Sensor_Oversampling_Get(mtouch_button[name].sensor);
    else
        return (uint8_t)0;
}

void MTOUCH_Button_Oversampling_Set(enum mtouch_button_names name,uint8_t oversampling)
{
    if(name < MTOUCH_BUTTONS)
    {
        MTOUCH_Sensor_Oversampling_Set(mtouch_button[name].sensor, oversampling);
    }
}

bool MTOUCH_Button_isPressed(enum mtouch_button_names name)
{
    if(name < MTOUCH_BUTTONS)
        return (bool)((mtouch_button[name].state == MTOUCH_BUTTON_STATE_pressed || mtouch_button[name].state == MTOUCH_BUTTON_STATE_releaseDebounce) ? true : false);
    else
        return false;
}

bool MTOUCH_Button_isInitialized(enum mtouch_button_names name)
{
    if(name < MTOUCH_BUTTONS)
        return (bool)((mtouch_button[name].state == MTOUCH_BUTTON_STATE_initializing) ? false : true);
    else
        return false;
}


mtouch_button_deviation_t MTOUCH_Button_Deviation_Get(enum mtouch_button_names name) /* Global */
{
    if(name < MTOUCH_BUTTONS)
        return mtouch_button[name].deviation;
    else
        return (mtouch_button_deviation_t)0;
}

static void Button_Deviation_Update(mtouch_button_t* button)
{
    int32_t deviation;
    
    deviation = (int32_t)((int32_t)(button->reading) - (int32_t)((button->baseline)>>MTOUCH_BUTTON_BASELINE_GAIN));

    /* Delta scaling */
    deviation = deviation >> (button->scaling); /* XC8 compiler will extend sign bit of signed values */

    /* Bounds checking */
    if (deviation > MTOUCH_BUTTON_DEVIATION_MAX)
    {
        deviation = MTOUCH_BUTTON_DEVIATION_MAX;
    }
    if (deviation < MTOUCH_BUTTON_DEVIATION_MIN)
    {
        deviation = MTOUCH_BUTTON_DEVIATION_MIN;
    }

    button->deviation = (mtouch_button_deviation_t)deviation;
}

mtouch_buttonmask_t MTOUCH_Button_Buttonmask_Get(void)
{
    mtouch_buttonmask_t output = 0;

    for (uint8_t i = 0; i < MTOUCH_BUTTONS; i++)
    {
        if (MTOUCH_Button_isPressed(i) == true)
        {
            output |= (mtouch_buttonmask_t)0x01 << i;
        }
    }
    return output;
}
 
/*
 * =======================================================================
 * Button Reading and Baseline
 * =======================================================================
 */
mtouch_button_reading_t MTOUCH_Button_Reading_Get(enum mtouch_button_names name) /* Global */
{
    if(name < MTOUCH_BUTTONS)
        return mtouch_button[name].reading;
    else
        return (mtouch_button_reading_t)0;
}

static void Button_Reading_Update(mtouch_button_t* button)
{
    if (GIE == (uint8_t) 1)
    {
        GIE = (uint8_t) 0;
        Button_Reading_Update_Helper(button);
        GIE = (uint8_t) 1;
    }
    else
    {
        Button_Reading_Update_Helper(button);
    }
}

static void Button_Reading_Update_Helper(mtouch_button_t* button)
{    
    if (button->state != MTOUCH_BUTTON_STATE_initializing)
    {
        button->reading -= button->reading>>MTOUCH_BUTTON_READING_GAIN;
        button->reading += MTOUCH_Sensor_RawSample_Get(button->sensor) >> MTOUCH_BUTTON_READING_GAIN;
    }
    else
    {
        button->reading = MTOUCH_Sensor_RawSample_Get(button->sensor);
    }
}

static void Button_Baseline_Initialize(mtouch_button_t* button)
{
    button->baseline   = MTOUCH_Sensor_RawSample_Get(button->sensor);
    button->baseline <<= MTOUCH_BUTTON_BASELINE_GAIN;
}

static void Button_Baseline_Update(mtouch_button_t* button)
{
    if (GIE == (uint8_t)1)
    {
        GIE = (uint8_t)0;
        button->baseline -= (button->baseline) >> MTOUCH_BUTTON_BASELINE_GAIN;
        button->baseline += button->reading;
        GIE = (uint8_t)1;
    }
    else
    {
        button->baseline -= (button->baseline) >> MTOUCH_BUTTON_BASELINE_GAIN;
        button->baseline += button->reading;
    }
}

mtouch_button_reading_t MTOUCH_Button_Baseline_Get(enum mtouch_button_names name) /* Global */
{
    if(name < MTOUCH_BUTTONS)
        return Button_Baseline_Get_helper(name);
    else
        return (mtouch_button_reading_t)0;
}

static mtouch_button_reading_t Button_Baseline_Get_helper(enum mtouch_button_names name)
{
    return (mtouch_button_reading_t)((mtouch_button[name].baseline) >> MTOUCH_BUTTON_BASELINE_GAIN);
}

uint8_t MTOUCH_Button_State_Get(enum mtouch_button_names name)
{
    if(name < MTOUCH_BUTTONS)
        return (uint8_t)mtouch_button[name].state;
    else
        return 0;
}





