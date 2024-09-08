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

// get/set RA0 procedures
#define RA0_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define RA0_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define RA0_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define RA0_GetValue()              PORTAbits.RA0
#define RA0_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define RA0_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define RA0_SetPullup()             do { WPUAbits.WPUA0 = 1; } while(0)
#define RA0_ResetPullup()           do { WPUAbits.WPUA0 = 0; } while(0)
#define RA0_SetAnalogMode()         do { ANSELAbits.ANSELA0 = 1; } while(0)
#define RA0_SetDigitalMode()        do { ANSELAbits.ANSELA0 = 0; } while(0)

// get/set ONE_WIRE_1 aliases
#define ONE_WIRE_1_TRIS                 TRISAbits.TRISA1
#define ONE_WIRE_1_LAT                  LATAbits.LATA1
#define ONE_WIRE_1_PORT                 PORTAbits.RA1
#define ONE_WIRE_1_WPU                  WPUAbits.WPUA1
#define ONE_WIRE_1_OD                   ODCONAbits.ODCA1
#define ONE_WIRE_1_ANS                  ANSELAbits.ANSELA1
#define ONE_WIRE_1_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define ONE_WIRE_1_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define ONE_WIRE_1_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define ONE_WIRE_1_GetValue()           PORTAbits.RA1
#define ONE_WIRE_1_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define ONE_WIRE_1_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define ONE_WIRE_1_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define ONE_WIRE_1_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define ONE_WIRE_1_SetPushPull()        do { ODCONAbits.ODCA1 = 0; } while(0)
#define ONE_WIRE_1_SetOpenDrain()       do { ODCONAbits.ODCA1 = 1; } while(0)
#define ONE_WIRE_1_SetAnalogMode()      do { ANSELAbits.ANSELA1 = 1; } while(0)
#define ONE_WIRE_1_SetDigitalMode()     do { ANSELAbits.ANSELA1 = 0; } while(0)

// get/set PIR_SIGNAL aliases
#define PIR_SIGNAL_TRIS                 TRISAbits.TRISA2
#define PIR_SIGNAL_LAT                  LATAbits.LATA2
#define PIR_SIGNAL_PORT                 PORTAbits.RA2
#define PIR_SIGNAL_WPU                  WPUAbits.WPUA2
#define PIR_SIGNAL_OD                   ODCONAbits.ODCA2
#define PIR_SIGNAL_ANS                  ANSELAbits.ANSELA2
#define PIR_SIGNAL_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define PIR_SIGNAL_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define PIR_SIGNAL_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define PIR_SIGNAL_GetValue()           PORTAbits.RA2
#define PIR_SIGNAL_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define PIR_SIGNAL_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define PIR_SIGNAL_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define PIR_SIGNAL_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define PIR_SIGNAL_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define PIR_SIGNAL_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define PIR_SIGNAL_SetAnalogMode()      do { ANSELAbits.ANSELA2 = 1; } while(0)
#define PIR_SIGNAL_SetDigitalMode()     do { ANSELAbits.ANSELA2 = 0; } while(0)

// get/set RA3 procedures
#define RA3_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define RA3_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define RA3_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define RA3_GetValue()              PORTAbits.RA3
#define RA3_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define RA3_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define RA3_SetPullup()             do { WPUAbits.WPUA3 = 1; } while(0)
#define RA3_ResetPullup()           do { WPUAbits.WPUA3 = 0; } while(0)
#define RA3_SetAnalogMode()         do { ANSELAbits.ANSELA3 = 1; } while(0)
#define RA3_SetDigitalMode()        do { ANSELAbits.ANSELA3 = 0; } while(0)

// get/set I2C_SCL aliases
#define I2C_SCL_TRIS                 TRISAbits.TRISA4
#define I2C_SCL_LAT                  LATAbits.LATA4
#define I2C_SCL_PORT                 PORTAbits.RA4
#define I2C_SCL_WPU                  WPUAbits.WPUA4
#define I2C_SCL_OD                   ODCONAbits.ODCA4
#define I2C_SCL_ANS                  ANSELAbits.ANSELA4
#define I2C_SCL_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define I2C_SCL_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define I2C_SCL_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define I2C_SCL_GetValue()           PORTAbits.RA4
#define I2C_SCL_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define I2C_SCL_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define I2C_SCL_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define I2C_SCL_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define I2C_SCL_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define I2C_SCL_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define I2C_SCL_SetAnalogMode()      do { ANSELAbits.ANSELA4 = 1; } while(0)
#define I2C_SCL_SetDigitalMode()     do { ANSELAbits.ANSELA4 = 0; } while(0)

