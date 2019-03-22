#include <proc/p32mx250f128b.h>

#include "Compiler.h"
#include <stdint.h>

#include "feedforward.h"
#include "feedforwardCalculate.h"
#include "dac.h"


#define TMR_PERIOD_NOMINAL (40e6 / (50*nFeedforward))


// Set up timer 2 to give interrupts at ~6 kHz
void timer_init()
{
    PR2 = TMR_PERIOD_NOMINAL;
    TMR2 = 0;
       
    IFS0CLR = _IFS0_T2IF_MASK;
	IEC0SET = _IEC0_T2IE_MASK; // Enable timer2 interrupt
   
    IPC2bits.T2IP = 7; // Interrupt priority 7 (highest)
    
    T2CONbits.TON = 1;
}


void inputCapture_init()
{
    IC1R = 0b0100; // Input capture 1 module connected to RPB2 (line trigger)
    
    IC1CONbits.ICTMR = 1; // Connect to timer 2
    IC1CONbits.ICM = 0b011; // Capture every rising edge
    IC1CONbits.ON = 1;
}


void feedforward_init()
{
    timer_init();
    inputCapture_init();
    feedforward_reset();
}



int32_t currentPhase;
int32_t currentPeriod;

void __ISR (_TIMER_2_VECTOR, IPL7SOFT) Timer2Handler(void)
{
    static uint32_t count = 0;
    static int32_t integralCount = 0;
    int32_t periodCorrection;
    CS_FF = 1; // Trigger DAC write
    
    if(IC1CONbits.ICBNE) { // If a rising edge occurred in the last cycle
        // Implement the phase lock
        
        int32_t nSubCycle = IC1BUF; // Read the input event timer2 value
        int32_t nPhase = nSubCycle + count*TMR_PERIOD_NOMINAL; // The phase of the rising edge in ticks, where a full turn is nFeedforward*TMR_PERIOD_NOMINAL
        int32_t phaseError = nPhase - (((nFeedforward+1)*TMR_PERIOD_NOMINAL)/2); // When in lock this should be close to zero
        
        integralCount += phaseError;
        periodCorrection = integralCount/10000 + phaseError/500; 
        if(periodCorrection>100) periodCorrection=100;
        if(periodCorrection<-100) periodCorrection=-100;
        PR2 = TMR_PERIOD_NOMINAL + periodCorrection; // Slightly adjust timer2 period. 
        
        currentPhase = phaseError;
        currentPeriod = PR2;
    }
    
    count++;
    if(count == nFeedforward) count = 0;
    
    CS_FF = 0;
    dac_setFeedForward(feedforwardBuf[count]); // Load up DAC value for next cycle
    
    
    IFS0CLR = _IFS0_T2IF_MASK;
}


