
#include "mcc_generated_files/mcc.h"
#include "digital_io.h"
#include "drivers/tick.h"
#include "mcc_generated_files/tmr1.h"
#include "drivers/fram_MB85RC.h"
#include "mcc_generated_files/spi1.h"

#define DIGITAL_IO_BUTTONS_COUNT        8

typedef struct digital_io_data {
    uint8_t outputs;
    uint8_t inputs;
    uint8_t rising_edge_detect;
    uint32_t buttons_timer[DIGITAL_IO_BUTTONS_COUNT];
    enum {BUTTON_LOW, BUTTON_HIGHT } buttons_state[DIGITAL_IO_BUTTONS_COUNT];
    uint64_t counter[DIGITAL_IO_BUTTONS_COUNT];
    uint16_t last_inputs; // for detecting rising edge
} digital_io_data_t;

static digital_io_data_t digital_io_data;
void digital_io_counter_ISR();


static void ws2815_encode_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t *out)
{
    const uint8_t patterns[2] = {0x8, 0xE};  // 0b1000, 0b1110
    uint8_t colors[3] = { green, red, blue };

    uint8_t out_byte = 0;
    uint8_t bitcount = 0;
    uint8_t out_index = 0;

    for (uint8_t c = 0; c < 3; c++) {
        uint8_t color = colors[c];
        for (int8_t bit = 7; bit >= 0; bit--) {
            uint8_t pattern = patterns[(color >> bit) & 0x01];

            // zapí?eme 4 bity z patternu (od MSB)
            for (int8_t p = 3; p >= 0; p--) {
                out_byte <<= 1;
                out_byte |= (pattern >> p) & 1;
                bitcount++;

                if (bitcount == 8) {
                    out[out_index++] = out_byte;
                    out_byte = 0;
                    bitcount = 0;
                }
            }
        }
    }

    // doplníme poslední bajt, pokud nebyl celý
    if (bitcount > 0) {
        out_byte <<= (8 - bitcount);
        out[out_index++] = out_byte;
    }
}

void digital_io_init()
{
    // load counter from FRAM
    for(uint16_t i = 0; i < 8; i++) {
        uint64_t data = 0;
        fram_MB85_read64(i, &data);
        digital_io_data.counter[i] = data;
    }
    
    TMR1_SetInterruptHandler(&digital_io_counter_ISR);
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
    
    if(digital_io_data.outputs & 0x10) DOUT_4_SetHigh();
    else DOUT_4_SetLow();
    
    if(digital_io_data.outputs & 0x20) DOUT_5_SetHigh();
    else DOUT_5_SetLow();
    
    if(digital_io_data.outputs & 0x40) DOUT_6_SetHigh(); 
    else DOUT_6_SetLow();
    
    if(digital_io_data.outputs & 0x80) DOUT_7_SetHigh();
    else DOUT_7_SetLow();
    
    // read inputs
    uint8_t inputs_temp = 0;
    inputs_temp |= ((0x01 & !DIN_0_GetValue()) << 0);
    inputs_temp |= ((0x01 & !DIN_1_GetValue()) << 1);
    inputs_temp |= ((0x01 & !DIN_2_GetValue()) << 2);
    inputs_temp |= ((0x01 & !DIN_3_GetValue()) << 3);
    inputs_temp |= ((0x01 & !DIN_4_GetValue()) << 4);
    inputs_temp |= ((0x01 & !DIN_5_GetValue()) << 5);
    inputs_temp |= ((0x01 & !DIN_6_GetValue()) << 6);
    inputs_temp |= ((0x01 & !DIN_7_GetValue()) << 7);
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
    
    
    
    
//    // for 20 LEDs
//    uint8_t data[12];
//    ws2815_encode_rgb(0x00, 0xFF, 0xFF, data);
//    for(int i = 0; i < 30; i++)
//    {
//        SPI1_Open(SPI1_DEFAULT);
//        
//        SPI1_ExchangeBlock12(data);
//               
//        SPI1_Close();
//        
//    }
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

void digital_io_counter_set_counter(uint8_t index, uint64_t new_value)
{
    digital_io_data.counter[index] = new_value;
}

uint64_t digital_io_counter_get_counter(uint8_t index)
{
    return digital_io_data.counter[index];
}

void digital_io_counter_save()
{
    // load counter from FRAM
    for(uint16_t i = 0; i < 8; i++) {
        fram_MB85_write64(i, digital_io_data.counter[i]);
    }
}

void digital_io_counter_ISR()
{
    uint8_t inputs_temp = 0;
    inputs_temp |= ((0x01 & !DIN_0_GetValue()) << 0);
    inputs_temp |= ((0x01 & !DIN_1_GetValue()) << 1);
    inputs_temp |= ((0x01 & !DIN_2_GetValue()) << 2);
    inputs_temp |= ((0x01 & !DIN_3_GetValue()) << 3);
    inputs_temp |= ((0x01 & !DIN_4_GetValue()) << 4);
    inputs_temp |= ((0x01 & !DIN_5_GetValue()) << 5);
    inputs_temp |= ((0x01 & !DIN_6_GetValue()) << 6);
    inputs_temp |= ((0x01 & !DIN_7_GetValue()) << 7);
    
    
    if(((inputs_temp >> 0) & 1) && (((digital_io_data.last_inputs >> 0) & 1) == 0 )){
        digital_io_data.counter[0]++;
    }
    if(((inputs_temp >> 1) & 1) && (((digital_io_data.last_inputs >> 1) & 1) == 0 )){
        digital_io_data.counter[1]++;
    }
    if(((inputs_temp >> 2) & 1) && (((digital_io_data.last_inputs >> 2) & 1) == 0 )){
        digital_io_data.counter[2]++;
    }
    if(((inputs_temp >> 3) & 1) && (((digital_io_data.last_inputs >> 3) & 1) == 0 )){
        digital_io_data.counter[3]++;
    }
    if(((inputs_temp >> 4) & 1) && (((digital_io_data.last_inputs >> 4) & 1) == 0 )){
        digital_io_data.counter[4]++;
    }
    if(((inputs_temp >> 5) & 1) && (((digital_io_data.last_inputs >> 5) & 1) == 0 )){
        digital_io_data.counter[5]++;
    }
    if(((inputs_temp >> 6) & 1) && (((digital_io_data.last_inputs >> 6) & 1) == 0 )){
        digital_io_data.counter[6]++;
    }
    if(((inputs_temp >> 7) & 1) && (((digital_io_data.last_inputs >> 7) & 1) == 0 )){
        digital_io_data.counter[7]++;
    }
    
    digital_io_data.last_inputs = inputs_temp;
}