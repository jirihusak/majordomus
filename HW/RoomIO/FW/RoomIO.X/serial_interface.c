
#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>
#include <string.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "device_memory.h"
#include "app.h"
#include "digital_io.h"
#include "drivers/tick.h"
#include "drivers/crc.h"
#include "drivers/dac_pwm.h"

#define RX_BUFFER_SIZE          256

static uint8_t rxSerialBuffer[RX_BUFFER_SIZE];
static uint16_t rxSerialBufferIndex;
typedef enum {RESPONSE_NONE, RESPONSE_DATA, RESPONSE_CONFIG, RESPONSE_STATUS} responseType;

static uint8_t tx_buffer[150];

void serial_interface_parse_msg();
void parse_msg_data(char *sub_token);
void parse_msg_config(char *sub_token);

void send_response_data();
void send_response_status();
void send_response_config();

void serialInterface_init()
{
    rxSerialBufferIndex = 0;
    memset(rxSerialBuffer, 0, RX_BUFFER_SIZE);
}

void serialInterface_process()
{
    bool delimiterFound = false;
    
    // read data from UART
    while(UART1_is_rx_ready() && rxSerialBufferIndex < RX_BUFFER_SIZE){
        LED_WORK_Toggle();
        rxSerialBuffer[rxSerialBufferIndex] = UART1_Read();
        rxSerialBufferIndex++;
        
        if(rxSerialBufferIndex >= 2 && rxSerialBuffer[rxSerialBufferIndex-1] == '\n' && rxSerialBuffer[rxSerialBufferIndex-2] == '\r') {
            delimiterFound = true;
            break;
        }
        
        if(rxSerialBufferIndex >= RX_BUFFER_SIZE){
            rxSerialBufferIndex = 0;
            memset(rxSerialBuffer, 0, RX_BUFFER_SIZE);
            break;
        }
    }
    
    // check if is delimiter \r\n
    if(delimiterFound) {
        serial_interface_parse_msg();      
        
        // clear buffer
        rxSerialBufferIndex = 0;
        memset(rxSerialBuffer, 0, RX_BUFFER_SIZE);   
    }
}

void serial_interface_parse_msg()
{
    if(rxSerialBufferIndex < 9)
        return;
    
    uint8_t crc = crc8( 0, rxSerialBuffer, rxSerialBufferIndex - 9); //",crc:AB\r\n"
    uint8_t received_crc = 0;
    int found =  sscanf(&rxSerialBuffer[rxSerialBufferIndex - 9], ",crc:%02x\r\n", &received_crc);
    
    if(!found || (crc != received_crc)){
        int actual_len = 0;
    
        actual_len += sprintf(tx_buffer, "id:%s,msg:badCrc", 
            eeprom_memory.device_name);
    
        uint8_t crc = crc8( 0, tx_buffer, actual_len);
        actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);
    
        printf("%s", tx_buffer); 
        return;
    }
    
    char * token = strtok(rxSerialBuffer, ", \r\n\t");
    
    // check if is for me
    bool is_for_me = false;
    responseType resp = RESPONSE_NONE; 
    
    while( token != NULL ) {
        //printf( " %s\n", token ); //printing each token
        
        if(strncmp("id:", token, 3) == 0){
            if(strcmp(&token[3], eeprom_memory.device_name) == 0 ) {
                is_for_me = true;
                LED_WORK_SetLow();
            }
            else if(strcmp(&token[3], "all") == 0) {
                is_for_me = true; 
                
                uint8_t delay = ((tick_getTick() & 7) + (meas_values.power & 7));
                    for(int i = 0; i < delay; i++){
                    __delay_ms(1);
                }       
                
                LED_WORK_SetLow();
            }
            else{
                break;
            }
        }
        else if( is_for_me && strncmp("msg:data", token, 8) == 0){
            resp = RESPONSE_DATA;
        }
        else if( is_for_me && strncmp("msg:config", token, 10) == 0){
            resp = RESPONSE_CONFIG;
        }
        else if( is_for_me && strncmp("msg:status", token, 10) == 0){
            resp = RESPONSE_STATUS;
        }
        else if( is_for_me && strncmp("do:", token, 3) == 0){
            digital_io_set_outputs(atoi(&token[3]));
        }
        else if( is_for_me && strncmp("dac0:", token, 5) == 0){
            dac_pwm_set_milivolts_CH0(atof(&token[5]) * 1000);
        }
        else if( is_for_me && strncmp("dac1:", token, 5) == 0){
            dac_pwm_set_milivolts_CH1(atof(&token[5]) * 1000);
        }
        else if( is_for_me && resp == RESPONSE_CONFIG && strncmp("cmd:reset", token, 9) == 0){
            Reset();
        }
        else if( is_for_me && resp == RESPONSE_CONFIG && strncmp("newId:", token, 6) == 0){
            strncpy(eeprom_memory.device_name, &token[6], DEVICE_MEMORY_MAX_NAME_LENGTH);
            device_memory_device_eeprom_save();
        }
        else if( is_for_me && resp == RESPONSE_CONFIG && strncmp("newId5050:", token, 10) == 0){
            if((tick_getTick() & 1)){
                strncpy(eeprom_memory.device_name, &token[10], DEVICE_MEMORY_MAX_NAME_LENGTH);
                device_memory_device_eeprom_save();
            }
        }
        
        token = strtok(NULL, ", \r\n\t");
   }
    
   //send response
    switch(resp){
        case RESPONSE_DATA:
            send_response_data();
            break;
        case RESPONSE_STATUS:
            send_response_status();
            break;
        case RESPONSE_CONFIG:
            send_response_config();
            break;
    }
    
    
   LED_WORK_SetHigh();

}

