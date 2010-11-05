/*
	UART1
	Interrupt Service Routine
	
	Alerts the main program when an interrupt is triggered on UART1
	
	Written by Ryan Owens
	9/27/10
	
	TODO: Routine needs a buffer to place the incoming character.
*/
#ifndef uart1ISR_h
#define uart1ISR_h

#define INT_UART1	(1<<7)

extern char uart1MessageComplete;
extern char uart1Message[75];
void ISR_UART1(void);

#endif