// get/set I2C_SDA aliases
#define I2C_SDA_TRIS                 TRISAbits.TRISA5
#define I2C_SDA_LAT                  LATAbits.LATA5
#define I2C_SDA_PORT                 PORTAbits.RA5
#define I2C_SDA_WPU                  WPUAbits.WPUA5
#define I2C_SDA_OD                   ODCONAbits.ODCA5
#define I2C_SDA_ANS                  ANSELAbits.ANSELA5
#define I2C_SDA_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define I2C_SDA_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define I2C_SDA_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define I2C_SDA_GetValue()           PORTAbits.RA5
#define I2C_SDA_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define I2C_SDA_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define I2C_SDA_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define I2C_SDA_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define I2C_SDA_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define I2C_SDA_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define I2C_SDA_SetAnalogMode()      do { ANSELAbits.ANSELA5 = 1; } while(0)
#define I2C_SDA_SetDigitalMode()     do { ANSELAbits.ANSELA5 = 0; } while(0)

// get/set RB0 procedures
#define RB0_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define RB0_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define RB0_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define RB0_GetValue()              PORTBbits.RB0
#define RB0_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define RB0_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define RB0_SetPullup()             do { WPUBbits.WPUB0 = 1; } while(0)
#define RB0_ResetPullup()           do { WPUBbits.WPUB0 = 0; } while(0)
#define RB0_SetAnalogMode()         do { ANSELBbits.ANSELB0 = 1; } while(0)
#define RB0_SetDigitalMode()        do { ANSELBbits.ANSELB0 = 0; } while(0)

// get/set LCD_A0 aliases
#define LCD_A0_TRIS                 TRISBbits.TRISB1
#define LCD_A0_LAT                  LATBbits.LATB1
#define LCD_A0_PORT                 PORTBbits.RB1
#define LCD_A0_WPU                  WPUBbits.WPUB1
#define LCD_A0_OD                   ODCONBbits.ODCB1
#define LCD_A0_ANS                  ANSELBbits.ANSELB1
#define LCD_A0_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define LCD_A0_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define LCD_A0_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define LCD_A0_GetValue()           PORTBbits.RB1
#define LCD_A0_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define LCD_A0_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define LCD_A0_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define LCD_A0_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define LCD_A0_SetPushPull()        do { ODCONBbits.ODCB1 = 0; } while(0)
#define LCD_A0_SetOpenDrain()       do { ODCONBbits.ODCB1 = 1; } while(0)
#define LCD_A0_SetAnalogMode()      do { ANSELBbits.ANSELB1 = 1; } while(0)
#define LCD_A0_SetDigitalMode()     do { ANSELBbits.ANSELB1 = 0; } while(0)

// get/set RB2 procedures
#define RB2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define RB2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define RB2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define RB2_GetValue()              PORTBbits.RB2
#define RB2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define RB2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define RB2_SetPullup()             do { WPUBbits.WPUB2 = 1; } while(0)
#define RB2_ResetPullup()           do { WPUBbits.WPUB2 = 0; } while(0)
#define RB2_SetAnalogMode()         do { ANSELBbits.ANSELB2 = 1; } while(0)
#define RB2_SetDigitalMode()        do { ANSELBbits.ANSELB2 = 0; } while(0)

// get/set RB3 procedures
#define RB3_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define RB3_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define RB3_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define RB3_GetValue()              PORTBbits.RB3
#define RB3_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define RB3_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define RB3_SetPullup()             do { WPUBbits.WPUB3 = 1; } while(0)
#define RB3_ResetPullup()           do { WPUBbits.WPUB3 = 0; } while(0)
#define RB3_SetAnalogMode()         do { ANSELBbits.ANSELB3 = 1; } while(0)
#define RB3_SetDigitalMode()        do { ANSELBbits.ANSELB3 = 0; } while(0)

// get/set LED_AMBIENT aliases
#define LED_AMBIENT_TRIS                 TRISBbits.TRISB4
#define LED_AMBIENT_LAT                  LATBbits.LATB4
#define LED_AMBIENT_PORT                 PORTBbits.RB4
#define LED_AMBIENT_WPU                  WPUBbits.WPUB4
#define LED_AMBIENT_OD                   ODCONBbits.ODCB4
#define LED_AMBIENT_ANS                  ANSELBbits.ANSELB4
#define LED_AMBIENT_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define LED_AMBIENT_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define LED_AMBIENT_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define LED_AMBIENT_GetValue()           PORTBbits.RB4
#define LED_AMBIENT_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define LED_AMBIENT_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define LED_AMBIENT_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define LED_AMBIENT_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define LED_AMBIENT_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define LED_AMBIENT_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define LED_AMBIENT_SetAnalogMode()      do { ANSELBbits.ANSELB4 = 1; } while(0)
#define LED_AMBIENT_SetDigitalMode()     do { ANSELBbits.ANSELB4 = 0; } while(0)

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

