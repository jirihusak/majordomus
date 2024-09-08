/* 
 * File:   digital_io.h
 * Author: husak
 *
 * Created on 14. ledna 2020, 21:50
 */

#ifndef DIGITAL_IO_H
#define	DIGITAL_IO_H

#include <stdint.h>
#include <stdbool.h>
#include <pic18.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
void digital_io_init();
void digital_io_task();
void digital_io_set_outputs(uint8_t data);
uint8_t digital_io_get_inputs();
uint8_t digital_io_get_buttons();
void digital_io_clear_buttons();
    

#ifdef	__cplusplus
}
#endif

#endif	/* DIGITAL_IO_H */

