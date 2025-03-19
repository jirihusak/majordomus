
#include "../mcc_generated_files/mcc.h"
#include "tempHum_SHT41.h"
#include "string.h"
#include "i2c_BitBang.h"
#include "tick.h"


#define I2CSLAVEADDR_SHT41		0x44	//


static uint8_t         txBuffer[3];
static uint8_t         rxBuffer[20];

static int32_t avg_temperature;
static uint32_t avg_humidity;
static uint32_t timer;

static enum {
    SHT41_INIT,
    SHT41_INIT_WAIT,
    SHT41_INIT_TRIGGER,
    SHT41_TRIGGER_WAIT,
    SHT41_READ,
    SHT41_IDLE,        
} state = SHT41_INIT;


void tempHum_SHT41_Task()
{
    int retVal;
    
    switch(state)
    {
        case SHT41_INIT:
            txBuffer[0] = 0xFD;	// start
            i2c_bb_tx(I2CSLAVEADDR_SHT41, txBuffer, 1);
            timer = tick_getTick();
            state = SHT41_INIT_WAIT;
            break;
            
        case SHT41_INIT_WAIT:
            if((tick_getTick()-timer) >= 10*TICK_MILISECOND)
            {
                state = SHT41_READ;
            }
            break;
            
        case SHT41_READ:
            retVal = i2c_bb_rx(I2CSLAVEADDR_SHT41, rxBuffer, 6);
            
            if(retVal != 0)
            {
                uint16_t temperatureRaw   = ((((uint16_t)rxBuffer[0]) << 8) & 0xff00) + (((uint16_t)rxBuffer[1]) & 0x00ff);
                uint16_t humidityRaw      = ((((uint16_t)rxBuffer[3]) << 8) & 0xff00) + (((uint16_t)rxBuffer[4]) & 0x00ff);

                float ftemperature = (((float)(((uint32_t)temperatureRaw) * 175))/65536) - 45;
                float fhumidity    = (((float)humidityRaw)*125/65536) - 6;


                avg_temperature = ftemperature * 10.0f;
                avg_humidity = fhumidity * 10.0f;
            }
            else{
                avg_temperature = INT16_MIN;
                avg_humidity = INT16_MIN;
            }
            timer = tick_getTick();
            state = SHT41_IDLE;
            
        case SHT41_IDLE:
            if((tick_getTick()-timer) >= 1000*TICK_MILISECOND)
            {
                state = SHT41_INIT;
            }
        
    }
    
}


int tempHum_SHT41_meas(int32_t *temperature, uint32_t *humidity)
{
    *temperature = avg_temperature;
    *humidity = avg_humidity;
    return 0;
}
