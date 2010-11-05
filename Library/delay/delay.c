/*
Common Delay Routines

Please ensure the #defines are set according to the timer prescaler used for the target timer
*/
#include "delay.h"
#include "LPC214x.h"
#include "rprintf.h"
#include "target.h"

void delay_ms(int count)
{
	for(int i=0; i<count; i++)
	{
		delay_us(1000);
	}
}

void delay_us(int count)
{
#ifdef TIMER1
	//Fcclk should be defined as system freq. in target.h
	T1MR0 = Fcclk/1000000; //Set the match register to measure 1MHz (1uS period)
	T1MCR = (3<<0);	//Generate an interrupt when MR0 is matched
	T1TCR = (1<<1);	//Reset the Timer Count
	T1TCR = (1<<0);	//Start timer 1
	for(int i=0; i<count; i++)
	{
		while(!(T1IR & (1<<0)));	//Wait for an MR0 interrupt
		T1IR = (1<<0);		//Reset the interrupt register
		//rprintf("+");
	}
#endif
#ifdef TIMER0
	//Fcclk should be defined as system freq. in target.h
	T0MR0 = Fcclk/1000000;	//Set the match register to measure 1MHz (1uS period)
	T0MCR = (3<<0);	//Generate an interrupt when MR0 is matched
	T0TCR = (1<<1);	//Reset the Timer Count
	T0TCR = (1<<0);	//Start timer 1
	for(int i=0; i<count; i++)
	{
		while(!(T0IR & (1<<0)));	//Wait for an MR0 interrupt
		T0IR = (1<<0);		//Reset the interrupt register
		//rprintf("+");
	}
#endif
}
