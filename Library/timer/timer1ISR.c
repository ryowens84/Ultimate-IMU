/*
Timer 0 Interrupt Service Routine
Written by Ryan Owens
9/27/10
*/

//TODO: Add sensor header files to allow for updated the classes

#include "LPC214x.h"
#include "timer1ISR.h"

char timer1IntFlag=0;

//Usage: None (Automatically Called by FW)
//Inputs: None
//This function is a global interrupt called by a match on the Timer 0 match.  
void ISR_Timer1(void)
{
	//Interrupt Code Here
	timer1IntFlag=1;	
	
	//Clear the interrupt and update the VIC priority
	T1IR = 0xFF;
	VICVectAddr =0;						
}



