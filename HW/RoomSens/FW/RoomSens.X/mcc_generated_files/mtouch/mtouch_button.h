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
#ifndef MTOUCH_BUTTON_H
#define MTOUCH_BUTTON_H
    
    #include <stdint.h>
    #include <stdbool.h>
    #include "mtouch_config.h"
    #include "mtouch.h"
    
/*
 * =======================================================================
 * Application / Configuration Settings
 * =======================================================================
 */
 
    enum mtouch_button_names
    {
        BTN_PLUS = 0,
        BTN_ENTER = 1,
        BTN_MINUS = 2
    };
    
/*
 * =======================================================================
 * Typedefs / Data Types
 * =======================================================================
 */
    typedef uint8_t mtouch_buttonmask_t;
    
    typedef uint16_t mtouch_button_statecounter_t;
    #define MTOUCH_BUTTON_STATECOUNTER_MIN (0)
    #define MTOUCH_BUTTON_STATECOUNTER_MAX (UINT16_MAX)

    typedef uint16_t mtouch_button_baselinecounter_t;
    #define MTOUCH_BUTTON_BASECOUNTER_MIN (0)
    #define MTOUCH_BUTTON_BASECOUNTER_MAX (UINT16_MAX)
    
    typedef uint16_t mtouch_button_reading_t;
    #define MTOUCH_BUTTON_READING_MIN (0)
    #define MTOUCH_BUTTON_READING_MAX (UINT16_MAX)
    
    
    typedef uint32_t mtouch_button_baseline_t;
    #define MTOUCH_BUTTON_BASELINE_MIN (0)
    #define MTOUCH_BUTTON_BASELINE_MAX (UINT32_MAX)

    
    typedef int8_t mtouch_button_deviation_t;
    #define MTOUCH_BUTTON_DEVIATION_MIN (INT8_MIN)
    #define MTOUCH_BUTTON_DEVIATION_MAX (INT8_MAX)

    typedef uint8_t mtouch_button_threshold_t;
    #define MTOUCH_BUTTON_THRESHOLD_MIN ((mtouch_button_threshold_t)(1))
    #define MTOUCH_BUTTON_THRESHOLD_MAX ((mtouch_button_threshold_t)(127))

    typedef uint8_t mtouch_button_scaling_t;
    #define MTOUCH_BUTTON_SCALING_MIN ((mtouch_button_scaling_t)(0))
    #define MTOUCH_BUTTON_SCALING_MAX ((mtouch_button_scaling_t)(8))

    


/*
 * =======================================================================
 * Global Functions
 * =======================================================================
 */
    
    void                        MTOUCH_Button_Initialize    (enum mtouch_button_names button);
    void                        MTOUCH_Button_InitializeAll (void);
    void                        MTOUCH_Button_ServiceAll    (void);
    mtouch_button_deviation_t   MTOUCH_Button_Deviation_Get (enum mtouch_button_names button);
    mtouch_button_reading_t     MTOUCH_Button_Reading_Get   (enum mtouch_button_names button);
    mtouch_button_reading_t     MTOUCH_Button_Baseline_Get  (enum mtouch_button_names button);
    mtouch_button_scaling_t     MTOUCH_Button_Scaling_Get   (enum mtouch_button_names button);
    void                        MTOUCH_Button_Scaling_Set   (enum mtouch_button_names button,mtouch_button_scaling_t scaling);
    mtouch_button_threshold_t   MTOUCH_Button_Threshold_Get (enum mtouch_button_names button);
    void                        MTOUCH_Button_Threshold_Set (enum mtouch_button_names button,mtouch_button_threshold_t threshold);
    uint8_t                     MTOUCH_Button_Oversampling_Get(enum mtouch_button_names button);
    void                        MTOUCH_Button_Oversampling_Set(enum mtouch_button_names button,uint8_t oversampling);
    
    bool                        MTOUCH_Button_isPressed     (enum mtouch_button_names button);
    bool                        MTOUCH_Button_isInitialized (enum mtouch_button_names button);
    mtouch_buttonmask_t         MTOUCH_Button_Buttonmask_Get(void);
    uint8_t                     MTOUCH_Button_State_Get     (enum mtouch_button_names button);
#endif // MTOUCH_BUTTON_H
