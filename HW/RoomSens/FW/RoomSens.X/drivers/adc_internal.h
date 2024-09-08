/**
  ADCC Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    adcc.h

  @Summary
    This is the generated header file for the ADCC driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  @Description
    This header file provides APIs for driver for ADCC.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC18F57K42
        Driver Version    :  2.1.4
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.30 and above
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

#ifndef ADC_Internal_H
#define ADC_Internal_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "../mcc_generated_files/adcc.h"

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
  Section: Data Types Definitions
*/

/**
 *  result size of an A/D conversion
 */

typedef uint16_t adc_result_t;
#ifndef uint24_t
typedef __uint24 uint24_t;
#endif

/** ADCC Channel Definition

 @Summary
   Defines the channels available for conversion.

 @Description
   This routine defines the channels that are available for the module to use.

 Remarks:
   None
 */

//typedef enum
//{
//    ADC_V_IN =  0x18,
//    ADC_V_IN_IO =  0x19,
//    ADC_0 =  0x21,
//    ADC_1 =  0x22,
//    channel_VSS =  0x3B,
//    channel_Temp =  0x3C,
//    channel_DAC1 =  0x3D,
//    channel_FVR_Buffer1 =  0x3E,
//    channel_FVR_Buffer2 =  0x3F
//} adcc_channel_t;

/**
  Section: ADCC Module APIs
*/

/**
  @Summary
    Initializes the ADCC.

  @Description
    This routine initializes the ADCC and must be called before any other ADCC routine.
    This routine should only be called once during system initialization.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    

  @Example
    <code>
    adc_result_t convertedValue;    

    ADC_Internal_Initialize();
    convertedValue = ADC_Internal_GetSingleConversion(channel_ANA0);
    </code>
*/
void ADC_Internal_Initialize(void);

/**
  @Summary
    Starts A/D conversion on selected analog channel.

  @Description
    This routine is used to trigger A/D conversion on selected analog channel.
    
  @Preconditions
    ADC_Internal_Initialize() function should have been called before calling this function.

  @Returns
    None

  @Param
    channel: Analog channel number on which A/D conversion has to be applied.
             For available analog channels refer adcc_channel_t enum from adcc.h file

  @Example
    <code>
    adc_result_t convertedValue; 

    ADC_Internal_Initialize();   
    ADC_Internal_StartConversion(channel_ANA0);
    while(!ADC_Internal_IsConversionDone());
    convertedValue = ADC_Internal_GetConversionResult();
    </code>
*/
void ADC_Internal_StartConversion(adcc_channel_t channel);

/**
  @Summary
    Determine if A/D conversion is completed.

  @Description
    This routine is used to determine if A/D conversion is completed.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_StartConversion(adcc_channel_t channel)
    functions should have been called before calling this function.

  @Returns
    true  - If conversion is completed
    false - If conversion is not completed

  @Param
    None

  @Example
    <code>
    adc_result_t convertedValue;    

    ADC_Internal_Initialize();    
    ADC_Internal_StartConversion(channel_ANA0);
    while(!ADC_Internal_IsConversionDone());
    convertedValue = ADC_Internal_GetConversionResult();
    </code>
 */
bool ADC_Internal_IsConversionDone(void);

/**
  @Summary
    Returns result of latest A/D conversion.

  @Description
    This routine is used to retrieve the result of latest A/D conversion.
    This routine returns the conversion value only after the conversion is complete.
    

  @Preconditions
    ADC_Internal_Initialize(), ADC_Internal_StartConversion() functions should have been called
    before calling this function.
    Completion status should be checked using ADC_Internal_IsConversionDone() routine.

  @Returns
    Returns the result of A/D conversion.

  @Param
    None

  @Example
    <code>
    adc_result_t convertedValue;

    ADC_Internal_Initialize();    
    ADC_Internal_StartConversion(channel_ANA0);
    while(!ADC_Internal_IsConversionDone());
    convertedValue = ADC_Internal_GetConversionResult();
    </code>
 */
adc_result_t ADC_Internal_GetConversionResult(void);

/**
  @Summary
    Returns the result of A/D conversion for requested analog channel.

  @Description
    This routine is used to retrieve the result of A/D conversion for requested 
    analog channel.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_DisableContinuousConversion() functions should have 
    been called before calling this function.

  @Returns
    Returns the result of A/D conversion.

  @Param
    channel: Analog channel number for which A/D conversion has to be applied
             For available analog channels refer adcc_channel_t enum from adcc.h file

  @Example
    <code>
    adcc_channel_t convertedValue;

    ADC_Internal_Initialize();
    ADC_Internal_DisableContinuousConversion();
    
    convertedValue = ADC_Internal_GetSingleConversion(channel_ANA0);
    </code>
*/
adc_result_t ADC_Internal_GetSingleConversion(adcc_channel_t channel);

/**
  @Summary
    Stops the ongoing continuous A/D conversion.

  @Description
    This routine is used to stop ongoing continuous A/D conversion.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_StartConversion() functions should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    ADC_Internal_StopConversion();
    </code>
*/
void ADC_Internal_StopConversion(void);