// get/set UNUSED aliases
#define UNUSED_TRIS                 TRISCbits.TRISC0
#define UNUSED_LAT                  LATCbits.LATC0
#define UNUSED_PORT                 PORTCbits.RC0
#define UNUSED_WPU                  WPUCbits.WPUC0
#define UNUSED_OD                   ODCONCbits.ODCC0
#define UNUSED_ANS                  ANSELCbits.ANSELC0
#define UNUSED_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define UNUSED_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define UNUSED_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define UNUSED_GetValue()           PORTCbits.RC0
#define UNUSED_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define UNUSED_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define UNUSED_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define UNUSED_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define UNUSED_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define UNUSED_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define UNUSED_SetAnalogMode()      do { ANSELCbits.ANSELC0 = 1; } while(0)
#define UNUSED_SetDigitalMode()     do { ANSELCbits.ANSELC0 = 0; } while(0)

// get/set GPIO_DIN_3 aliases
#define GPIO_DIN_3_TRIS                 TRISCbits.TRISC1
#define GPIO_DIN_3_LAT                  LATCbits.LATC1
#define GPIO_DIN_3_PORT                 PORTCbits.RC1
#define GPIO_DIN_3_WPU                  WPUCbits.WPUC1
#define GPIO_DIN_3_OD                   ODCONCbits.ODCC1
#define GPIO_DIN_3_ANS                  ANSELCbits.ANSELC1
#define GPIO_DIN_3_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define GPIO_DIN_3_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define GPIO_DIN_3_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define GPIO_DIN_3_GetValue()           PORTCbits.RC1
#define GPIO_DIN_3_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define GPIO_DIN_3_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define GPIO_DIN_3_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define GPIO_DIN_3_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define GPIO_DIN_3_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define GPIO_DIN_3_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define GPIO_DIN_3_SetAnalogMode()      do { ANSELCbits.ANSELC1 = 1; } while(0)
#define GPIO_DIN_3_SetDigitalMode()     do { ANSELCbits.ANSELC1 = 0; } while(0)

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

// get/set GPIO_DOUT_1 aliases
#define GPIO_DOUT_1_TRIS                 TRISCbits.TRISC4
#define GPIO_DOUT_1_LAT                  LATCbits.LATC4
#define GPIO_DOUT_1_PORT                 PORTCbits.RC4
#define GPIO_DOUT_1_WPU                  WPUCbits.WPUC4
#define GPIO_DOUT_1_OD                   ODCONCbits.ODCC4
#define GPIO_DOUT_1_ANS                  ANSELCbits.ANSELC4
#define GPIO_DOUT_1_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define GPIO_DOUT_1_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define GPIO_DOUT_1_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define GPIO_DOUT_1_GetValue()           PORTCbits.RC4
#define GPIO_DOUT_1_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define GPIO_DOUT_1_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define GPIO_DOUT_1_SetPullup()          do { WPUCbits.WPUC4 = 1; } while(0)
#define GPIO_DOUT_1_ResetPullup()        do { WPUCbits.WPUC4 = 0; } while(0)
#define GPIO_DOUT_1_SetPushPull()        do { ODCONCbits.ODCC4 = 0; } while(0)
#define GPIO_DOUT_1_SetOpenDrain()       do { ODCONCbits.ODCC4 = 1; } while(0)
#define GPIO_DOUT_1_SetAnalogMode()      do { ANSELCbits.ANSELC4 = 1; } while(0)
#define GPIO_DOUT_1_SetDigitalMode()     do { ANSELCbits.ANSELC4 = 0; } while(0)

// get/set GPIO_DOUT_2 aliases
#define GPIO_DOUT_2_TRIS                 TRISCbits.TRISC5
#define GPIO_DOUT_2_LAT                  LATCbits.LATC5
#define GPIO_DOUT_2_PORT                 PORTCbits.RC5
#define GPIO_DOUT_2_WPU                  WPUCbits.WPUC5
#define GPIO_DOUT_2_OD                   ODCONCbits.ODCC5
#define GPIO_DOUT_2_ANS                  ANSELCbits.ANSELC5
#define GPIO_DOUT_2_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define GPIO_DOUT_2_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define GPIO_DOUT_2_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define GPIO_DOUT_2_GetValue()           PORTCbits.RC5
#define GPIO_DOUT_2_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define GPIO_DOUT_2_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define GPIO_DOUT_2_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define GPIO_DOUT_2_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define GPIO_DOUT_2_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define GPIO_DOUT_2_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define GPIO_DOUT_2_SetAnalogMode()      do { ANSELCbits.ANSELC5 = 1; } while(0)
#define GPIO_DOUT_2_SetDigitalMode()     do { ANSELCbits.ANSELC5 = 0; } while(0)

