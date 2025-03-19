/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC18LF57K42
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.30 and above
        MPLAB 	          :  MPLAB X 5.40	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set ONE_WIRE_0 aliases
#define ONE_WIRE_0_TRIS                 TRISBbits.TRISB4
#define ONE_WIRE_0_LAT                  LATBbits.LATB4
#define ONE_WIRE_0_PORT                 PORTBbits.RB4
#define ONE_WIRE_0_WPU                  WPUBbits.WPUB4
#define ONE_WIRE_0_OD                   ODCONBbits.ODCB4
#define ONE_WIRE_0_ANS                  ANSELBbits.ANSELB4
#define ONE_WIRE_0_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define ONE_WIRE_0_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define ONE_WIRE_0_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define ONE_WIRE_0_GetValue()           PORTBbits.RB4
#define ONE_WIRE_0_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define ONE_WIRE_0_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define ONE_WIRE_0_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define ONE_WIRE_0_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define ONE_WIRE_0_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define ONE_WIRE_0_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define ONE_WIRE_0_SetAnalogMode()      do { ANSELBbits.ANSELB4 = 1; } while(0)
#define ONE_WIRE_0_SetDigitalMode()     do { ANSELBbits.ANSELB4 = 0; } while(0)

// get/set LED_WORK aliases
#define LED_WORK_TRIS                 TRISBbits.TRISB5
#define LED_WORK_LAT                  LATBbits.LATB5
#define LED_WORK_PORT                 PORTBbits.RB5
#define LED_WORK_WPU                  WPUBbits.WPUB5
#define LED_WORK_OD                   ODCONBbits.ODCB5
#define LED_WORK_ANS                  ANSELBbits.ANSELB5
#define LED_WORK_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define LED_WORK_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define LED_WORK_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define LED_WORK_GetValue()           PORTBbits.RB5
#define LED_WORK_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define LED_WORK_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define LED_WORK_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define LED_WORK_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define LED_WORK_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define LED_WORK_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define LED_WORK_SetAnalogMode()      do { ANSELBbits.ANSELB5 = 1; } while(0)
#define LED_WORK_SetDigitalMode()     do { ANSELBbits.ANSELB5 = 0; } while(0)

// get/set DOUT_2 aliases
#define DOUT_2_TRIS                 TRISCbits.TRISC0
#define DOUT_2_LAT                  LATCbits.LATC0
#define DOUT_2_PORT                 PORTCbits.RC0
#define DOUT_2_WPU                  WPUCbits.WPUC0
#define DOUT_2_OD                   ODCONCbits.ODCC0
#define DOUT_2_ANS                  ANSELCbits.ANSELC0
#define DOUT_2_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define DOUT_2_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define DOUT_2_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define DOUT_2_GetValue()           PORTCbits.RC0
#define DOUT_2_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define DOUT_2_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define DOUT_2_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define DOUT_2_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define DOUT_2_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define DOUT_2_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define DOUT_2_SetAnalogMode()      do { ANSELCbits.ANSELC0 = 1; } while(0)
#define DOUT_2_SetDigitalMode()     do { ANSELCbits.ANSELC0 = 0; } while(0)

// get/set DOUT_1 aliases
#define DOUT_1_TRIS                 TRISCbits.TRISC1
#define DOUT_1_LAT                  LATCbits.LATC1
#define DOUT_1_PORT                 PORTCbits.RC1
#define DOUT_1_WPU                  WPUCbits.WPUC1
#define DOUT_1_OD                   ODCONCbits.ODCC1
#define DOUT_1_ANS                  ANSELCbits.ANSELC1
#define DOUT_1_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define DOUT_1_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define DOUT_1_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define DOUT_1_GetValue()           PORTCbits.RC1
#define DOUT_1_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define DOUT_1_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define DOUT_1_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define DOUT_1_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define DOUT_1_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define DOUT_1_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define DOUT_1_SetAnalogMode()      do { ANSELCbits.ANSELC1 = 1; } while(0)
#define DOUT_1_SetDigitalMode()     do { ANSELCbits.ANSELC1 = 0; } while(0)

