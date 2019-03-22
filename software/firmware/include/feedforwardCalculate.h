#pragma once
#include <stdint.h>

// Number of points in the feedforward signal (i.e. update rate is 50 Hz * nFeedforward)
// NB if this is changed the sine lookup table needs to be updated manually
#define nFeedforward (120)

extern uint16_t feedforwardBuf[nFeedforward];

void feedforward_setAmplitude(int harmonic, int component, int16_t amplitude);
void feedforward_reset(void);
void feedforward_calculate(void);
