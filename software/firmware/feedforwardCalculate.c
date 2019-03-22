#include "feedforwardCalculate.h"


 

uint16_t feedforwardBuf[nFeedforward]; // This buffer contains the precalculated feed-forward signal
extern int16_t sineLookup[];

#define nHarmonics (5) // Number of harmonics to include in feedforward


// Amplitude for the n=1->nHarmonics sine and cos harmonics
int16_t sineAmplitude[nHarmonics];
int16_t cosAmplitude[nHarmonics];


void feedforward_setAmplitude(int harmonic, int component, int16_t amplitude)
{
    if(component==0) sineAmplitude[harmonic-1] = amplitude;
    else cosAmplitude[harmonic-1] = amplitude;
}


// Set all feed-forward amplitudes to zero
void feedforward_reset()
{
    int i;
    for(i=1; i<nHarmonics+1; i++) {
        feedforward_setAmplitude(i, 0, 0);
        feedforward_setAmplitude(i, 1, 0);        
    }
    
    feedforward_calculate();
}


// Calculates the feed-forward signal at point n out of nFeedforward
uint16_t calculateDacValue(int n)
{
    uint16_t dacVal;
    int32_t sum = 0;
    
    int i;
    for(i=0;i<nHarmonics;i++)
    {
        sum += sineAmplitude[i] * sineLookup[ (n*(i+1))%120 ];
        sum += cosAmplitude[i] * sineLookup[ (30 + n*(i+1))%120 ];
    }
    
    dacVal = 0x8000 + (sum>>15);
    return dacVal;
}


// Calculates the feedforward signal. This is called whenever the Fourier 
// coefficients are changed, so that in the interrupt loop we do the minimal 
// amount of work
void feedforward_calculate()
{
    int i;
    for(i=0;i<nFeedforward;i++) feedforwardBuf[i] = calculateDacValue(i);
}