// get/set RC2 procedures
#define RC2_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RC2_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RC2_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RC2_GetValue()              PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RC2_SetPullup()             do { WPUCbits.WPUC2 = 1; } while(0)
#define RC2_ResetPullup()           do { WPUCbits.WPUC2 = 0; } while(0)
#define RC2_SetAnalogMode()         do { ANSELCbits.ANSELC2 = 1; } while(0)
#define RC2_SetDigitalMode()        do { ANSELCbits.ANSELC2 = 0; } while(0)

// get/set RC3 procedures
#define RC3_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define RC3_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define RC3_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define RC3_GetValue()              PORTCbits.RC3
#define RC3_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define RC3_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define RC3_SetPullup()             do { WPUCbits.WPUC3 = 1; } while(0)
#define RC3_ResetPullup()           do { WPUCbits.WPUC3 = 0; } while(0)
#define RC3_SetAnalogMode()         do { ANSELCbits.ANSELC3 = 1; } while(0)
#define RC3_SetDigitalMode()        do { ANSELCbits.ANSELC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()             do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()           do { WPUCbits.WPUC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSELC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSELC4 = 0; } while(0)

// get/set DIN_0 aliases
#define DIN_0_TRIS                 TRISCbits.TRISC7
#define DIN_0_LAT                  LATCbits.LATC7
#define DIN_0_PORT                 PORTCbits.RC7
#define DIN_0_WPU                  WPUCbits.WPUC7
#define DIN_0_OD                   ODCONCbits.ODCC7
#define DIN_0_ANS                  ANSELCbits.ANSELC7
#define DIN_0_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define DIN_0_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define DIN_0_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define DIN_0_GetValue()           PORTCbits.RC7
#define DIN_0_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define DIN_0_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define DIN_0_SetPullup()          do { WPUCbits.WPUC7 = 1; } while(0)
#define DIN_0_ResetPullup()        do { WPUCbits.WPUC7 = 0; } while(0)
#define DIN_0_SetPushPull()        do { ODCONCbits.ODCC7 = 0; } while(0)
#define DIN_0_SetOpenDrain()       do { ODCONCbits.ODCC7 = 1; } while(0)
#define DIN_0_SetAnalogMode()      do { ANSELCbits.ANSELC7 = 1; } while(0)
#define DIN_0_SetDigitalMode()     do { ANSELCbits.ANSELC7 = 0; } while(0)

// get/set DIN_1 aliases
#define DIN_1_TRIS                 TRISDbits.TRISD4
#define DIN_1_LAT                  LATDbits.LATD4
#define DIN_1_PORT                 PORTDbits.RD4
#define DIN_1_WPU                  WPUDbits.WPUD4
#define DIN_1_OD                   ODCONDbits.ODCD4
#define DIN_1_ANS                  ANSELDbits.ANSELD4
#define DIN_1_SetHigh()            do { LATDbits.LATD4 = 1; } while(0)
#define DIN_1_SetLow()             do { LATDbits.LATD4 = 0; } while(0)
#define DIN_1_Toggle()             do { LATDbits.LATD4 = ~LATDbits.LATD4; } while(0)
#define DIN_1_GetValue()           PORTDbits.RD4
#define DIN_1_SetDigitalInput()    do { TRISDbits.TRISD4 = 1; } while(0)
#define DIN_1_SetDigitalOutput()   do { TRISDbits.TRISD4 = 0; } while(0)
#define DIN_1_SetPullup()          do { WPUDbits.WPUD4 = 1; } while(0)
#define DIN_1_ResetPullup()        do { WPUDbits.WPUD4 = 0; } while(0)
#define DIN_1_SetPushPull()        do { ODCONDbits.ODCD4 = 0; } while(0)
#define DIN_1_SetOpenDrain()       do { ODCONDbits.ODCD4 = 1; } while(0)
#define DIN_1_SetAnalogMode()      do { ANSELDbits.ANSELD4 = 1; } while(0)
#define DIN_1_SetDigitalMode()     do { ANSELDbits.ANSELD4 = 0; } while(0)

