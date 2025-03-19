
#include "mcc_generated_files/mcc.h"
#include "digital_io.h"
#include "drivers/tick.h"

#define DIGITAL_IO_BUTTONS_COUNT        4

typedef struct digital_io_data {
    uint8_t outputs;
    uint8_t inputs;
    uint8_t rising_edge_detect;
    uint32_t buttons_timer[DIGITAL_IO_BUTTONS_COUNT];
    enum {BUTTON_LOW, BUTTON_HIGHT } buttons_state[DIGITAL_IO_BUTTONS_COUNT];
} digital_io_data_t;

static digital_io_data_t digital_io_data;

void digital_io_init()
{
    
}

void digital_io_task()
{
    // set outputs
    if(digital_io_data.outputs & 0x01) DOUT_0_SetHigh();
    else DOUT_0_SetLow();
    
    if(digital_io_data.outputs & 0x02) DOUT_1_SetHigh();
    else DOUT_1_SetLow();
    
    if(digital_io_data.outputs & 0x04) DOUT_2_SetHigh(); 
    else DOUT_2_SetLow();
    
    if(digital_io_data.outputs & 0x08) DOUT_3_SetHigh();
    else DOUT_3_SetLow();
    
//    if(digital_io_data.outputs & 0x10) DOUT_4_SetHigh();
//    else DOUT_4_SetLow();
//    
//    if(digital_io_data.outputs & 0x20) DOUT_5_SetHigh();
//    else DOUT_5_SetLow();
//    
//    if(digital_io_data.outputs & 0x40) DOUT_6_SetHigh(); 
//    else DOUT_6_SetLow();
//    
//    if(digital_io_data.outputs & 0x80) DOUT_7_SetHigh();
//    else DOUT_7_SetLow();
    
    // read inputs
    uint8_t inputs_temp = 0;
    inputs_temp |= ((0x01 & !DIN_0_GetValue()) << 0);
    inputs_temp |= ((0x01 & !DIN_1_GetValue()) << 1);
    inputs_temp |= ((0x01 & !DIN_2_GetValue()) << 2);
    inputs_temp |= ((0x01 & !DIN_3_GetValue()) << 3);
//    inputs_temp |= ((0x01 & !DIN_4_GetValue()) << 4);
//    inputs_temp |= ((0x01 & !DIN_5_GetValue()) << 5);
//    inputs_temp |= ((0x01 & !DIN_6_GetValue()) << 6);
//    inputs_temp |= ((0x01 & !DIN_7_GetValue()) << 7);
    digital_io_data.inputs = inputs_temp;

    // process read inputs as buttons
    for(int i = 0; i < DIGITAL_IO_BUTTONS_COUNT; i++)
    {
        uint8_t input = (digital_io_data.inputs >> i ) & 0x01;
        if(digital_io_data.buttons_state[i] == BUTTON_LOW && input == 1){
            digital_io_data.buttons_state[i] = BUTTON_HIGHT;
            digital_io_data.buttons_timer[i] = tick_getTick(); 
            
            // rising edge
            digital_io_data.rising_edge_detect |= (0x01 << i);
            
        }
        else if(digital_io_data.buttons_state[i] == BUTTON_HIGHT && input == 0 ){
            digital_io_data.buttons_state[i] = BUTTON_LOW;
            digital_io_data.buttons_timer[i] = tick_getTick(); 
        }
    }
    
    
}

void digital_io_set_outputs(uint8_t data)
{
    digital_io_data.outputs = data;
}

uint8_t digital_io_get_inputs()
{
    return digital_io_data.inputs;
}

uint8_t digital_io_get_buttons()
{
    return digital_io_data.rising_edge_detect;
}

void digital_io_clear_buttons()
{
    digital_io_data.rising_edge_detect = 0;
}
