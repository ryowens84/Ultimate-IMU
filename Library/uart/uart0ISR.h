/*
	UART1
	Interrupt Service Routine
	
	Alerts the main program when an interrupt is triggered on UART1
	
	Written by Ryan Owens
	9/27/10
	
	TODO: Routine needs a buffer to place the incoming character.
*/
#ifndef uart0ISR_h
#define uart0ISR_h

#define INT_UART0	(1<<6)

extern char uart0MessageComplete;
extern char uart0Message[75];
void ISR_UART0(void);

#endif
