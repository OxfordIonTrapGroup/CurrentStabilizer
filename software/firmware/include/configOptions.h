#pragma config PMDL1WAY = OFF // Allow multiple reconfigurations of peripherals
#pragma config IOL1WAY = OFF // Allow multiple peripheral pin select changes

#pragma config FNOSC = PRIPLL // Primary Oscillator + PLL
#pragma config POSCMOD = HS // 'HS' mode for crystals > 10 MHz (ours is 25MHz)
#pragma config FPLLIDIV = DIV_5 // Divide input clock (25MHz) by 5
#pragma config FPLLMUL = MUL_16 // Multiply PLL input (5 MHz) by 16 ...
#pragma config FPLLODIV = DIV_2 // ... and divide output by 2, giving a 40 MHz system clock
#pragma config FPBDIV = DIV_1 // Peripheral clock same as system clock
#pragma config FSOSCEN = OFF  // Secondary Oscillator disable (Necessary for digital IO on SOCS pins

#pragma config FUSBIDIO = OFF // Disable the USBID pin function (only required for USB OTG)
#pragma config FVBUSONIO = OFF // Disable the Vbus_on pin function (---"---"---)

#pragma config FWDTEN = OFF // Watchdog off
#pragma config JTAGEN = OFF // JTAG disable
#pragma config ICESEL = ICS_PGx3 // ICE comm via pin-set 3
#pragma config CP = OFF // Code protection off