/**
  @Summary
    Stops the ADCC from re-triggering A/D conversion cycle 
    upon completion of each conversion.

  @Description
    In continuous mode, stops the ADCC from re-triggering A/D conversion cycle 
    upon completion of each conversion.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_EnableContinuousConversion() function should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_EnableContinuousConversion();
    ADC_Internal_SetStopOnInterrupt();
    </code>
*/
void ADC_Internal_SetStopOnInterrupt(void);

/**
  @Summary
    Discharges the input sample capacitor by setting the channel to AVss.

  @Description
    This routine is used to discharge input sample capacitor by selecting analog
    ground (AVss) channel.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_DischargeSampleCapacitor();
    </code>
*/
void ADC_Internal_DischargeSampleCapacitor(void); 

/**
  @Summary
    Loads the Acquisition Time Control register.

  @Description
    This routine is used to load 13-bit ADCC Acquisition Time Control register by
    a value provided by user.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    13-bit value to be set in the acquisition register.

  @Example
    <code>
    uint16_t acquisitionValue = 98;
    ADC_Internal_Initialize();
    ADC_Internal_LoadAcquisitionRegister(acquisitionValue);
    </code>
*/
void ADC_Internal_LoadAcquisitionRegister(uint16_t);

/**
  @Summary
    Loads the Precharge Time Control register.

  @Description
    This routine is used to load 13-bit ADCC Precharge Time Control register by
    a value provided by user.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    13-bit value to be set in the precharge register.

  @Example
    <code>
    uint16_t prechargeTime = 98;
    ADC_Internal_Initialize();
    ADC_Internal_SetPrechargeTime(prechargeTime);
    </code>
*/
void ADC_Internal_SetPrechargeTime(uint16_t);

/**
  @Summary
    Loads the Repeat Setting register.

  @Description
    This routine loads ADCC Repeat Setting register with 8-bit value provided by user.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    8-bit value to be set in the Repeat Setting register.

  @Example
    <code>
    uint8_t repeat = 98;
    ADC_Internal_Initialize();
    ADC_Internal_SetRepeatCount(repeat);
    </code>
*/
void ADC_Internal_SetRepeatCount(uint8_t);

/**
  @Summary
    Returns the current value of Repeat Count register.

  @Description
    This routine retrieves the current value of ADCC Repeat Count register.

  @Preconditions
    ADC_Internal_Initialize(), ADC_Internal_StartConversion() should have been called before calling
    this function.

  @Returns
    Value of ADCC Repeat Count register

  @Param
    None.

  @Example
    <code>
    adc_result_t convertedValue;
    uint8_t count;
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    count = ADC_Internal_GetCurrentCountofConversions();
    </code>
*/
uint8_t ADC_Internal_GetCurrentCountofConversions(void);

/**
  @Summary
    Clears the A/D Accumulator.

  @Description
    This routine is used to clear A/D Accumulator

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    None.

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_ClearAccumulator();
    </code>
*/
void ADC_Internal_ClearAccumulator(void);

/**
  @Summary
   Returns the value of ADCC Accumulator.

  @Description
    This routine is is to retrieve the 17-bit value of ADCC accumulator.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    17-bit value obtained from ADCC Accumulator register.

  @Param
    None.

  @Example
    <code>
    uint24_t accumulatorValue;
    ADC_Internal_Initialize();
    accumulatorValue = ADC_Internal_GetAccumulatorValue();
    </code>
*/
uint24_t ADC_Internal_GetAccumulatorValue(void);

/**
  @Summary
   Determines if ADCC accumulator has overflowed.

  @Description
    This routine is used to determine whether ADCC accumulator has overflowed.

  @Preconditions  
    ADC_Internal_Initialize(), ADC_Internal_StartConversion() should have been called before calling
    this function.

  @Returns
    1: ADCC accumulator or ERR calculation have overflowed
    0: ADCC accumulator and ERR calculation have not overflowed

  @Param
    None.

  @Example
    <code>
    bool accumulatorOverflow;    
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion();
    accumulatorOverflow = ADC_Internal_HasAccumulatorOverflowed();
    </code>
*/
bool ADC_Internal_HasAccumulatorOverflowed(void);

/**
  @Summary
   Returns the value of ADCC Filter register.

  @Description
    This routine is used to retrieve value of ADCC Filter register.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    16-bit value obtained from ADFLTRH and ADFLTRL registers.

  @Param
    None.

  @Example
    <code>
    uint16_t filterValue;
    ADC_Internal_Initialize();
    filterValue = ADC_Internal_GetFilterValue();
    </code>
*/
uint16_t ADC_Internal_GetFilterValue(void);

/**
  @Summary
   Returns the value of ADCC Previous Result register.

  @Description
    This routine is used to retrieve value of ADCC Previous register.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_StartConversion() should have been called before
    calling this function.

  @Returns
    16-bit value obtained from ADPREVH and ADPREVL registers.

  @Param
    None.

  @Example
    <code>
    uint16_t prevResult, convertedValue;
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    convertedValue = ADC_Internal_GetConversionResult();
    prevResult = ADC_Internal_GetPreviousResult();
    </code>
*/
uint16_t ADC_Internal_GetPreviousResult(void);

