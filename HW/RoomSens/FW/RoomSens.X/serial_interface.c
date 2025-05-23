
#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>
#include <string.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/pin_manager.h"
#include "device_memory.h"
#include "app.h"
#include "digital_io.h"
#include "drivers/piezo.h"
#include "drivers/tick.h"
#include "drivers/crc.h"
#include "drivers/dac_pwm.h"

#define RX_BUFFER_SIZE          256

static uint8_t rxSerialBuffer[RX_BUFFER_SIZE];
static uint16_t rxSerialBufferIndex;
typedef enum {RESPONSE_NONE, RESPONSE_DATA, RESPONSE_CONFIG, RESPONSE_STATUS, RESPONSE_BADCRC} responseType;

//static json_t mem[16];
static uint8_t tx_buffer[150];
static char myIdString[DEVICE_MEMORY_MAX_NAME_LENGTH+4];

void serial_interface_parse_msg();
void parse_msg_data(char *sub_token);
void parse_msg_config(char *sub_token);

void send_response_data();
void send_response_status();
void send_response_config();
void send_response_badCrc();


void serialInterface_init()
{
    rxSerialBufferIndex = 0;
    memset(rxSerialBuffer, 0, RX_BUFFER_SIZE);
    
    sprintf(myIdString, "id:%s,", eeprom_memory.device_name);
}

void serialInterface_process()
{
    bool delimiterFound = false;
    
    // read data from UART
    while(UART1_is_rx_ready() && rxSerialBufferIndex < RX_BUFFER_SIZE){
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
        
        //printf("ahoj bla {askd asjd a sd adk jf?a d ads? ja?sd ?as d?a sd?jf?asdkljf?akjd fad?sad a?dks}");
        
        
        // clear buffer
        rxSerialBufferIndex = 0;
        memset(rxSerialBuffer, 0, RX_BUFFER_SIZE);
        
    }
    
}

// {"dst":"kuchyn", "msg":"data", "do":3, "lcd1":"zvonek"}
// {"id":"123456", "msg":"config"}
// {"dst":"all", "msg":"config"}
void serial_interface_parse_msg()
{
   if(rxSerialBufferIndex < 9)
        return;
   
    // check if is for me
    bool is_for_me = false;
    responseType resp = RESPONSE_NONE; 
    
    // chceck ID 
    if(strncmp(rxSerialBuffer, myIdString, strlen(myIdString)) == 0 )
    {
        is_for_me = true;
        LED_WORK_SetLow();
    }
    else if(strncmp(rxSerialBuffer, "id:all,", 7) == 0)
    {
        is_for_me = true;
        LED_WORK_SetLow();
        // insert random delay prevent all devices reansweren in same time
        uint8_t delay = ((tick_getTick() & 7) + (meas_values.power & 7));
        for(int i = 0; i < delay; i++){
            __delay_ms(1);
        }  
    }
    else {
        return;
    }
    
    uint8_t crc = crc8( 0, rxSerialBuffer, rxSerialBufferIndex - 9); //",crc:AB\r\n"
    uint8_t received_crc = 0;
    int found =  sscanf(&rxSerialBuffer[rxSerialBufferIndex - 9], ",crc:%02x\r\n", &received_crc);
    
    if(!found || (crc != received_crc))
    {
        send_response_badCrc();
        return;
    }
    
    char * token = strtok(rxSerialBuffer, ", \r\n\t");
    
    while( token != NULL ) {
        //printf( " %s\n", token ); //printing each token
//        
//        if(strncmp("id:", token, 3) == 0){
//            if(strcmp(&token[3], eeprom_memory.device_name) == 0 ) {
//                is_for_me = true;
//                LED_WORK_SetLow();
//                if(resp == RESPONSE_BADCRC) break;
//            }
//            else if(strcmp(&token[3], "all") == 0) {
//                is_for_me = true; 
//                if(resp == RESPONSE_BADCRC) break;
//                
//                uint8_t delay = ((tick_getTick() & 7) + (meas_values.power & 7));
//                    for(int i = 0; i < delay; i++){
//                    __delay_ms(1);
//                }       
//                LED_WORK_SetLow();
//                
//            }
//            else{
//                break;
//            }
//        }
        if( is_for_me && strncmp("msg:data", token, 8) == 0){
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
        else if(is_for_me && strncmp("reqT:", token, 5) == 0){
            int16_t reqTemp = atoi(&token[5]);
            control_struct.req_temp = reqTemp;
        }
        else if(is_for_me && strncmp("reqL:", token, 5) == 0){
           
        }
        else if(is_for_me && strncmp("beep:", token, 5) == 0){
            uint8_t beep_int = atoi(&token[5]);
            piezo_beep_start(beep_int);
        }
        else if(is_for_me && strncmp("light:", token, 6) == 0){
            uint8_t light_ctrl = atoi(&token[6]);
            if(light_ctrl){
                LED_AMBIENT_SetHigh();
            }
            else{
                LED_AMBIENT_SetLow();
            }
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
    
    LED_WORK_SetHigh();
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
    
    
   
}

void send_response_data() 
{
    uint16_t actual_len = 0;
    
    actual_len += sprintf(&(tx_buffer[actual_len]), "id:%s,di:%d,btn:%d,t:%d,t2:%d", 
            eeprom_memory.device_name, digital_io_get_inputs(), digital_io_get_buttons(), meas_values.temperature, meas_values.temperature2);
    
    if(meas_values.temperature_ext_0 != INT16_MIN)
    {
        actual_len += sprintf(&(tx_buffer[actual_len]), ",t3:%d",
            meas_values.temperature_ext_0);
    }
    
    if(meas_values.temperature_ext_1 != INT16_MIN)
    {
        actual_len += sprintf(&(tx_buffer[actual_len]), ",t4:%d",
            meas_values.temperature_ext_1);
    }
    
    actual_len += sprintf(&(tx_buffer[actual_len]), ",rh:%d,mo:%d,voc:%d,lux:%ld,nl:%d",
            meas_values.humidity, meas_values.motion, meas_values.voc, meas_values.lux, meas_values.noise);
    
    actual_len += sprintf(&(tx_buffer[actual_len]), ",adc0:%d.%02d,adc1:%d.%02d", 
            meas_values.adc0 / 1000, (meas_values.adc0/10) % 100, meas_values.adc1 / 1000, (meas_values.adc1/10) % 100);
    
    if(control_struct.req_temp_trigger) {
        actual_len += sprintf(&(tx_buffer[actual_len]), ",newReqT:%d",control_struct.new_req_temp);
        control_struct.req_temp_trigger = false;
    }
    
    uint8_t crc = crc8( 0, tx_buffer, actual_len);
    actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);
    
    printf("%s", tx_buffer);
    
    digital_io_clear_buttons();
}

void send_response_config() 
{
    uint16_t actual_len = 0;
        
    actual_len += sprintf(tx_buffer, "id:%s", 
            eeprom_memory.device_name);
    
    uint8_t crc = crc8( 0, tx_buffer, actual_len);
    actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);
    
    // load new id to string for parsing
    sprintf(myIdString, "id:%s,", eeprom_memory.device_name);
    
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

void send_response_badCrc()
{
    int actual_len = 0;
    
    actual_len += sprintf(tx_buffer, "id:%s,msg:badCrc", 
        eeprom_memory.device_name);

    uint8_t crc = crc8( 0, tx_buffer, actual_len);
    actual_len += sprintf(&(tx_buffer[actual_len]), ",crc:%02x\r\n", crc);

    printf("%s", tx_buffer); 
}
