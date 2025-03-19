
#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>

#include "temp_ds18B20.h"
#include "../app.h"
#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/pin_manager.h"
#include "one_wire.h"
#include "tick.h"

#define READ_DATA_COUNT     9

static uint8_t read_buffer[READ_DATA_COUNT];
t_one_wire_port oneWirePorts[ONE_WIRE_MAX_PORTS];

static uint8_t crc8( uint8_t *addr, uint8_t len);

static void owPort0Out() {ONE_WIRE_0_SetLow(); ONE_WIRE_0_SetDigitalOutput();}
static void owPort0In() {ONE_WIRE_0_SetDigitalInput();}
static uint8_t owPort0Port() { return ONE_WIRE_0_GetValue();}

static void owPort1Out() {ONE_WIRE_1_SetLow(); ONE_WIRE_1_SetDigitalOutput();}
static void owPort1In() {ONE_WIRE_1_SetDigitalInput();}
static uint8_t owPort1Port() { return ONE_WIRE_1_GetValue();}


void temp_DS18B20_Init()
{
    oneWirePorts[0].owOut = owPort0Out;
    oneWirePorts[0].owIn = owPort0In;
    oneWirePorts[0].owPin = owPort0Port;
    
    oneWirePorts[1].owOut = owPort1Out;
    oneWirePorts[1].owIn = owPort1In;
    oneWirePorts[1].owPin = owPort1Port;
 

}

void temp_DS18B20_Task()
{
    static enum {
        FSM_CONVERT,
        FSM_WAIT,
        FSM_READ,   
        //FSM_WAIT_2,
    } state = FSM_CONVERT;
    
    static uint32_t time = 0;
    static int actual_temp_index = 0;
    int16_t temp;
    
    switch(state)
    {
        case FSM_CONVERT:
            
            ow_reset(actual_temp_index);
            ow_write_byte(actual_temp_index,0xCC); // Issue command SKIP_ROM
            ow_write_byte(actual_temp_index,0x44); // Issue command CONVERT_T
            
            time = tick_getTick();
            state = FSM_WAIT;
            break;
            
        case FSM_WAIT:
            if((tick_getTick() - time) > (10*TICK_MILISECOND))
            {
                state = FSM_READ;
            }
            break;
            
        case FSM_READ:
            
            ow_reset(actual_temp_index);
            ow_write_byte(actual_temp_index,0xCC); // Issue command SKIP_ROM
            ow_write_byte(actual_temp_index,0xBE); // Issue command READ_SCRATCHPAD
            
            for (int i = 0; i < READ_DATA_COUNT; i++)
            {
                read_buffer[i] = ow_read_byte(actual_temp_index);
                //printf(" %x ", read_buffer[i]);
            }
            
            // check data
            uint8_t crc = crc8( read_buffer, 8);
            
            if(crc == read_buffer[8])
            {
                // read temp
                temp = read_buffer[1];
                temp = temp << 8;
                temp |= read_buffer[0];
    
                oneWirePorts[actual_temp_index].temperature = (temp * 10) / 16;
            }
            else
            {
                oneWirePorts[actual_temp_index].temperature = INT16_MIN;
            }
            
            //printf("crc: %x \r\n", crc);

            //next port
            actual_temp_index++;
            if(actual_temp_index >= ONE_WIRE_MAX_PORTS){
                actual_temp_index = 0;
            }
            
            time = tick_getTick();
            state = FSM_CONVERT;
            break;
    }
}

int16_t temp_DS18B20_Read(uint8_t index)
{
    if(index < ONE_WIRE_MAX_PORTS)
        return oneWirePorts[index].temperature;
    else
        return INT16_MIN;
}

static uint8_t crc8( uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;
	
	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}