/**
  @Summary
   Sets the ADCC Threshold Set-point.

  @Description
    This routine is used to set value of ADCC Threshold Set-point.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    16-bit value for set point.

  @Example
    <code>
    uint16_t setPoint = 90;
    ADC_Internal_Initialize();
    ADC_Internal_DefineSetPoint(setPoint);
    ADC_Internal_StartConversion(channel_ANA0);
    </code>
*/
void ADC_Internal_DefineSetPoint(uint16_t);

/**
  @Summary
   Sets the value of ADCC Upper Threshold.

  @Description
    This routine is used to set value of ADCC Upper Threshold register.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    16-bit value for upper threshold.

  @Example
    <code>
        uint16_t upperThreshold = 90;
        ADC_Internal_Initialize();
        ADC_Internal_SetUpperThreshold(upperThreshold);
        ADC_Internal_StartConversion(channel_ANA0);
    </code>
*/
void ADC_Internal_SetUpperThreshold(uint16_t);

/**
  @Summary
   Sets the value of ADCC Lower Threshold.

  @Description
    This routine is used to set value of ADCC Lower Threshold register.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    16- bit value for lower threshold.

  @Example
    <code>
    uint16_t lowerThreshold = 90;
    ADC_Internal_Initialize();
    ADC_Internal_SetLowerThreshold(lowerThreshold);
    ADC_Internal_StartConversion(channel_ANA0);    
    </code>
*/
void ADC_Internal_SetLowerThreshold(uint16_t);

/**
  @Summary
   Returns the value of ADCC Set-point Error.

  @Description
    This routine retrieves the value of ADCC Set-point Error register.

  @Preconditions
    ADC_Internal_Initialize(), ADC_Internal_StartConversion() should have been called before calling
    this function.

  @Returns
    16-bit value obtained from ADERRH and ADERRL registers.

  @Param
    None.

  @Example
    <code>
    uint16_t error;
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    error = ADC_Internal_GetErrorCalculation(void);
    </code>
*/
uint16_t ADC_Internal_GetErrorCalculation(void);

/**
  @Summary
   Enables Double-Sampling.

  @Description
    This routine is used to enable double-sampling bit.
    Two conversions are performed on each trigger. Data from the first conversion 
    appears in PREV.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    None.

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_EnableDoubleSampling();    
    ADC_Internal_StartConversion(channel_ANA0);
    </code>
*/
void ADC_Internal_EnableDoubleSampling(void);

/**
  @Summary
   Enables continuous A/D conversion.

  @Description
    This routine is used to enable continuous A/D conversion.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    None.

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_EnableContinuousConversion();
    </code>
*/
void ADC_Internal_EnableContinuousConversion(void);

/**
  @Summary
   Disables continuous A/D conversion.

  @Description
    This routine is used to disable continuous A/D conversion.

  @Preconditions
    ADC_Internal_Initialize() should have been called before calling this function.

  @Returns
    None

  @Param
    None.

  @Example
    <code>
    ADC_Internal_Initialize();
    ADC_Internal_DisableContinuousConversion();
    </code>
*/
void ADC_Internal_DisableContinuousConversion(void);

/**
  @Summary
   Determines if ADCC ERR crosses upper threshold.

  @Description
    This routine is used to determine if ADCC ERR has crossed the upper threshold.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_StartConversion() should have been called 
    before calling this function.

  @Returns
    1: if ERR > UTH
    0: if ERR <= UTH

  @Param
    None.

  @Example
    <code>
    bool uThr;
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    uThr = ADC_Internal_HasErrorCrossedUpperThreshold();
    </code>
*/
bool ADC_Internal_HasErrorCrossedUpperThreshold(void);

/**
  @Summary
   Determines if ADCC ERR is less than lower threshold.

  @Description
    This routine is used to determine if ADCC ERR is less than the lower threshold.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_StartConversion() should have been called 
    before calling this function.

  @Returns
    1: if ERR < LTH
    0: if ERR >= LTH

  @Param
    None.

  @Example
    <code>
    bool lThr;
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    lThr = ADC_Internal_HasErrorCrossedLowerThreshold();
    </code>
*/
bool ADC_Internal_HasErrorCrossedLowerThreshold(void);

/**
  @Summary
   Returns Status of ADCC

  @Description
    This routine is used to retrieve contents of ADCC status register.

  @Preconditions
    ADC_Internal_Initialize() and ADC_Internal_StartConversion() should have been called 
    before calling this function.

  @Returns
    Returns the contents of ADCC STATUS register

  @Param
    None.

  @Example
    <code>
    uint8_t adccStatus;
    ADC_Internal_Initialize();
    ADC_Internal_StartConversion(channel_ANA0);
    adccStatus = ADC_Internal_GetConversionStageStatus();
    </code>
*/
uint8_t ADC_Internal_GetConversionStageStatus(void);


uint16_t ADC_Internal_Get_Millivolts(adcc_channel_t channel);

void ADC_Internal_Disable();


#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif    //ADC_Internal_H
/**
 End of File
*/

