#include "i2c_BitBang.h"
#include "fram_MB85RC.h"

static uint8_t tx_buffer[10]; // 2B adresy + 8B dat
static uint8_t rx_buffer[8];

#define FRAM_MB85_I2C_ADDR  0x50 

void fram_MB85_init(void)
{
    
}

int fram_MB85_write64(uint8_t index, uint64_t value)
{
    uint16_t address = (index * 8) + 8; // 64b value, skip first item due I2C safety...
    tx_buffer[0] = address & 0xFF;

    // 
    for (int i = 0; i < 8; i++) {
        tx_buffer[1 + i] = (uint8_t)((value >> (56 - i * 8))& 0xFF);
    }

    return i2c_bb_tx_rx(FRAM_MB85_I2C_ADDR, tx_buffer, 9, rx_buffer, 0);
}

int fram_MB85_read64(uint8_t index, uint64_t *value)
{
    uint16_t address = (index * 8) + 8; // 64b value, skip first item due I2C safety...
    tx_buffer[0] = address & 0xFF;

    int ret = i2c_bb_tx_rx(FRAM_MB85_I2C_ADDR, tx_buffer, 1, rx_buffer, 8);

    if (ret) {
        *value = 0;
        for (int i = 0; i < 8; i++) {
            *value = (*value << 8) | rx_buffer[i];
        }
    }
    return ret;
}