// get/set DIN_2 aliases
#define DIN_2_TRIS                 TRISDbits.TRISD5
#define DIN_2_LAT                  LATDbits.LATD5
#define DIN_2_PORT                 PORTDbits.RD5
#define DIN_2_WPU                  WPUDbits.WPUD5
#define DIN_2_OD                   ODCONDbits.ODCD5
#define DIN_2_ANS                  ANSELDbits.ANSELD5
#define DIN_2_SetHigh()            do { LATDbits.LATD5 = 1; } while(0)
#define DIN_2_SetLow()             do { LATDbits.LATD5 = 0; } while(0)
#define DIN_2_Toggle()             do { LATDbits.LATD5 = ~LATDbits.LATD5; } while(0)
#define DIN_2_GetValue()           PORTDbits.RD5
#define DIN_2_SetDigitalInput()    do { TRISDbits.TRISD5 = 1; } while(0)
#define DIN_2_SetDigitalOutput()   do { TRISDbits.TRISD5 = 0; } while(0)
#define DIN_2_SetPullup()          do { WPUDbits.WPUD5 = 1; } while(0)
#define DIN_2_ResetPullup()        do { WPUDbits.WPUD5 = 0; } while(0)
#define DIN_2_SetPushPull()        do { ODCONDbits.ODCD5 = 0; } while(0)
#define DIN_2_SetOpenDrain()       do { ODCONDbits.ODCD5 = 1; } while(0)
#define DIN_2_SetAnalogMode()      do { ANSELDbits.ANSELD5 = 1; } while(0)
#define DIN_2_SetDigitalMode()     do { ANSELDbits.ANSELD5 = 0; } while(0)

// get/set DIN_3 aliases
#define DIN_3_TRIS                 TRISDbits.TRISD6
#define DIN_3_LAT                  LATDbits.LATD6
#define DIN_3_PORT                 PORTDbits.RD6
#define DIN_3_WPU                  WPUDbits.WPUD6
#define DIN_3_OD                   ODCONDbits.ODCD6
#define DIN_3_ANS                  ANSELDbits.ANSELD6
#define DIN_3_SetHigh()            do { LATDbits.LATD6 = 1; } while(0)
#define DIN_3_SetLow()             do { LATDbits.LATD6 = 0; } while(0)
#define DIN_3_Toggle()             do { LATDbits.LATD6 = ~LATDbits.LATD6; } while(0)
#define DIN_3_GetValue()           PORTDbits.RD6
#define DIN_3_SetDigitalInput()    do { TRISDbits.TRISD6 = 1; } while(0)
#define DIN_3_SetDigitalOutput()   do { TRISDbits.TRISD6 = 0; } while(0)
#define DIN_3_SetPullup()          do { WPUDbits.WPUD6 = 1; } while(0)
#define DIN_3_ResetPullup()        do { WPUDbits.WPUD6 = 0; } while(0)
#define DIN_3_SetPushPull()        do { ODCONDbits.ODCD6 = 0; } while(0)
#define DIN_3_SetOpenDrain()       do { ODCONDbits.ODCD6 = 1; } while(0)
#define DIN_3_SetAnalogMode()      do { ANSELDbits.ANSELD6 = 1; } while(0)
#define DIN_3_SetDigitalMode()     do { ANSELDbits.ANSELD6 = 0; } while(0)

// get/set DOUT_3 aliases
#define DOUT_3_TRIS                 TRISEbits.TRISE2
#define DOUT_3_LAT                  LATEbits.LATE2
#define DOUT_3_PORT                 PORTEbits.RE2
#define DOUT_3_WPU                  WPUEbits.WPUE2
#define DOUT_3_OD                   ODCONEbits.ODCE2
#define DOUT_3_ANS                  ANSELEbits.ANSELE2
#define DOUT_3_SetHigh()            do { LATEbits.LATE2 = 1; } while(0)
#define DOUT_3_SetLow()             do { LATEbits.LATE2 = 0; } while(0)
#define DOUT_3_Toggle()             do { LATEbits.LATE2 = ~LATEbits.LATE2; } while(0)
#define DOUT_3_GetValue()           PORTEbits.RE2
#define DOUT_3_SetDigitalInput()    do { TRISEbits.TRISE2 = 1; } while(0)
#define DOUT_3_SetDigitalOutput()   do { TRISEbits.TRISE2 = 0; } while(0)
#define DOUT_3_SetPullup()          do { WPUEbits.WPUE2 = 1; } while(0)
#define DOUT_3_ResetPullup()        do { WPUEbits.WPUE2 = 0; } while(0)
#define DOUT_3_SetPushPull()        do { ODCONEbits.ODCE2 = 0; } while(0)
#define DOUT_3_SetOpenDrain()       do { ODCONEbits.ODCE2 = 1; } while(0)
#define DOUT_3_SetAnalogMode()      do { ANSELEbits.ANSELE2 = 1; } while(0)
#define DOUT_3_SetDigitalMode()     do { ANSELEbits.ANSELE2 = 0; } while(0)

