#pragma once

#include "Compiler.h"


// Clock frequency values
// These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
#define GetSystemClock()		(40000000ul)			// Hz
#define GetInstructionClock()	(GetSystemClock()/1)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
#define GetPeripheralClock()	(GetSystemClock()/1)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.



// LEDs
#define LED0_TRIS			(TRISBbits.TRISB8)	// The one and only LED
#define LED0_IO				(LATBbits.LATB8)


//Ethernet register mappings
#define ENC_CS_IO (LATBbits.LATB9)              // Defines the chip select pin LAT register bit
#define ENC_CS_TRIS (TRISBbits.TRISB9)          // Defines the chip select pin TRIS register bit
#define ENC_SSPBUF (SPI1BUF)                    // Points to the SPI buffer register
#define ENC_SPICON1bits (SPI1CONbits)           // Provides bitwise access to the SPI control register
#define ENC_SPICON1 (SPI1CON)                   // Points to the SPI control register
#define ENC_SPISTATbits (SPI1STATbits)          // Provides bitwise access to the SPI status register
#define ENC_SPIBRG (SPI1BRG)                    // Points to the SPI baud rate generator
#define ENC_SPI_IF (IFS1bits.SPI1RXIF)          // Points to the SPI interrupt flag
//See 9.1.3 ENC28J60 Config in TCPIP Stack Help
