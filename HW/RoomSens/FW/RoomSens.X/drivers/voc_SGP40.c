
#include "voc_SGP40.h"
#include "i2c_BitBang.h"
#include "stdio.h"
#include "tick.h"
#include "../app.h"

#define I2CSLAVEADDR_SGP40          0x59    	

#define SGP40_FEATURESET            0x0020    ///< The required set for this library
#define SGP40_CRC8_POLYNOMIAL       0x31        ///< Seed for SGP40's CRC polynomial
#define SGP40_CRC8_INIT             0xFF       ///< Init value for CRC
#define SGP40_WORD_LEN              2           ///< 2 bytes per word

uint8_t         txBuffer[20];
uint8_t         rxBuffer[20];

static enum {
    SGP40_SELF_TEST_START,
    SGP40_SELF_TEST_WAIT,
    SGP40_MEAS,
    SGP40_WAIT,
    SGP40_READ,
    SGP40_IDLE,        
} state = SGP40_SELF_TEST_START;
static uint32_t timer;
static int16_t voc_index;

uint8_t generateCRC(uint8_t *data, uint8_t datalen);

void voc_SGP40_Init()
{
    VocAlgorithm_init(&voc_algorithm_params);
}

void voc_SGP40_Task()
{
    static int retVal;
    static int32_t raw;
    
    switch(state)
    {
        case SGP40_SELF_TEST_START:
            // send raw meas cmd
            txBuffer[0] = 0x28;
            txBuffer[1] = 0x0E;
            retVal = i2c_bb_tx(I2CSLAVEADDR_SGP40, txBuffer, 2);
            timer = tick_getTick();
            state = SGP40_SELF_TEST_WAIT;
            
            break;
            
        case SGP40_SELF_TEST_WAIT:
            if((tick_getTick()-timer) >= 320*TICK_MILISECOND)
            {
                rxBuffer[0] = 0,rxBuffer[1] = 0;
                retVal = i2c_bb_rx(I2CSLAVEADDR_SGP40, rxBuffer, 2);
                //printf("SGP40 test : %x %x ", rxBuffer[0], rxBuffer[1]);
                state = SGP40_MEAS;
            }
            break;
            
        case SGP40_MEAS:
            // send raw meas cmd
            txBuffer[0] = 0x26;
            txBuffer[1] = 0x0F;

            uint16_t rhticks = (uint32_t)(((meas_values.humidity/10) * 65535) / 100 + 0.5);
            txBuffer[2] = rhticks >> 8;
            txBuffer[3] = rhticks & 0xFF;
            txBuffer[4] = generateCRC(txBuffer + 2, 2);
            uint16_t tempticks = (int32_t)((((meas_values.temperature/10) + 45) * 65535) / 175);
            txBuffer[5] = tempticks >> 8;
            txBuffer[6] = tempticks & 0xFF;
            txBuffer[7] = generateCRC(txBuffer + 5, 2);
            
            retVal = i2c_bb_tx(I2CSLAVEADDR_SGP40, txBuffer, 8);
            state = SGP40_WAIT;
            timer = tick_getTick();
            break;
            
        case SGP40_WAIT:
            if((tick_getTick()-timer) >= 40*TICK_MILISECOND)
            {
                state = SGP40_READ;
            }
            break;
            
        case SGP40_READ:
            // read data
            rxBuffer[0] = 0,rxBuffer[1] = 0;
            retVal = i2c_bb_rx(I2CSLAVEADDR_SGP40, rxBuffer, 2);
            raw = (rxBuffer[0] << 8) + rxBuffer[1];
            
            if(retVal){
                VocAlgorithm_process(&voc_algorithm_params, raw, &voc_index);
            }
            else {
                voc_index = INT16_MIN;
            }
                        
            //printf("SGP40: %d ", raw);
            //printf("SGP40: %d\r\n", voc_index);
            
            timer = tick_getTick();
            state = SGP40_IDLE;
            break;
            
        case SGP40_IDLE:
            if((tick_getTick()-timer) >= 1300*TICK_MILISECOND)
            {
                state = SGP40_SELF_TEST_START;
            }
            
            break;
        
    }
    
    
}

int16_t voc_SGP40_get_air_quality()
{
    return voc_index;
}

uint8_t generateCRC(uint8_t *data, uint8_t datalen) {
  // calculates 8-Bit checksum with given polynomial
  uint8_t crc = SGP40_CRC8_INIT;

  for (uint8_t i = 0; i < datalen; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      if (crc & 0x80)
        crc = (crc << 1) ^ (uint8_t)SGP40_CRC8_POLYNOMIAL;
      else
        crc <<= 1;
    }
  }
  return crc;
}