/*
	UART1
	Interrupt Service Routine
	
	Alerts the main program when an interrupt is triggered on UART1
	
	Written by Ryan Owens
	9/27/10
	
*/
#include "LPC214x.h"
#include "uart1ISR.h"

char uart1Message[75];
int mindex=0;
char uart1MessageComplete=0;

void ISR_UART1(void)
{
	char val = (char)U1RBR;
	
	if(val=='\r'){ 	
		uart1Message[mindex++]='\n';
		uart1Message[mindex++]='\0';
		mindex=0;
		uart1MessageComplete=1;
	}
	else if(uart1MessageComplete==0){
		if(val != '\n')uart1Message[mindex++]=val;
	}
	
	VICVectAddr =0;	//Update the VIC priorities
}
