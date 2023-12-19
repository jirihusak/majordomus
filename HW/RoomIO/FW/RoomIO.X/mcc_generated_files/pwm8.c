 /**
   PWM8 Generated Driver File
 
   @Company
     Microchip Technology Inc.
 
   @File Name
     pwm8.c
 
   @Summary
     This is the generated driver implementation file for the PWM8 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs
 
   @Description
     This source file provides implementations for driver APIs for PWM8.
     Generation Information :
         Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
         Device            :  PIC18LF57K42
         Driver Version    :  2.01
     The generated drivers are tested against the following:
         Compiler          :  XC8 2.30 and above or later
         MPLAB             :  MPLAB X 5.40
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
 
 /**
   Section: Included Files
 */

 #include <xc.h>
 #include "pwm8.h"

 /**
   Section: PWM Module APIs
 */

 void PWM8_Initialize(void)
 {
    // Set the PWM to the options selected in the PIC10 / PIC12 / PIC16 / PIC18 MCUs.
    // PWM8POL active_hi; PWM8EN enabled; 
    PWM8CON = 0x80;   

    // DC 0; 
    PWM8DCH = 0x00;   

    // DC 0; 
    PWM8DCL = 0x00;   

    // Select timer
    CCPTMRS1bits.P8TSEL = 2;
 }

 void PWM8_LoadDutyValue(uint16_t dutyValue)
 {
     // Writing to 8 MSBs of PWM duty cycle in PWMDCH register
     PWM8DCH = (dutyValue & 0x03FC)>>2;
     
     // Writing to 2 LSBs of PWM duty cycle in PWMDCL register
     PWM8DCL = (dutyValue & 0x0003)<<6;
 }
 /**
  End of File
 */
