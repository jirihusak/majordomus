/*
    MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:

    You may use this software, and any derivatives created by any person or
    entity by or on your behalf, exclusively with Microchip's products.
    Microchip and its subsidiaries ("Microchip"), and its licensors, retain all
    ownership and intellectual property rights in the accompanying software and
    in all derivatives hereto.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products.  You agree
    that you are solely responsible for testing the software and determining
    its suitability.  Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH MICROCHIP'S
    PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
    (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY,
    INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF
    ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
    FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL
    LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
    THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
    THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF
    THESE TERMS.
*/
#ifndef MTOUCH_SENSOR_H
#define MTOUCH_SENSOR_H
  
    #include <stdint.h>
    #include <stdbool.h>
        

    
    
    enum mtouch_sensor_names
    {
        Sensor_ANF7 = 0,
        Sensor_ANF6 = 1,
        Sensor_ANF5 = 2
    };
    
    enum mtouch_sensor_error
    {
        MTOUCH_SENSOR_ERROR_none            =  0,
        MTOUCH_SENSOR_ERROR_invalid_index   = -1,
        MTOUCH_SENSOR_ERROR_interrupt_notEnabled = -2,
        MTOUCH_SENSOR_ERROR_invalid_calibrate= -3,
        MTOUCH_SENSOR_ERROR_tooManyRetries = -4,
        MTOUCH_SENSOR_ERROR_scanOverrun = -5,
        MTOUCH_SENSOR_ERROR_interruptedScan = -6
    };

/*
 * =======================================================================
 * Typedefs / Data Types
 * =======================================================================
 */
    typedef uint16_t mtouch_sensor_sample_t;
    #define MTOUCH_SENSOR_SAMPLE_MIN (0)
    #define MTOUCH_SENSOR_SAMPLE_MAX (UINT16_MAX)

    typedef uint8_t mtouch_sensor_mask_t;
    #define MTOUCH_SENSOR_MASK_MIN (0)
    #define MTOUCH_SENSOR_MASK_MAX (UINT8_MAX)

/*
 * =======================================================================
 * Global Functions
 * =======================================================================
 */
    
    enum mtouch_sensor_error    MTOUCH_Sensor_Initialize        (enum mtouch_sensor_names sensor);
    void                        MTOUCH_Sensor_Scan_Initialize   (void);
    void                        MTOUCH_Sensor_InitializeAll     (void);
    void                        MTOUCH_Sensor_SetSampledCallback(void (*callback)(enum mtouch_sensor_names sensor));
    bool                        MTOUCH_Sensor_SampleAll         (void);
    bool                        MTOUCH_Sensor_isSamplingComplete(void);
    void                        MTOUCH_Sensor_postAcquisitionProcess(void);
    void                        MTOUCH_Sensor_startLowpower     (void);
    void                        MTOUCH_Sensor_exitLowpower      (void);
    
    void                        MTOUCH_Sensor_ADCC_Initialize   (void);
    void                        ADCC_ThresholdISR_mTouch        (void);
    bool                        MTOUCH_Sensor_isSampling        (void);
    bool                        MTOUCH_Sensor_isAnySensorActive (void);
    bool                        MTOUCH_Sensor_isActive          (enum mtouch_sensor_names sensor);
    bool                        MTOUCH_Sensor_wasSampled        (enum mtouch_sensor_names sensor);
    bool                        MTOUCH_Sensor_isCalibrated      (enum mtouch_sensor_names sensor);
    bool                        MTOUCH_Sensor_isEnabled         (enum mtouch_sensor_names sensor);
    void                        MTOUCH_Sensor_Sampled_ResetAll  (void);
    void                        MTOUCH_Sensor_Disable           (enum mtouch_sensor_names sensor);
    void                        MTOUCH_Sensor_Enable            (enum mtouch_sensor_names sensor);
    void                        MTOUCH_Sensor_Calibrate         (enum mtouch_sensor_names sensor); 
    mtouch_sensor_sample_t      MTOUCH_Sensor_RawSample_Get     (enum mtouch_sensor_names sensor);  
    mtouch_sensor_sample_t      MTOUCH_Sensor_CVDSample_Get     (enum mtouch_sensor_names sensor);
    uint8_t                     MTOUCH_Sensor_AdditionalCap_Get (enum mtouch_sensor_names sensor); 
    uint8_t                     MTOUCH_Sensor_AcquisitionTime_Get(enum mtouch_sensor_names sensor); 
    uint8_t                     MTOUCH_Sensor_PreChargeTime_Get (enum mtouch_sensor_names sensor); 
    uint8_t                     MTOUCH_Sensor_Oversampling_Get  (enum mtouch_sensor_names name);
    void                        MTOUCH_Sensor_Oversampling_Set (enum mtouch_sensor_names name, uint8_t value);
    
    
    void                        MTOUCH_Sensor_NotifyInterruptOccurred(void);
#endif