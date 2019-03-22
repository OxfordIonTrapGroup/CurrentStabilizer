#include "Compiler.h"
#include "adc.h"
#include "delay.h"

uint16_t adcVal;


void adc_init()
{
    ANSELBbits.ANSB0 = 1; // Set AN2 = RB0 as analog input
    
    AD1CON1 = 0;
    
    AD1CHS = 0x20000; // Sample AN2
    
    AD1CON3 = 2; // Tad = Tpb/8 = 200ns
    
    AD1CON1SET = 0x8000; // Turn on ADC
    
    volatile int i; for(i=0;i<10000;i++);
    AD1CON1SET = 0x2; // Start sampling
}


void adc_poll()
{
    //AD1CON1SET = 0x2; // Start sampling
    //volatile int i;
    //
    // Trigger ADC conversion
    AD1CON1CLR = 0x2;
    
    //while(!(AD1CON1 & 0x1)); // Wait for conversion
    adcVal = ADC1BUF0;
    
    AD1CON1SET = 0x2; // Start sampling
}
