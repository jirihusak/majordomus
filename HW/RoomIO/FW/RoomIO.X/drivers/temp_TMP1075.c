
#include <stdio.h>
#include "i2c_BitBang.h"

#define TMP1075_I2C_ADDR    0x48

static uint8_t tx_buffer[3];
static uint8_t rx_buffer[2];

void temp_TMP1075_init()
{
    tx_buffer[0] = 0x01; // config register
    tx_buffer[1] = 0x0A; // defualt for tmp1075N
    tx_buffer[2] = 0x60; // continuous mode, 220ms period, alert/interrupt not connected
    i2c_bb_tx_rx(TMP1075_I2C_ADDR,tx_buffer,3,rx_buffer,0);
}
    
int32_t temp_TMP1075_read_temperature()
{
    tx_buffer[0] = 0; // read register
    int retVal = i2c_bb_tx_rx(TMP1075_I2C_ADDR,tx_buffer,1,rx_buffer,2);
    
    if(retVal)
    {
        int16_t raw_data = ((int16_t)rx_buffer[0] << 8) + (rx_buffer[1]);
        
        int32_t decadic = ((int32_t)raw_data * 100) >> 8;
        
        printf("temp raw %x %x %d %d\r\n", rx_buffer[0], rx_buffer[1], raw_data, (int16_t)decadic);
        
        return decadic;
    }
    
    return 0;
}
