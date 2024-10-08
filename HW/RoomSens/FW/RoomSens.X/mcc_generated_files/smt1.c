/**
  SMT1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    smt1.c

  @Summary
    This is the generated driver implementation file for the SMT1 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for SMT1.
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "smt1.h"
#include "interrupt_manager.h"


/**
  Section: SMT1 APIs
*/

void SMT1_Initialize(void)
{
    // WPOL high/rising edge enabled; SMT1STP rolls over to 24'h000000; SMT1SPOL high/rising edge enabled; SMT1EN enabled; SMT1PS 1:8 Prescaler; SMT1CPOL rising edge; 
    SMT1CON0 = 0x83;

    // SMT1REPEAT Repeat Data Acquisition; SMT1MODE Gated windowed measure; SMT1GO disabled; 
    SMT1CON1 = 0x45;

    // SMT1CPWUP SMT1CPW1 update complete; SMT1CPRUP SMT1PR1 update complete; SMT1RST SMT1TMR1 update complete; 
    SMT1STAT = 0x00;

    // SMT1CSEL FOSC; 
    SMT1CLK = 0x01;

    // SMT1WSEL LFINTOSC; 
    SMT1WIN = 0x01;

    // SMT1SSEL CLC3OUT; 
    SMT1SIG = 0x18;

    // SMT1PR 0; 
    SMT1PRU = 0x00;

    // SMT1PR 255; 
    SMT1PRH = 0xFF;

    // SMT1PR 255; 
    SMT1PRL = 0xFF;

    // Enabling SMT1 period acquisition interrupt.
    PIE1bits.SMT1PRAIE = 1;

}
        
void SMT1_DataAcquisitionEnable(void)
{
    // Start the SMT module by writing to SMTxGO bit
    SMT1CON1bits.SMT1GO = 1;
}

void SMT1_DataAcquisitionDisable(void)
{
    // Start the SMT module by writing to SMTxGO bit
    SMT1CON1bits.SMT1GO = 0;
}

void SMT1_HaltCounter(void)
{
    SMT1CON0bits.SMT1STP = 1;
}

void SMT1_SetPeriod(uint32_t periodVal)
{
    // Write to the SMT1 Period registers
    SMT1PRU = (periodVal >> 16);
    SMT1PRH = (periodVal >> 8);
    SMT1PRL = periodVal;
}

uint32_t SMT1_GetPeriod()
{
    return (SMT1PR);
}

void SMT1_SingleDataAcquisition(void)
{
    SMT1CON1bits.SMT1REPEAT = 0;
}

void SMT1_RepeatDataAcquisition(void)
{
    SMT1CON1bits.SMT1REPEAT = 1;
}

void SMT1_ManualPeriodBufferUpdate(void)
{
    SMT1STATbits.SMT1CPRUP = 1;
}

void SMT1_ManualPulseWidthBufferUpdate(void)
{
    SMT1STATbits.SMT1CPWUP = 1;
}

void SMT1_ManualTimerReset(void)
{
    SMT1STATbits.SMT1RST = 1;
}

bool SMT1_IsWindowOpen(void)
{
    return (SMT1STATbits.SMT1WS);
}

bool SMT1_IsSignalAcquisitionInProgress(void)
{
    return (SMT1STATbits.SMT1AS);
}

bool SMT1_IsTimerIncrementing(void)
{
    return (SMT1STATbits.SMT1TS);
}

uint32_t SMT1_GetCapturedPulseWidth()
{
    return (SMT1CPW);
}

uint32_t SMT1_GetCapturedPeriod()
{
    return (SMT1CPR);
}

uint32_t SMT1_GetTimerValue()
{
    return (SMT1TMR);
}

void (*SMT1_InterruptHandler)(void);
void SMT1_SetInterruptHandler(void (* InterruptHandler)(void)){
    SMT1_InterruptHandler = InterruptHandler;
}

void __interrupt(irq(SMT1PRA),base(24584)) SMT1_PR_ACQ_ISR()
{
    
    // Disabling SMT1 period acquisition interrupt flag bit.
    PIR1bits.SMT1PRAIF = 0;
    if(SMT1_InterruptHandler)
    {
        SMT1_InterruptHandler();
    }
    
}
/**
 End of File
*/
