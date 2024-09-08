/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC18LF57K42
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.30 and above
        MPLAB             :  MPLAB X 5.40

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.
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

#include "pin_manager.h"





void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATE = 0x00;
    LATD = 0x00;
    LATA = 0x00;
    LATF = 0x00;
    LATB = 0x00;
    LATC = 0x00;

    /**
    TRISx registers
    */
    TRISE = 0x06;
    TRISF = 0xE7;
    TRISA = 0xF7;
    TRISB = 0xC1;
    TRISC = 0x0A;
    TRISD = 0xE7;

    /**
    ANSELx registers
    */
    ANSELD = 0x63;
    ANSELC = 0x01;
    ANSELB = 0xC0;
    ANSELE = 0x06;
    ANSELF = 0x00;
    ANSELA = 0xC0;

    /**
    WPUx registers
    */
    WPUD = 0x00;
    WPUF = 0x07;
    WPUE = 0x00;
    WPUB = 0x00;
    WPUA = 0x00;
    WPUC = 0x0A;

    /**
    RxyI2C registers
    */
    RB1I2C = 0x00;
    RB2I2C = 0x00;
    RC3I2C = 0x00;
    RC4I2C = 0x00;
    RD0I2C = 0x00;
    RD1I2C = 0x00;

    /**
    ODx registers
    */
    ODCONE = 0x00;
    ODCONF = 0x00;
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;
    ODCOND = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0xFF;
    SLRCONB = 0xFF;
    SLRCONC = 0xFF;
    SLRCOND = 0xFF;
    SLRCONE = 0x07;
    SLRCONF = 0xFF;

    /**
    INLVLx registers
    */
    INLVLA = 0xFF;
    INLVLB = 0xFF;
    INLVLC = 0xFF;
    INLVLD = 0xFF;
    INLVLE = 0x0F;
    INLVLF = 0xFF;





   
    
	
    RC0PPS = 0x1E;   //RC0->SPI1:SCK1;    
    CLCIN3PPS = 0x08;   //RB0->CLC4:CLCIN3;    
    RC2PPS = 0x14;   //RC2->UART1:TXDE1;    
    U1RXPPS = 0x13;   //RC3->UART1:RX1;    
    RD2PPS = 0x0E;   //RD2->PWM6:PWM6;    
    SPI1SCKPPS = 0x10;   //RC0->SPI1:SCK1;    
    RF3PPS = 0x13;   //RF3->UART1:TX1;    
    CLCIN0PPS = 0x00;   //RA0->CLC3:CLCIN0;    
    RD4PPS = 0x10;   //RD4->PWM8:PWM8;    
    SMT1WINPPS = 0x10;   //RC0->SMT1:SMT1WIN;    
    RA3PPS = 0x0D;   //RA3->PWM5:PWM5;    
    RB2PPS = 0x04;   //RB2->CLC4:CLC4;    
    RB3PPS = 0x1F;   //RB3->SPI1:SDO1;    
    SMT1SIGPPS = 0x11;   //RC1->SMT1:SMT1SIG;    
    RC7PPS = 0x0F;   //RC7->PWM7:PWM7;    
    SPI1SDIPPS = 0x10;   //RC0->SPI1:SDI1;    
    U1CTSPPS = 0x10;   //RC0->UART1:CTS1;    
}
  
void PIN_MANAGER_IOC(void)
{   
}

/**
 End of File
*/