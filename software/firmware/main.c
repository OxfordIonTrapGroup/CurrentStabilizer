#include "configOptions.h"

#include "TCPIPConfig.h"
#include "TCPIP Stack/TCPIP.h"

#include "ipInterface.h"
#include "dac.h"
#include "adc.h"


// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);


int main(void)
{
    static DWORD t = 0;

    TickInit(); 
    
    InitializeBoard();
    dac_init();
    adc_init();
    feedforward_init();
    ipInterface_init();

    LED0_IO = 0;
    int ledStatus = 0;
    
    
    while(1)
    {
        commandHandler();
        
        if(ipInterface_connected()) {
            LED0_IO = 1;
        }
        {
        // Blink LED 
        if(TickGet() - t >= TICK_SECOND/10ul)
        {
            t = TickGet();
            if(ledStatus) ledStatus = 0;
            else ledStatus = 1;
            LED0_IO = ledStatus;
        }
        }

        
        ipInterface_poll();
        adc_poll();
    }
}




static void InitializeBoard(void)
{    
    ANSELA = 0;
    ANSELB = 0; // All inputs digital

    // Initialise ethernet SDI and SDO pins
    SDI1R = 0b11; // SDI1 on pin 22 (RPB11)
    RPB13R = 0b11; // SDO1 on pin 24 (RPB13)

    // Initialise ethernet INT pin
    TRISBbits.TRISB10 = 1; // RB10 input

    // LED
    LED0_TRIS = 0;

    // Enable multi-vectored interrupts
    INTEnableSystemMultiVectoredInt();

    // Enable optimal performance
    SYSTEMConfigPerformance(GetSystemClock());
    mOSCSetPBDIV(OSC_PB_DIV_1);                // Use 1:1 CPU Core:Peripheral clocks

    ENC_CS_IO = 1;
    ENC_CS_TRIS = 0;
}

