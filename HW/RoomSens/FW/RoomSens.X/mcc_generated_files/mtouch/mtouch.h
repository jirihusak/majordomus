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
#ifndef MTOUCH_H
#define MTOUCH_H

#include <stdint.h>
#include <stdbool.h>
#include "mtouch_config.h"
#include "mtouch_sensor.h"
#include "mtouch_button.h"

#if (__XC8_VERSION < 1420)
    #error  Please update XC8 compiler to version 1.42 or higher. 
#endif

/*
 * =======================================================================
 * Global Functions :: mTouch Library API
 * =======================================================================
 */
    void    MTOUCH_Initialize               (void);
    bool    MTOUCH_Service_Mainloop         (void);
    void    MTOUCH_Tick                     (void);
    bool    MTOUCH_Service_isInProgress     (void);
    void    MTOUCH_requestInitSet           (void);
    bool    MTOUCH_requestInitGet           (void);
/*
 * =======================================================================
 * mTouch Library :: State API
 * =======================================================================
 * To get state information on mTouch sensors, there are two options:
 *
 * 1. Provide callback functions defined in your
 *    own application's files. To do this:
 *
 *         Use the 'Set Callback' API to provide the function pointer
 *         to your application's function:
 *          void MTOUCH_Button_SetPressedCallback   (void (*callback)(enum mtouch_button_names button));
 *          void MTOUCH_Button_SetNotPressedCallback(void (*callback)(enum mtouch_button_names button));
 *
 *  2. Alternatively, you may choose to directly poll the status of the
 *     mTouch state machine by using the appropriate API. To do this:
 *
 *          Use the appropriate API in your application:
 *          bool MTOUCH_Button_isPressed     (enum mtouch_button_names    button);
 */

 /*
 * =======================================================================
 * Firmware and product ID
 * =======================================================================
 */
    
#define ID_PRODUCT                  (uint32_t)0x00000000
#define ID_MANUFACTURER             (uint16_t)0x0001
#define ID_BUILD                    (uint8_t)0
#define ID_MINOR                    (uint8_t)70
#define ID_MAJOR                    (uint8_t)2

#endif