void send_response_data() 
{
    int actual_len = 0;
    
    actual_len += sprintf(&(tx_buffer[actual_len]), "id:%s,di:%d,btn:%d", 
            eeprom_memory.device_name, digital_io_get_inputs(), digital_io_get_buttons());
    
    if(meas_values.temperature_ext_0 != INT16_MIN){
        actual_len += sprintf(&(tx_buffer[actual_len]), ",t0:%d.%d", 
                meas_values.temperature_ext_0 / 10, abs(meas_values.temperature_ext_0) % 10);
    }
    else
    {
        actual_len += sprintf(&(tx_buffer[actual_len]), ",t0:NaN");
    }
    
    if(meas_values.temperature_ext_1 != INT16_MIN){
        actual_len += sprintf(&(tx_buffer[actual_len]), ",t1:%d.%d", 
               meas_values.temperature_ext_1 / 10, abs(meas_values.temperature_ext_1) % 10);
    }else
    {
        actual_len += sprintf(&(tx_buffer[actual_len]), ",t1:NaN");
    }
    
    actual_len += sprintf(&(tx_buffer[actual_len]), ",adc0:%d.%02d,adc1:%d.%02d", 
            meas_values.adc0 / 1000, (meas_values.adc0/10) % 100, meas_values.adc1 / 1000, (meas_values.adc1/10) % 100);
    
    uint8_t crc = crc8( 0, tx_buffer, actual_len);
    actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);
    
    printf("%s", tx_buffer);
    
    digital_io_clear_buttons();
}

void send_response_config() 
{
    int actual_len = 0;
        
    actual_len += sprintf(tx_buffer, "id:%s", 
            eeprom_memory.device_name);
    
    uint8_t crc = crc8( 0, tx_buffer, actual_len);
    actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);
    
    printf("%s", tx_buffer);
    
}

void send_response_status() 
{
    int actual_len = 0;
    
    actual_len += sprintf(tx_buffer, "id:%s,type:%s,version:%s(%s)", 
            eeprom_memory.device_name, BOARD_TYPE, APP_VERSION, BUILD_DATE);
    
    actual_len += sprintf(&(tx_buffer[actual_len]), ",pwr:%d.%02d,pwrOut:%d.%02d", 
            meas_values.power / 1000, (meas_values.power/10) % 100, meas_values.power_outputs / 1000, (meas_values.power_outputs/10) % 100);
    
    uint8_t crc = crc8( 0, tx_buffer, actual_len);
    actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);
    
    printf("%s", tx_buffer); 
}

