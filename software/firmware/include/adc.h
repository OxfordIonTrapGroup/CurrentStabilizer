#pragma once
#include <stdint.h>

void adc_init();

void adc_poll();

extern uint16_t adcVal;