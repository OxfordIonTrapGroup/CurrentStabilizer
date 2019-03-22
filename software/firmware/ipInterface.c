#include "TCPIPConfig.h"
#include "TCPIP Stack/TCPIP.h"
#include "circBuff.h"


// Rx & Tx circular buffers
#define RXBUFF_LEN 1024
#define TXBUFF_LEN 1024

uint8_t rxBuff_raw[RXBUFF_LEN];
circBuff rxBuff;

uint8_t txBuff_raw[TXBUFF_LEN];
circBuff txBuff;



#define SERVER_PORT	9760


typedef struct
{
	unsigned short wConfigurationLength;	// Number of bytes saved in EEPROM/Flash (sizeof(APP_CONFIG))
	unsigned short wOriginalChecksum;		// Checksum of the original AppConfig defaults as loaded from ROM (to detect when to wipe the EEPROM/Flash record of AppConfig due to a stack change, such as when switching from Ethernet to Wi-Fi)
	unsigned short wCurrentChecksum;		// Checksum of the current EEPROM/Flash data.  This protects against using corrupt values if power failure occurs while writing them and helps detect coding errors in which some other task writes to the EEPROM in the AppConfig area.
} NVM_VALIDATION_STRUCT;





// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;
static unsigned short wOriginalAppConfigChecksum;    // Checksum of the ROM defaults for AppConfig



// MAC Address Serialization using a MPLAB PM3 Programmer and 
// Serialized Quick Turn Programming (SQTP). 
// The advantage of using SQTP for programming the MAC Address is it
// allows you to auto-increment the MAC address without recompiling 
// the code for each unit.  To use SQTP, the MAC address must be fixed
// at a specific location in program memory.  Uncomment these two pragmas
// that locate the MAC address at 0x1FFF0.  Syntax below is for MPLAB C 
// Compiler for PIC18 MCUs. Syntax will vary for other compilers.
static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};

static void InitAppConfig(void)
{

        // Start out zeroing all AppConfig bytes to ensure all fields are 
        // deterministic for checksum generation
        memset((void*)&AppConfig, 0x00, sizeof(AppConfig));
        
        AppConfig.Flags.bIsDHCPEnabled = TRUE;
        AppConfig.Flags.bInConfigMode = TRUE;
        memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));

        AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
        AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
        AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
        AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
        AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
        AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
        AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
    
        // Load the default NetBIOS Host Name
        memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
        FormatNetBIOSName(AppConfig.NetBIOSName);

        // Compute the checksum of the AppConfig defaults as loaded from ROM
        wOriginalAppConfigChecksum = CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig));
}



enum _TCPServerState
	{
		SM_HOME = 0,
		SM_LISTENING,
        SM_CLOSING,
	} TCPServerState = SM_HOME;
TCP_SOCKET	s;    
    
void tcpServer(void)
{
    int i;
	uint8_t tmp, nChunk;
    const uint8_t bufLen = 32;
    uint8_t buf[bufLen];
	uint16_t nRxFifoFree, nTxFifoFree;

	switch(TCPServerState)
	{
		case SM_HOME:
			// Allocate a socket for this server to listen and accept connections on
			s = TCPOpen(0, TCP_OPEN_SERVER, SERVER_PORT, TCP_SERVER);
			if(s == INVALID_SOCKET)
				return;

            // Clear 'was reset' sticky flag
            TCPWasReset(s);
            
			TCPServerState = SM_LISTENING;
			break;

		case SM_LISTENING:
            if(TCPWasReset(s)) {
                TCPServerState = SM_CLOSING;
                return;
            }
            
			// See if anyone is connected to us
			if(!TCPIsConnected(s))
				return;


			// Figure out how many bytes have been received and how many we can transmit.
			nRxFifoFree = TCPIsGetReady(s);	// Get TCP RX FIFO byte count
			nTxFifoFree = TCPIsPutReady(s);	// Get TCP TX FIFO free space

            // Move all bytes from rx fifo into rx buffer
            while(nRxFifoFree > 0)
            {
                nChunk = (nRxFifoFree>bufLen)?(bufLen):(nRxFifoFree);
                TCPGetArray(s, buf, nChunk);
                for(i=0; i<nChunk; i++) circBuffPush(&rxBuff, buf[i]);
                nRxFifoFree -= nChunk;
            }


            // Move as many bytes as we can into the tx buffer
            tmp = circBuffNBuffered(&txBuff);
            if(tmp < nTxFifoFree) nTxFifoFree = tmp; // We transmit min(num free in fifo, num stored in buffer)
            while(nTxFifoFree > 0)
            {
                nChunk = (nTxFifoFree>bufLen)?(bufLen):(nTxFifoFree);
                circBuffRead( &txBuff, buf, nChunk);               
                TCPPutArray(s, buf, nChunk);
                nTxFifoFree -= nChunk;
            }								
            
            // Only needed if we are in a hurry to transmit
            TCPFlush(s); 
			break;

		case SM_CLOSING:
            TCPClose(s);
			TCPServerState = SM_HOME;
            
            circBuffFlush( &txBuff );
            circBuffFlush( &rxBuff );
			break;
	}
}


void ipInterface_putc(char c)
{
    circBuffPush(&txBuff, c);
}

void _mon_putc(char c)
{
    ipInterface_putc(c);
}

int ipInterface_getc(char * c)
{
    return circBuffRead( &rxBuff, c, 1 );
}


int ipInterface_connected(void)
{
    return (TCPServerState == SM_LISTENING) && TCPIsConnected(s);
}



void ipInterface_poll(void)
{
    StackTask();
    StackApplications();
    tcpServer(); 
}


void ipInterface_init(void)
{
    InitAppConfig();
    StackInit(); 
    
    // Initialise the Rx & Tx circular buffers
    rxBuff.buffBase =  rxBuff_raw;
	rxBuff.length = RXBUFF_LEN;    
    circBuffFlush( &rxBuff );

    txBuff.buffBase =  txBuff_raw;
	txBuff.length = TXBUFF_LEN;    
    circBuffFlush( &txBuff );
}