// get/set GPIO_DOUT_3 aliases
#define GPIO_DOUT_3_TRIS                 TRISCbits.TRISC6
#define GPIO_DOUT_3_LAT                  LATCbits.LATC6
#define GPIO_DOUT_3_PORT                 PORTCbits.RC6
#define GPIO_DOUT_3_WPU                  WPUCbits.WPUC6
#define GPIO_DOUT_3_OD                   ODCONCbits.ODCC6
#define GPIO_DOUT_3_ANS                  ANSELCbits.ANSELC6
#define GPIO_DOUT_3_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define GPIO_DOUT_3_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define GPIO_DOUT_3_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define GPIO_DOUT_3_GetValue()           PORTCbits.RC6
#define GPIO_DOUT_3_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define GPIO_DOUT_3_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define GPIO_DOUT_3_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define GPIO_DOUT_3_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define GPIO_DOUT_3_SetPushPull()        do { ODCONCbits.ODCC6 = 0; } while(0)
#define GPIO_DOUT_3_SetOpenDrain()       do { ODCONCbits.ODCC6 = 1; } while(0)
#define GPIO_DOUT_3_SetAnalogMode()      do { ANSELCbits.ANSELC6 = 1; } while(0)
#define GPIO_DOUT_3_SetDigitalMode()     do { ANSELCbits.ANSELC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetPullup()             do { WPUCbits.WPUC7 = 1; } while(0)
#define RC7_ResetPullup()           do { WPUCbits.WPUC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSELC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSELC7 = 0; } while(0)

// get/set ADC_V_IN aliases
#define ADC_V_IN_TRIS                 TRISDbits.TRISD0
#define ADC_V_IN_LAT                  LATDbits.LATD0
#define ADC_V_IN_PORT                 PORTDbits.RD0
#define ADC_V_IN_WPU                  WPUDbits.WPUD0
#define ADC_V_IN_OD                   ODCONDbits.ODCD0
#define ADC_V_IN_ANS                  ANSELDbits.ANSELD0
#define ADC_V_IN_SetHigh()            do { LATDbits.LATD0 = 1; } while(0)
#define ADC_V_IN_SetLow()             do { LATDbits.LATD0 = 0; } while(0)
#define ADC_V_IN_Toggle()             do { LATDbits.LATD0 = ~LATDbits.LATD0; } while(0)
#define ADC_V_IN_GetValue()           PORTDbits.RD0
#define ADC_V_IN_SetDigitalInput()    do { TRISDbits.TRISD0 = 1; } while(0)
#define ADC_V_IN_SetDigitalOutput()   do { TRISDbits.TRISD0 = 0; } while(0)
#define ADC_V_IN_SetPullup()          do { WPUDbits.WPUD0 = 1; } while(0)
#define ADC_V_IN_ResetPullup()        do { WPUDbits.WPUD0 = 0; } while(0)
#define ADC_V_IN_SetPushPull()        do { ODCONDbits.ODCD0 = 0; } while(0)
#define ADC_V_IN_SetOpenDrain()       do { ODCONDbits.ODCD0 = 1; } while(0)
#define ADC_V_IN_SetAnalogMode()      do { ANSELDbits.ANSELD0 = 1; } while(0)
#define ADC_V_IN_SetDigitalMode()     do { ANSELDbits.ANSELD0 = 0; } while(0)

// get/set ADC_V_IN_IO aliases
#define ADC_V_IN_IO_TRIS                 TRISDbits.TRISD1
#define ADC_V_IN_IO_LAT                  LATDbits.LATD1
#define ADC_V_IN_IO_PORT                 PORTDbits.RD1
#define ADC_V_IN_IO_WPU                  WPUDbits.WPUD1
#define ADC_V_IN_IO_OD                   ODCONDbits.ODCD1
#define ADC_V_IN_IO_ANS                  ANSELDbits.ANSELD1
#define ADC_V_IN_IO_SetHigh()            do { LATDbits.LATD1 = 1; } while(0)
#define ADC_V_IN_IO_SetLow()             do { LATDbits.LATD1 = 0; } while(0)
#define ADC_V_IN_IO_Toggle()             do { LATDbits.LATD1 = ~LATDbits.LATD1; } while(0)
#define ADC_V_IN_IO_GetValue()           PORTDbits.RD1
#define ADC_V_IN_IO_SetDigitalInput()    do { TRISDbits.TRISD1 = 1; } while(0)
#define ADC_V_IN_IO_SetDigitalOutput()   do { TRISDbits.TRISD1 = 0; } while(0)
#define ADC_V_IN_IO_SetPullup()          do { WPUDbits.WPUD1 = 1; } while(0)
#define ADC_V_IN_IO_ResetPullup()        do { WPUDbits.WPUD1 = 0; } while(0)
#define ADC_V_IN_IO_SetPushPull()        do { ODCONDbits.ODCD1 = 0; } while(0)
#define ADC_V_IN_IO_SetOpenDrain()       do { ODCONDbits.ODCD1 = 1; } while(0)
#define ADC_V_IN_IO_SetAnalogMode()      do { ANSELDbits.ANSELD1 = 1; } while(0)
#define ADC_V_IN_IO_SetDigitalMode()     do { ANSELDbits.ANSELD1 = 0; } while(0)

