/* 
 * File:   i2c_BitBang.h
 * Author: husak
 *
 * Created on 20. b?ezna 2018, 22:54
 */

#ifndef I2C_BITBANG_H
#define	I2C_BITBANG_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "stdbool.h"
#include "stdint.h"

    
// Define i2c pins
#define SDA			LATAbits.LATA5  		// 
#define SCL			LATAbits.LATA4			// 
#define SDA_GET		PORTAbits.RA5           // 
#define SCL_GET		PORTAbits.RA4			// 
#define SDA_DIR		TRISAbits.TRISA5		// 
#define SCL_DIR		TRISAbits.TRISA4		// 

// Define i2c speed
#define I2C_SPEED	 100				// kbps 
#define HalfBitDelay 500/I2C_SPEED	// usec

void i2c_bb_init(void);
bool i2c_bb_tx_rx(uint8_t addr, const uint8_t *tx_dat, uint8_t tx_len, uint8_t *rx_dest, uint8_t rx_len);
uint8_t i2c_bb_rx(uint8_t addr,uint8_t *dest,uint8_t len);
uint8_t i2c_bb_tx(uint8_t,const uint8_t *dat,uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_BITBANG_H */