// get/set DOUT_0 aliases
#define DOUT_0_TRIS                 TRISFbits.TRISF0
#define DOUT_0_LAT                  LATFbits.LATF0
#define DOUT_0_PORT                 PORTFbits.RF0
#define DOUT_0_WPU                  WPUFbits.WPUF0
#define DOUT_0_OD                   ODCONFbits.ODCF0
#define DOUT_0_ANS                  ANSELFbits.ANSELF0
#define DOUT_0_SetHigh()            do { LATFbits.LATF0 = 1; } while(0)
#define DOUT_0_SetLow()             do { LATFbits.LATF0 = 0; } while(0)
#define DOUT_0_Toggle()             do { LATFbits.LATF0 = ~LATFbits.LATF0; } while(0)
#define DOUT_0_GetValue()           PORTFbits.RF0
#define DOUT_0_SetDigitalInput()    do { TRISFbits.TRISF0 = 1; } while(0)
#define DOUT_0_SetDigitalOutput()   do { TRISFbits.TRISF0 = 0; } while(0)
#define DOUT_0_SetPullup()          do { WPUFbits.WPUF0 = 1; } while(0)
#define DOUT_0_ResetPullup()        do { WPUFbits.WPUF0 = 0; } while(0)
#define DOUT_0_SetPushPull()        do { ODCONFbits.ODCF0 = 0; } while(0)
#define DOUT_0_SetOpenDrain()       do { ODCONFbits.ODCF0 = 1; } while(0)
#define DOUT_0_SetAnalogMode()      do { ANSELFbits.ANSELF0 = 1; } while(0)
#define DOUT_0_SetDigitalMode()     do { ANSELFbits.ANSELF0 = 0; } while(0)

// get/set RF3 procedures
#define RF3_SetHigh()            do { LATFbits.LATF3 = 1; } while(0)
#define RF3_SetLow()             do { LATFbits.LATF3 = 0; } while(0)
#define RF3_Toggle()             do { LATFbits.LATF3 = ~LATFbits.LATF3; } while(0)
#define RF3_GetValue()              PORTFbits.RF3
#define RF3_SetDigitalInput()    do { TRISFbits.TRISF3 = 1; } while(0)
#define RF3_SetDigitalOutput()   do { TRISFbits.TRISF3 = 0; } while(0)
#define RF3_SetPullup()             do { WPUFbits.WPUF3 = 1; } while(0)
#define RF3_ResetPullup()           do { WPUFbits.WPUF3 = 0; } while(0)
#define RF3_SetAnalogMode()         do { ANSELFbits.ANSELF3 = 1; } while(0)
#define RF3_SetDigitalMode()        do { ANSELFbits.ANSELF3 = 0; } while(0)

// get/set I2C_SCL aliases
#define I2C_SCL_TRIS                 TRISFbits.TRISF5
#define I2C_SCL_LAT                  LATFbits.LATF5
#define I2C_SCL_PORT                 PORTFbits.RF5
#define I2C_SCL_WPU                  WPUFbits.WPUF5
#define I2C_SCL_OD                   ODCONFbits.ODCF5
#define I2C_SCL_ANS                  ANSELFbits.ANSELF5
#define I2C_SCL_SetHigh()            do { LATFbits.LATF5 = 1; } while(0)
#define I2C_SCL_SetLow()             do { LATFbits.LATF5 = 0; } while(0)
#define I2C_SCL_Toggle()             do { LATFbits.LATF5 = ~LATFbits.LATF5; } while(0)
#define I2C_SCL_GetValue()           PORTFbits.RF5
#define I2C_SCL_SetDigitalInput()    do { TRISFbits.TRISF5 = 1; } while(0)
#define I2C_SCL_SetDigitalOutput()   do { TRISFbits.TRISF5 = 0; } while(0)
#define I2C_SCL_SetPullup()          do { WPUFbits.WPUF5 = 1; } while(0)
#define I2C_SCL_ResetPullup()        do { WPUFbits.WPUF5 = 0; } while(0)
#define I2C_SCL_SetPushPull()        do { ODCONFbits.ODCF5 = 0; } while(0)
#define I2C_SCL_SetOpenDrain()       do { ODCONFbits.ODCF5 = 1; } while(0)
#define I2C_SCL_SetAnalogMode()      do { ANSELFbits.ANSELF5 = 1; } while(0)
#define I2C_SCL_SetDigitalMode()     do { ANSELFbits.ANSELF5 = 0; } while(0)