// get/set RD2 procedures
#define RD2_SetHigh()            do { LATDbits.LATD2 = 1; } while(0)
#define RD2_SetLow()             do { LATDbits.LATD2 = 0; } while(0)
#define RD2_Toggle()             do { LATDbits.LATD2 = ~LATDbits.LATD2; } while(0)
#define RD2_GetValue()              PORTDbits.RD2
#define RD2_SetDigitalInput()    do { TRISDbits.TRISD2 = 1; } while(0)
#define RD2_SetDigitalOutput()   do { TRISDbits.TRISD2 = 0; } while(0)
#define RD2_SetPullup()             do { WPUDbits.WPUD2 = 1; } while(0)
#define RD2_ResetPullup()           do { WPUDbits.WPUD2 = 0; } while(0)
#define RD2_SetAnalogMode()         do { ANSELDbits.ANSELD2 = 1; } while(0)
#define RD2_SetDigitalMode()        do { ANSELDbits.ANSELD2 = 0; } while(0)

// get/set GPIO_DOUT_0 aliases
#define GPIO_DOUT_0_TRIS                 TRISDbits.TRISD3
#define GPIO_DOUT_0_LAT                  LATDbits.LATD3
#define GPIO_DOUT_0_PORT                 PORTDbits.RD3
#define GPIO_DOUT_0_WPU                  WPUDbits.WPUD3
#define GPIO_DOUT_0_OD                   ODCONDbits.ODCD3
#define GPIO_DOUT_0_ANS                  ANSELDbits.ANSELD3
#define GPIO_DOUT_0_SetHigh()            do { LATDbits.LATD3 = 1; } while(0)
#define GPIO_DOUT_0_SetLow()             do { LATDbits.LATD3 = 0; } while(0)
#define GPIO_DOUT_0_Toggle()             do { LATDbits.LATD3 = ~LATDbits.LATD3; } while(0)
#define GPIO_DOUT_0_GetValue()           PORTDbits.RD3
#define GPIO_DOUT_0_SetDigitalInput()    do { TRISDbits.TRISD3 = 1; } while(0)
#define GPIO_DOUT_0_SetDigitalOutput()   do { TRISDbits.TRISD3 = 0; } while(0)
#define GPIO_DOUT_0_SetPullup()          do { WPUDbits.WPUD3 = 1; } while(0)
#define GPIO_DOUT_0_ResetPullup()        do { WPUDbits.WPUD3 = 0; } while(0)
#define GPIO_DOUT_0_SetPushPull()        do { ODCONDbits.ODCD3 = 0; } while(0)
#define GPIO_DOUT_0_SetOpenDrain()       do { ODCONDbits.ODCD3 = 1; } while(0)
#define GPIO_DOUT_0_SetAnalogMode()      do { ANSELDbits.ANSELD3 = 1; } while(0)
#define GPIO_DOUT_0_SetDigitalMode()     do { ANSELDbits.ANSELD3 = 0; } while(0)

// get/set RD4 procedures
#define RD4_SetHigh()            do { LATDbits.LATD4 = 1; } while(0)
#define RD4_SetLow()             do { LATDbits.LATD4 = 0; } while(0)
#define RD4_Toggle()             do { LATDbits.LATD4 = ~LATDbits.LATD4; } while(0)
#define RD4_GetValue()              PORTDbits.RD4
#define RD4_SetDigitalInput()    do { TRISDbits.TRISD4 = 1; } while(0)
#define RD4_SetDigitalOutput()   do { TRISDbits.TRISD4 = 0; } while(0)
#define RD4_SetPullup()             do { WPUDbits.WPUD4 = 1; } while(0)
#define RD4_ResetPullup()           do { WPUDbits.WPUD4 = 0; } while(0)
#define RD4_SetAnalogMode()         do { ANSELDbits.ANSELD4 = 1; } while(0)
#define RD4_SetDigitalMode()        do { ANSELDbits.ANSELD4 = 0; } while(0)

// get/set ONE_WIRE_0 aliases
#define ONE_WIRE_0_TRIS                 TRISEbits.TRISE0
#define ONE_WIRE_0_LAT                  LATEbits.LATE0
#define ONE_WIRE_0_PORT                 PORTEbits.RE0
#define ONE_WIRE_0_WPU                  WPUEbits.WPUE0
#define ONE_WIRE_0_OD                   ODCONEbits.ODCE0
#define ONE_WIRE_0_ANS                  ANSELEbits.ANSELE0
#define ONE_WIRE_0_SetHigh()            do { LATEbits.LATE0 = 1; } while(0)
#define ONE_WIRE_0_SetLow()             do { LATEbits.LATE0 = 0; } while(0)
#define ONE_WIRE_0_Toggle()             do { LATEbits.LATE0 = ~LATEbits.LATE0; } while(0)
#define ONE_WIRE_0_GetValue()           PORTEbits.RE0
#define ONE_WIRE_0_SetDigitalInput()    do { TRISEbits.TRISE0 = 1; } while(0)
#define ONE_WIRE_0_SetDigitalOutput()   do { TRISEbits.TRISE0 = 0; } while(0)
#define ONE_WIRE_0_SetPullup()          do { WPUEbits.WPUE0 = 1; } while(0)
#define ONE_WIRE_0_ResetPullup()        do { WPUEbits.WPUE0 = 0; } while(0)
#define ONE_WIRE_0_SetPushPull()        do { ODCONEbits.ODCE0 = 0; } while(0)
#define ONE_WIRE_0_SetOpenDrain()       do { ODCONEbits.ODCE0 = 1; } while(0)
#define ONE_WIRE_0_SetAnalogMode()      do { ANSELEbits.ANSELE0 = 1; } while(0)
#define ONE_WIRE_0_SetDigitalMode()     do { ANSELEbits.ANSELE0 = 0; } while(0)

