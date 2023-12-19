
#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>

#include "i2c_BitBang.h"
#include "../mcc_generated_files/mcc.h"

void i2c_bb_hc(void);
void i2c_bb_start(void);
void i2c_bb_stop(void);
uint8_t i2c_bb_tx_byte(uint8_t val);
uint8_t i2c_bb_rx_byte(uint8_t ack);

//setup bit bang I2C pins
void i2c_bb_init(void){
  //set pins as inputs
  SDA_DIR = 1;
  SCL_DIR = 1;
  //set output to low
  SDA = 0;
  SCL = 0;
}

bool i2c_bb_tx_rx(uint8_t addr, const uint8_t *tx_dat, uint8_t tx_len, uint8_t *rx_dest, uint8_t rx_len)
{
    uint8_t ack;
    int i;
    if(tx_len < 1 || tx_len > 127)
        return false;

    //send start
    i2c_bb_start();
    //send address with W bit
    ack=i2c_bb_tx_byte((addr<<1));
    //send data bytes
    for(i=0;i<tx_len && ack;i++){
        //transmit next byte
        ack=i2c_bb_tx_byte(tx_dat[i]);
    }

    if(rx_len < 1 || rx_len > 127) {
        //transmit stop
        i2c_bb_stop();
        return false;
    }

    SCL_DIR = 1;
    i2c_bb_hc();

    //send start
    i2c_bb_start();
    //send address with R bit
    if(!i2c_bb_tx_byte((addr<<1)|0x01)){
        //got NACK return error
        i2c_bb_stop();
        return false;
    }
    //send data bytes
    for(i=0; i<rx_len; i++){
        //transmit next byte
        rx_dest[i]=i2c_bb_rx_byte( i != (rx_len-1));
    }
    //transmit stop
    i2c_bb_stop();

    //return if slave NACKed
    return ack;
}

//wait for 1/2 of I2C clock period
void i2c_bb_hc(void){
  //wait for 0.05ms
  __delay_us(1);
//    for(int i=0; i < 100; i++){
//        NOP();
//    }
}

void i2c_bb_start(void){
  //wait for 1/2 clock first
  i2c_bb_hc();
  //pull SDA low
  SDA_DIR = 0;
  //wait for 1/2 clock for end of start
  i2c_bb_hc();
}

void i2c_bb_stop(void){
  //pull SDA low
  SDA_DIR = 0;
  //wait for 1/2 clock for end of start
  i2c_bb_hc();
  //float SCL
  SCL_DIR = 1;
  //wait for 1/2 clock
  i2c_bb_hc();
  //float SDA
  SDA_DIR = 1;
  //wait for 1/2 clock
  i2c_bb_hc();

}

//send value over I2C return 1 if slave ACKed
uint8_t i2c_bb_tx_byte(uint8_t val){
  int i;
  //shift out bits
  for(i=0;i<8;i++){      
    //pull SCL low
    SCL_DIR = 0;   
    //check bit
    if(val&0x80){
      //float SDA
      SDA_DIR = 1;
    }else{
      //pull SDA low
      SDA_DIR = 0;
      
    }
    //shift
    val<<=1;
    //wait for 1/2 clock
    i2c_bb_hc();
    //float SCL
    SCL_DIR = 1;
    //wait for 1/2 clock
    i2c_bb_hc();
  }
  //check ack bit
  //pull SCL low
  SCL_DIR = 0;
  //float SDA
  SDA_DIR = 1;
  //wait for 1/2 clock
  i2c_bb_hc();
  //float SCL
  SCL_DIR = 1;
  //wait for 1/2 clock
  i2c_bb_hc();
  //sample SDA
  val=SDA_GET;
  //pull SCL low
  SCL_DIR = 0;
  //return sampled value
  return !val;
}
//send value over I2C return 1 if slave ACKed
uint8_t i2c_bb_rx_byte(uint8_t ack){
  unsigned char val = 0;
  int i;
  //shift out bits
  SDA_DIR = 1;
  for(i=0;i<8;i++){      
    //pull SCL low
    SCL_DIR = 0;
    //wait for 1/2 clock
    i2c_bb_hc();
    //float SCL
    SCL_DIR = 1;
    //wait for 1/2 clock
    i2c_bb_hc();
    //shift value to make room
    val<<=1;
    //sample data
    if(SDA_GET){
      val|=1;
    }
  }
  //check ack bit
  //pull SCL low
  SCL_DIR = 0;
  //check if we are ACKing this byte
  if(ack){
    //pull SDA low for ACK
    SDA_DIR = 0;
  }else{
    //float SDA for NACK
    SDA_DIR = 1;
  }
  //wait for 1/2 clock
  i2c_bb_hc();
  //float SCL
  SCL_DIR = 1;
  //wait for 1/2 clock
  i2c_bb_hc();
  //pull SCL low
  SCL_DIR = 0;
  //float SDA
  SDA_DIR = 1;
  //return value
  return val;
}

uint8_t i2c_bb_tx(uint8_t addr,const uint8_t *dat,uint8_t len){
  short ack;
  int i;
  //send start
  i2c_bb_start();
  //send address with W bit
  ack=i2c_bb_tx_byte((addr<<1));
  //send data bytes
  for(i=0;i<len && ack;i++){
    //transmit next byte
    ack=i2c_bb_tx_byte(dat[i]);
  }
  //transmit stop
  i2c_bb_stop();
  //return if slave NACKed
  return ack;
}

uint8_t i2c_bb_rx(uint8_t addr,uint8_t *dest,uint8_t len){
  int i;
  //send start
  i2c_bb_start();
  //send address with R bit
  if(!i2c_bb_tx_byte((addr<<1)|0x01)){
    //got NACK return error
    i2c_bb_stop();
    return 0;
  }
  //send data bytes
  for(i=0;i<len;i++){
    //transmit next byte
    dest[i]=i2c_bb_rx_byte(i != (len-1));
  }
  //transmit stop
  i2c_bb_stop();
  //return if slave NACKed
  return 1;
}