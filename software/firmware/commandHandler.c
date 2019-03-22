#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ipInterface.h"
#include "dac.h"
#include "feedforward.h"
#include "feedforwardCalculate.h"
#include "adc.h"


#define MAXCMDLEN 250
char cmdBuf[MAXCMDLEN];
uint8_t cmdInd = 0;

void commandProcess(char * cmdStr);

void commandIdn();
void commandReset();
void commandComTest(char ** args);
void commandSetFineDac(char ** args);
void commandSetCoarseDac(char ** args);
void commandSetFeedforward(char ** args);
void commandResetFeedforward();
void commandGetErrorSignal();
void commandPhaseLockStatus();
void commandTest(char ** args);


// Called periodically to process any received data
void commandHandler()
{
	int i;
	uint8_t bufEnd, cmdStart;
    uint8_t c;

    
    // While the receive buffer is not empty
    while( ipInterface_getc(&c) )
    {    
        cmdBuf[cmdInd] = c;

        // bufEnd points to end of data in cmdBuf
        bufEnd = cmdInd + 1;
        cmdStart = 0;

        // Go through the new additions to the buffer looking for a '\n'
        for( i=cmdInd; i<bufEnd; i++) {
            if(cmdBuf[i] != '\n') continue;
            // If we get here, there is a valid command between cmdStart and cmdInd
            cmdBuf[i] = 0; // Zero terminate ...
            // ... and run command
            commandProcess(&cmdBuf[cmdStart]);
            cmdStart = i +1; // Start of next command
        }

        // Copy the remaining unprocessed commands to the start of the buffer
        if( cmdStart != 0) {
            if( cmdStart > bufEnd ) cmdStart = bufEnd;
            memcpy(&cmdBuf[0], &cmdBuf[cmdStart], bufEnd-cmdStart);
        }
        cmdInd = bufEnd-cmdStart;


        // If we have got a full command buffer with out a valid command in it...
        if( bufEnd == MAXCMDLEN ) {
            // Dump the buffer
            cmdInd = 0;
        }
    }
}


/* Commands:
 * idn? : Returns device version string
 * reset : Software reset
 * comTest <str> : Loopback test. Responds with 'comTest=<str>\n'
 * setFineDac <val> : set the fine setpoint DAC to the uint16 val
 * setCoarseDac <val> : set the coarse setpoint DAC to the uint16 val 
 * getErrorSignal : return the ADC reading of the error signal
 * setFeedforward <nHarmonic> <component> <amplitude> : set the feedforward amplitude for nHarmonic=1-5, component = 'c' or 's' for sine or cosine, and amplitude a int16 number
 * resetFeedforward : Set all the feedforward amplitudes to zero
 * phaseLockStatus : Returns the current feedforward period and phase
 */

// Parses and acts on received command string. Handles up to 5 arguments
void commandProcess(char * cmdStr) {
	int nargs;
	char *cmdWord, *args[5] = {0};

	//xil_printf("Got command str: '%s'\n", cmdStr);

	cmdWord = strtok(cmdStr, " \n");

	// Split the arguments list up to 5 arguments
	for(nargs = 0; (nargs < 5) && ((args[nargs] = strtok(0, " \n")) != NULL); nargs++);


	if (strcmp(cmdWord, "idn?") == 0) { // Identify
		commandIdn();
	} else if( strcmp(cmdWord, "reset") == 0) { // Software reset
		commandReset();
	} else if( strcmp(cmdWord, "comTest") == 0 && nargs == 1) { 
		commandComTest(args);
	} else if( strcmp(cmdWord, "setFineDac") == 0 && nargs == 1) { 
		commandSetFineDac(args);
	} else if( strcmp(cmdWord, "setCoarseDac") == 0 && nargs == 1) { 
		commandSetCoarseDac(args);
	} else if( strcmp(cmdWord, "setFeedforward") == 0 && nargs == 3) {         
        commandSetFeedforward(args);
    } else if( strcmp(cmdWord, "resetFeedforward") == 0) {         
        commandResetFeedforward();
	} else if( strcmp(cmdWord, "getErrorSignal") == 0) {           
        commandGetErrorSignal();
    } else if( strcmp(cmdWord, "phaseLockStatus") == 0) {    
        commandPhaseLockStatus();
	} else if( strcmp(cmdWord, "test") == 0) { 
		commandTest(args);   
	} else { // Unknown command
		printf("Error: Unknown command '%s' or wrong nargs (%d)\n", cmdWord, nargs);
	}

}


void commandIdn()
{
	printf("BFieldController\n");
}

void commandReset()
{
	SoftReset();
}


void commandComTest(char ** args)
{
	printf("comTest=%s\n", args[0] );
}


void commandSetFineDac(char ** args)
{
    uint16_t val = atol(args[0]);
    dac_setFine(val);
}


void commandSetCoarseDac(char ** args)
{
    uint16_t val = atol(args[0]);
    dac_setCoarse(val);
}


void commandSetFeedforward(char ** args)
{
    int harmonic, component, amplitude;
    
    harmonic = atoi(args[0]);
    if(harmonic < 1 || harmonic > 5) {
        printf("Error: harmonic must be between 1 and 5 (was %d)\n", harmonic);
        return;
    }
    
    if(args[1][0] == 's') component = 0;
    else if(args[1][0] == 'c') component = 1;
    else {
        printf("Error: unknown component '%s' - should be 's' or 'c'\n", args[1]);
        return;
    }
    
    amplitude = atoi(args[2]);
    
    feedforward_setAmplitude(harmonic, component, amplitude);
    feedforward_calculate();
}


void commandResetFeedforward()
{
    feedforward_reset();
}


void commandGetErrorSignal()
{
    printf("%d\n", adcVal);
}


void commandTest(char ** args)
{
    int n = atol(args[0]);
    printf("%d\n", calculateDacValue(n));
}


void commandPhaseLockStatus()
{
    printf("period = %d\nphase = %d\n", currentPeriod, currentPhase);
}