// get/set ADC_0 aliases
#define ADC_0_TRIS                 TRISEbits.TRISE1
#define ADC_0_LAT                  LATEbits.LATE1
#define ADC_0_PORT                 PORTEbits.RE1
#define ADC_0_WPU                  WPUEbits.WPUE1
#define ADC_0_OD                   ODCONEbits.ODCE1
#define ADC_0_ANS                  ANSELEbits.ANSELE1
#define ADC_0_SetHigh()            do { LATEbits.LATE1 = 1; } while(0)
#define ADC_0_SetLow()             do { LATEbits.LATE1 = 0; } while(0)
#define ADC_0_Toggle()             do { LATEbits.LATE1 = ~LATEbits.LATE1; } while(0)
#define ADC_0_GetValue()           PORTEbits.RE1
#define ADC_0_SetDigitalInput()    do { TRISEbits.TRISE1 = 1; } while(0)
#define ADC_0_SetDigitalOutput()   do { TRISEbits.TRISE1 = 0; } while(0)
#define ADC_0_SetPullup()          do { WPUEbits.WPUE1 = 1; } while(0)
#define ADC_0_ResetPullup()        do { WPUEbits.WPUE1 = 0; } while(0)
#define ADC_0_SetPushPull()        do { ODCONEbits.ODCE1 = 0; } while(0)
#define ADC_0_SetOpenDrain()       do { ODCONEbits.ODCE1 = 1; } while(0)
#define ADC_0_SetAnalogMode()      do { ANSELEbits.ANSELE1 = 1; } while(0)
#define ADC_0_SetDigitalMode()     do { ANSELEbits.ANSELE1 = 0; } while(0)

// get/set ADC_1 aliases
#define ADC_1_TRIS                 TRISEbits.TRISE2
#define ADC_1_LAT                  LATEbits.LATE2
#define ADC_1_PORT                 PORTEbits.RE2
#define ADC_1_WPU                  WPUEbits.WPUE2
#define ADC_1_OD                   ODCONEbits.ODCE2
#define ADC_1_ANS                  ANSELEbits.ANSELE2
#define ADC_1_SetHigh()            do { LATEbits.LATE2 = 1; } while(0)
#define ADC_1_SetLow()             do { LATEbits.LATE2 = 0; } while(0)
#define ADC_1_Toggle()             do { LATEbits.LATE2 = ~LATEbits.LATE2; } while(0)
#define ADC_1_GetValue()           PORTEbits.RE2
#define ADC_1_SetDigitalInput()    do { TRISEbits.TRISE2 = 1; } while(0)
#define ADC_1_SetDigitalOutput()   do { TRISEbits.TRISE2 = 0; } while(0)
#define ADC_1_SetPullup()          do { WPUEbits.WPUE2 = 1; } while(0)
#define ADC_1_ResetPullup()        do { WPUEbits.WPUE2 = 0; } while(0)
#define ADC_1_SetPushPull()        do { ODCONEbits.ODCE2 = 0; } while(0)
#define ADC_1_SetOpenDrain()       do { ODCONEbits.ODCE2 = 1; } while(0)
#define ADC_1_SetAnalogMode()      do { ANSELEbits.ANSELE2 = 1; } while(0)
#define ADC_1_SetDigitalMode()     do { ANSELEbits.ANSELE2 = 0; } while(0)

