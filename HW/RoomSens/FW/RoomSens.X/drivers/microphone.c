
#include "microphone.h"
#include <stdio.h>
#include <stdlib.h>
#include <pic18lf57k42.h>
#include "../mcc_generated_files/tmr2.h"
#include "../mcc_generated_files/mcc.h"
#include "tick.h"

static enum {
    MIC_INIT,
    MIC_START,
    MIC_WAIT_FOR_VALID,
    MIC_SAMPLING,
    MIC_STOP,
    MIC_WAIT,
    MIC_IDLE
} state = MIC_INIT;
static uint32_t fsm_timer = 0;

// variables for PDM -> PCM conversion
static uint8_t winFlag = 0;
static int16_t winval = 0;
static int16_t sum1 = 0;   // sumarize PDM signal counter of 1
static int16_t sum2 = 0;   // integrate sum1

static int16_t s2_comb1_1 = 0;
static int16_t s2_comb1_2 = 0;

static int16_t s2_comb2_1 = 0;
static int16_t s2_comb2_2 = 0;
static uint16_t n = 0;
static int32_t sabs = 0;
static int16_t avg = 0;
static int32_t sum = 0;

#define WINDOWSIZE 32			// 32us * 64

#define SAMPLES_MAX 40
static uint16_t samples[SAMPLES_MAX];
static uint16_t counter = 0;
static int32_t samplesAvg = 0;

// this ISR is called every 32us and SMT timer has result
void microphone_sample_isr()
{
    sum1 += SMT1CPR - 128;
    sum2 += sum1;
    winFlag++;
    
    if(winFlag == 3)
    {
        winFlag = 0;
            
        // CIC filter
        // http://chiselapp.com/user/rberteig/repository/PDMStunts/file?name=ATTiny/MicDemo.c&ci=tip
        // https://curiouser.cheshireeng.com/2015/01/21/pdm-in-attiny85-source-code/
        int tmp1;//, tmp2;
        tmp1 = sum2 - s2_comb1_2;
        s2_comb1_2 = s2_comb1_1;
        s2_comb1_1 = sum2;
        int16_t v = tmp1 - s2_comb2_2;
        s2_comb2_2 = s2_comb2_1;
        s2_comb2_1 = tmp1;

        // Compute an average of this window to use to remove DC
        // offset from the next window.
        sum += v;

        // Take the absolute value for our RMS estimate based on
        // mean absolute value, after removing the DC offset based
        // on the previous window's average sample.
        v -= avg;
        if (v < 0) { v = -v; }
        sabs += v;

        // Count samples in this window
        ++n;
        
        if (n == WINDOWSIZE) {
            n = 0;
            // Once per RMS window, based on WINDOWSIZE samples at
            // 1 MHz / 128 = 7812.5 Hz sample rate.

            // Compute the mean sample value.
            //avg = sum / WINDOWSIZE;
            avg = sum >> 5; 
            sum = 0;
            
            if(counter < SAMPLES_MAX) {
                samples[counter] = sabs;
                counter++;
            }

            sabs = 0;
        }
        
    }    
}

void microphone_task()
{
    switch(state)
    {
        case MIC_INIT:
            //SPI1_SetTxInterruptHandler(&microphone_rx_isr);
            SMT1_SetInterruptHandler(&microphone_sample_isr);
            state = MIC_START;
            break;
        case MIC_START:
            fsm_timer = tick_getTick();
            state = MIC_WAIT_FOR_VALID;
            PWM6_LoadDutyValue(23);
            CCPTMRS1bits.P6TSEL = 1;
            TMR2_StartTimer();
            break;
        case MIC_WAIT_FOR_VALID:
            if((tick_getTick() - fsm_timer) > 10*TICK_MILISECOND)
            {
                state = MIC_SAMPLING;
                counter = 0;
                SMT1_DataAcquisitionEnable();
            }
        case MIC_SAMPLING:
            if((tick_getTick() - fsm_timer) > 50*TICK_MILISECOND)
            {
                state = MIC_STOP;
            }
            break;
        case MIC_STOP:
            PWM6_LoadDutyValue(0);
            TMR2_StopTimer();
            SMT1_DataAcquisitionDisable();
            samplesAvg = 0;
            uint16_t avgNr = 0;
            for(int i = 8; i < counter; i++) {
                //printf("%d ", samples[i]);
                samplesAvg += samples[i];
                avgNr++;
            }
            samplesAvg = samplesAvg/avgNr;
            //printf("$%d %d;\r\n", samplesAvg, avgNr);
            fsm_timer = tick_getTick();
            
            state = MIC_WAIT;
            break;
        case MIC_WAIT:
            if((tick_getTick() - fsm_timer) > 1*TICK_MILISECOND)
            {
                fsm_timer = tick_getTick();
                state = MIC_IDLE;
            }
            break;
        case MIC_IDLE:
            if((tick_getTick() - fsm_timer) > 50*TICK_MILISECOND)
            {
                state = MIC_START;
            }
            break;
    }
}

uint32_t microphone_get_noise_level()
{
    return samplesAvg;
}

uint8_t microphone_is_sampling()
{
    if(state == MIC_IDLE)
        return 0;
    else 
        return 1;
}