#include <proc/p32mx250f128b.h>

#include "Compiler.h"
#include "dac.h"





void dac_init(void)
{
    TRISBbits.TRISB1 = 0; // SDO_FB is an output
    TRISBbits.TRISB3 = 0; // SCK_FB is an output
    TRISBbits.TRISB4 = 0; // CS_FB_FINE is an output
    TRISAbits.TRISA4 = 0; // CS_FB_COARSE is an output
    
    TRISBbits.TRISB15 = 0; // SCK_FF is an output
    TRISBbits.TRISB5 = 0; // SDO_FF is an output
    TRISBbits.TRISB7 = 0; // CS_FF is an output
    
    
    // Ensure CS are all high to reset DAC state machines
    CS_FB_FINE = 1;
    CS_FB_COARSE = 1;
    CS_FF = 1;
    
    // Setup SPI2 for feed-forward DAC
    RPB5R = 0b0100; // Connect RB5 to SDO2
    SPI2CONbits.MODE16 = 1; // 16 bit word size
    SPI2CONbits.CKP = 1; // Output data transition on falling clock edge, with clock idling high
    SPI2CONbits.MSTEN = 1; // Master mode
    SPI2CONbits.DISSDI = 1; // SDI pin not controlled by module
    SPI2BRG = 0; // SPI clock is PBCLK/2 = 20 MHz
    SPI2STAT = 0; // Clear status bits
    SPI2CONbits.ON = 1;
}


void bitbangSPI(uint16_t val)
{
    int i;
    for(i=0;i<16;i++) {
        SDO_FB = (val & 0x8000) != 0;
        SCK_FB = 0;
        asm("nop");
        SCK_FB = 1;
        val = val << 1;
    }
}


void dac_setCoarse(uint16_t val)
{
    CS_FB_COARSE = 0;
    bitbangSPI(val);
    CS_FB_COARSE = 1;
}


void dac_setFine(uint16_t val)
{
    CS_FB_FINE = 0;
    bitbangSPI(val);
    CS_FB_FINE = 1;
}


void dac_setFeedForward(uint16_t val)
{
    //CS_FF = 0;
    IFS1bits.SPI2RXIF = 0; // Clear 'receive complete' flag
    SPI2BUF = val;
    while( SPI2STATbits.SPIBUSY ); // Wait for transmit complete
    asm("nop");
    //CS_FF = 1;
}