// get/set GPIO_DIN_2 aliases
#define GPIO_DIN_2_TRIS                 TRISFbits.TRISF0
#define GPIO_DIN_2_LAT                  LATFbits.LATF0
#define GPIO_DIN_2_PORT                 PORTFbits.RF0
#define GPIO_DIN_2_WPU                  WPUFbits.WPUF0
#define GPIO_DIN_2_OD                   ODCONFbits.ODCF0
#define GPIO_DIN_2_ANS                  ANSELFbits.ANSELF0
#define GPIO_DIN_2_SetHigh()            do { LATFbits.LATF0 = 1; } while(0)
#define GPIO_DIN_2_SetLow()             do { LATFbits.LATF0 = 0; } while(0)
#define GPIO_DIN_2_Toggle()             do { LATFbits.LATF0 = ~LATFbits.LATF0; } while(0)
#define GPIO_DIN_2_GetValue()           PORTFbits.RF0
#define GPIO_DIN_2_SetDigitalInput()    do { TRISFbits.TRISF0 = 1; } while(0)
#define GPIO_DIN_2_SetDigitalOutput()   do { TRISFbits.TRISF0 = 0; } while(0)
#define GPIO_DIN_2_SetPullup()          do { WPUFbits.WPUF0 = 1; } while(0)
#define GPIO_DIN_2_ResetPullup()        do { WPUFbits.WPUF0 = 0; } while(0)
#define GPIO_DIN_2_SetPushPull()        do { ODCONFbits.ODCF0 = 0; } while(0)
#define GPIO_DIN_2_SetOpenDrain()       do { ODCONFbits.ODCF0 = 1; } while(0)
#define GPIO_DIN_2_SetAnalogMode()      do { ANSELFbits.ANSELF0 = 1; } while(0)
#define GPIO_DIN_2_SetDigitalMode()     do { ANSELFbits.ANSELF0 = 0; } while(0)

// get/set GPIO_DIN_1 aliases
#define GPIO_DIN_1_TRIS                 TRISFbits.TRISF1
#define GPIO_DIN_1_LAT                  LATFbits.LATF1
#define GPIO_DIN_1_PORT                 PORTFbits.RF1
#define GPIO_DIN_1_WPU                  WPUFbits.WPUF1
#define GPIO_DIN_1_OD                   ODCONFbits.ODCF1
#define GPIO_DIN_1_ANS                  ANSELFbits.ANSELF1
#define GPIO_DIN_1_SetHigh()            do { LATFbits.LATF1 = 1; } while(0)
#define GPIO_DIN_1_SetLow()             do { LATFbits.LATF1 = 0; } while(0)
#define GPIO_DIN_1_Toggle()             do { LATFbits.LATF1 = ~LATFbits.LATF1; } while(0)
#define GPIO_DIN_1_GetValue()           PORTFbits.RF1
#define GPIO_DIN_1_SetDigitalInput()    do { TRISFbits.TRISF1 = 1; } while(0)
#define GPIO_DIN_1_SetDigitalOutput()   do { TRISFbits.TRISF1 = 0; } while(0)
#define GPIO_DIN_1_SetPullup()          do { WPUFbits.WPUF1 = 1; } while(0)
#define GPIO_DIN_1_ResetPullup()        do { WPUFbits.WPUF1 = 0; } while(0)
#define GPIO_DIN_1_SetPushPull()        do { ODCONFbits.ODCF1 = 0; } while(0)
#define GPIO_DIN_1_SetOpenDrain()       do { ODCONFbits.ODCF1 = 1; } while(0)
#define GPIO_DIN_1_SetAnalogMode()      do { ANSELFbits.ANSELF1 = 1; } while(0)
#define GPIO_DIN_1_SetDigitalMode()     do { ANSELFbits.ANSELF1 = 0; } while(0)

// get/set GPIO_DIN_0 aliases
#define GPIO_DIN_0_TRIS                 TRISFbits.TRISF2
#define GPIO_DIN_0_LAT                  LATFbits.LATF2
#define GPIO_DIN_0_PORT                 PORTFbits.RF2
#define GPIO_DIN_0_WPU                  WPUFbits.WPUF2
#define GPIO_DIN_0_OD                   ODCONFbits.ODCF2
#define GPIO_DIN_0_ANS                  ANSELFbits.ANSELF2
#define GPIO_DIN_0_SetHigh()            do { LATFbits.LATF2 = 1; } while(0)
#define GPIO_DIN_0_SetLow()             do { LATFbits.LATF2 = 0; } while(0)
#define GPIO_DIN_0_Toggle()             do { LATFbits.LATF2 = ~LATFbits.LATF2; } while(0)
#define GPIO_DIN_0_GetValue()           PORTFbits.RF2
#define GPIO_DIN_0_SetDigitalInput()    do { TRISFbits.TRISF2 = 1; } while(0)
#define GPIO_DIN_0_SetDigitalOutput()   do { TRISFbits.TRISF2 = 0; } while(0)
#define GPIO_DIN_0_SetPullup()          do { WPUFbits.WPUF2 = 1; } while(0)
#define GPIO_DIN_0_ResetPullup()        do { WPUFbits.WPUF2 = 0; } while(0)
#define GPIO_DIN_0_SetPushPull()        do { ODCONFbits.ODCF2 = 0; } while(0)
#define GPIO_DIN_0_SetOpenDrain()       do { ODCONFbits.ODCF2 = 1; } while(0)
#define GPIO_DIN_0_SetAnalogMode()      do { ANSELFbits.ANSELF2 = 1; } while(0)
#define GPIO_DIN_0_SetDigitalMode()     do { ANSELFbits.ANSELF2 = 0; } while(0)

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

