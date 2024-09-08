

#include "../mcc_generated_files/mcc.h"
#include "string.h"
#include "i2c_BitBang.h"
#include "light_VEML7700.h"

#define I2CSLAVEADDR_VEML7700		0x10	

#define VEML7700_REGISTER_CONF		0x00
#define VEML7700_REGISTER_ALS		0x04	

uint8_t         txBuffer[3];
uint8_t         rxBuffer[20];


int light_VEML7700_init()
{
    txBuffer[0] = VEML7700_REGISTER_CONF;	
    txBuffer[1] = 0x00;
    txBuffer[2] = 0x00;
    int retVal = i2c_bb_tx(I2CSLAVEADDR_VEML7700, txBuffer, 3);
}

int light_VEML7700_meas(int32_t *lux)
{
    uint16_t alsRaw = 0;
    txBuffer[0] = VEML7700_REGISTER_ALS;	

    //i2c1_writeNBytes(I2CSLAVEADDR_HDC1080, txBuffer, 3);
    int retVal = i2c_bb_tx_rx(I2CSLAVEADDR_VEML7700, txBuffer, 1, rxBuffer, 2);
    
    alsRaw = ((((uint16_t)rxBuffer[1]) << 8) & 0xff00) + (((uint16_t)rxBuffer[0]) & 0x00ff);
    
    if(retVal) {
        *lux = alsRaw;
        return 1;
    }
    
    *lux = INT16_MIN;
    return 0;
}