// get/set I2C_SDA aliases
#define I2C_SDA_TRIS                 TRISFbits.TRISF6
#define I2C_SDA_LAT                  LATFbits.LATF6
#define I2C_SDA_PORT                 PORTFbits.RF6
#define I2C_SDA_WPU                  WPUFbits.WPUF6
#define I2C_SDA_OD                   ODCONFbits.ODCF6
#define I2C_SDA_ANS                  ANSELFbits.ANSELF6
#define I2C_SDA_SetHigh()            do { LATFbits.LATF6 = 1; } while(0)
#define I2C_SDA_SetLow()             do { LATFbits.LATF6 = 0; } while(0)
#define I2C_SDA_Toggle()             do { LATFbits.LATF6 = ~LATFbits.LATF6; } while(0)
#define I2C_SDA_GetValue()           PORTFbits.RF6
#define I2C_SDA_SetDigitalInput()    do { TRISFbits.TRISF6 = 1; } while(0)
#define I2C_SDA_SetDigitalOutput()   do { TRISFbits.TRISF6 = 0; } while(0)
#define I2C_SDA_SetPullup()          do { WPUFbits.WPUF6 = 1; } while(0)
#define I2C_SDA_ResetPullup()        do { WPUFbits.WPUF6 = 0; } while(0)
#define I2C_SDA_SetPushPull()        do { ODCONFbits.ODCF6 = 0; } while(0)
#define I2C_SDA_SetOpenDrain()       do { ODCONFbits.ODCF6 = 1; } while(0)
#define I2C_SDA_SetAnalogMode()      do { ANSELFbits.ANSELF6 = 1; } while(0)
#define I2C_SDA_SetDigitalMode()     do { ANSELFbits.ANSELF6 = 0; } while(0)

// get/set ONE_WIRE_1 aliases
#define ONE_WIRE_1_TRIS                 TRISFbits.TRISF7
#define ONE_WIRE_1_LAT                  LATFbits.LATF7
#define ONE_WIRE_1_PORT                 PORTFbits.RF7
#define ONE_WIRE_1_WPU                  WPUFbits.WPUF7
#define ONE_WIRE_1_OD                   ODCONFbits.ODCF7
#define ONE_WIRE_1_ANS                  ANSELFbits.ANSELF7
#define ONE_WIRE_1_SetHigh()            do { LATFbits.LATF7 = 1; } while(0)
#define ONE_WIRE_1_SetLow()             do { LATFbits.LATF7 = 0; } while(0)
#define ONE_WIRE_1_Toggle()             do { LATFbits.LATF7 = ~LATFbits.LATF7; } while(0)
#define ONE_WIRE_1_GetValue()           PORTFbits.RF7
#define ONE_WIRE_1_SetDigitalInput()    do { TRISFbits.TRISF7 = 1; } while(0)
#define ONE_WIRE_1_SetDigitalOutput()   do { TRISFbits.TRISF7 = 0; } while(0)
#define ONE_WIRE_1_SetPullup()          do { WPUFbits.WPUF7 = 1; } while(0)
#define ONE_WIRE_1_ResetPullup()        do { WPUFbits.WPUF7 = 0; } while(0)
#define ONE_WIRE_1_SetPushPull()        do { ODCONFbits.ODCF7 = 0; } while(0)
#define ONE_WIRE_1_SetOpenDrain()       do { ODCONFbits.ODCF7 = 1; } while(0)
#define ONE_WIRE_1_SetAnalogMode()      do { ANSELFbits.ANSELF7 = 1; } while(0)
#define ONE_WIRE_1_SetDigitalMode()     do { ANSELFbits.ANSELF7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);




#endif // PIN_MANAGER_H
/**
 End of File
*/