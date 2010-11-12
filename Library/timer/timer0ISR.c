/*
Timer 0 Interrupt Service Routine
Written by Ryan Owens
9/27/10
*/

//TODO: Add sensor header files to allow for updated the classes

#include "LPC214x.h"
#include "main.h"
#include "timer0ISR.h"

char timer0IntFlag=0;

//Usage: None (Automatically Called by FW)
//Inputs: None
//This function is a global interrupt called by a match on the Timer 0 match.  
void ISR_Timer0(void)
{
	//Interrupt Code Here
	timer0IntFlag+=1;
	
	//Clear the interrupt and update the VIC priority
	T0IR = 0xFF;
	VICVectAddr =0;						
}
