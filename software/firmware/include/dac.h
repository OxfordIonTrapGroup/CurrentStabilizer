#pragma once

#include <stdint.h>

void dac_init(void);

void dac_setCoarse(uint16_t val);
void dac_setFine(uint16_t val);
void dac_setFeedForward(uint16_t val);


#define SDO_FB  (LATBbits.LATB1)
#define SCK_FB  (LATBbits.LATB3)
#define CS_FB_FINE  (LATBbits.LATB4)
#define CS_FB_COARSE    (LATAbits.LATA4)
#define CS_FF   (LATBbits.LATB7)