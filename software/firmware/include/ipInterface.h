#pragma once

// Call frequently to service TCPIP stack
void ipInterface_poll(void);


void ipInterface_init(void);

// Returns 1 if a client is connected, else 0
int ipInterface_connected(void);




// *** NB when the connection is closed the tx and rx buffers are flushed

// Add a character to the transmit buffer
void ipInterface_putc(char c);

// Attempt to read a character from the receive buffer into c.
// Returns 1 on success, or 0 if no data is available
int ipInterface_getc(char * c);