// get/set LCD_CS aliases
#define LCD_CS_TRIS                 TRISFbits.TRISF4
#define LCD_CS_LAT                  LATFbits.LATF4
#define LCD_CS_PORT                 PORTFbits.RF4
#define LCD_CS_WPU                  WPUFbits.WPUF4
#define LCD_CS_OD                   ODCONFbits.ODCF4
#define LCD_CS_ANS                  ANSELFbits.ANSELF4
#define LCD_CS_SetHigh()            do { LATFbits.LATF4 = 1; } while(0)
#define LCD_CS_SetLow()             do { LATFbits.LATF4 = 0; } while(0)
#define LCD_CS_Toggle()             do { LATFbits.LATF4 = ~LATFbits.LATF4; } while(0)
#define LCD_CS_GetValue()           PORTFbits.RF4
#define LCD_CS_SetDigitalInput()    do { TRISFbits.TRISF4 = 1; } while(0)
#define LCD_CS_SetDigitalOutput()   do { TRISFbits.TRISF4 = 0; } while(0)
#define LCD_CS_SetPullup()          do { WPUFbits.WPUF4 = 1; } while(0)
#define LCD_CS_ResetPullup()        do { WPUFbits.WPUF4 = 0; } while(0)
#define LCD_CS_SetPushPull()        do { ODCONFbits.ODCF4 = 0; } while(0)
#define LCD_CS_SetOpenDrain()       do { ODCONFbits.ODCF4 = 1; } while(0)
#define LCD_CS_SetAnalogMode()      do { ANSELFbits.ANSELF4 = 1; } while(0)
#define LCD_CS_SetDigitalMode()     do { ANSELFbits.ANSELF4 = 0; } while(0)

// get/set RF5 procedures
#define RF5_SetHigh()            do { LATFbits.LATF5 = 1; } while(0)
#define RF5_SetLow()             do { LATFbits.LATF5 = 0; } while(0)
#define RF5_Toggle()             do { LATFbits.LATF5 = ~LATFbits.LATF5; } while(0)
#define RF5_GetValue()              PORTFbits.RF5
#define RF5_SetDigitalInput()    do { TRISFbits.TRISF5 = 1; } while(0)
#define RF5_SetDigitalOutput()   do { TRISFbits.TRISF5 = 0; } while(0)
#define RF5_SetPullup()             do { WPUFbits.WPUF5 = 1; } while(0)
#define RF5_ResetPullup()           do { WPUFbits.WPUF5 = 0; } while(0)
#define RF5_SetAnalogMode()         do { ANSELFbits.ANSELF5 = 1; } while(0)
#define RF5_SetDigitalMode()        do { ANSELFbits.ANSELF5 = 0; } while(0)

// get/set RF6 procedures
#define RF6_SetHigh()            do { LATFbits.LATF6 = 1; } while(0)
#define RF6_SetLow()             do { LATFbits.LATF6 = 0; } while(0)
#define RF6_Toggle()             do { LATFbits.LATF6 = ~LATFbits.LATF6; } while(0)
#define RF6_GetValue()              PORTFbits.RF6
#define RF6_SetDigitalInput()    do { TRISFbits.TRISF6 = 1; } while(0)
#define RF6_SetDigitalOutput()   do { TRISFbits.TRISF6 = 0; } while(0)
#define RF6_SetPullup()             do { WPUFbits.WPUF6 = 1; } while(0)
#define RF6_ResetPullup()           do { WPUFbits.WPUF6 = 0; } while(0)
#define RF6_SetAnalogMode()         do { ANSELFbits.ANSELF6 = 1; } while(0)
#define RF6_SetDigitalMode()        do { ANSELFbits.ANSELF6 = 0; } while(0)

// get/set RF7 procedures
#define RF7_SetHigh()            do { LATFbits.LATF7 = 1; } while(0)
#define RF7_SetLow()             do { LATFbits.LATF7 = 0; } while(0)
#define RF7_Toggle()             do { LATFbits.LATF7 = ~LATFbits.LATF7; } while(0)
#define RF7_GetValue()              PORTFbits.RF7
#define RF7_SetDigitalInput()    do { TRISFbits.TRISF7 = 1; } while(0)
#define RF7_SetDigitalOutput()   do { TRISFbits.TRISF7 = 0; } while(0)
#define RF7_SetPullup()             do { WPUFbits.WPUF7 = 1; } while(0)
#define RF7_ResetPullup()           do { WPUFbits.WPUF7 = 0; } while(0)
#define RF7_SetAnalogMode()         do { ANSELFbits.ANSELF7 = 1; } while(0)
#define RF7_SetDigitalMode()        do { ANSELFbits.ANSELF7 = 0; } while(0)

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