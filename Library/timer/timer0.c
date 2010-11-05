#include "timer0.h"
#include "LPC214x.h"
#include "target.h"	//Needed for Fcclk
#include "rprintf.h"

static unsigned long int tFreq;

void timer0Init(unsigned long int freq)
{
	tFreq=freq;
	T0PR=Fcclk/tFreq;			//Set up the prescaler for a 'freq' frequency
	T0CTCR=0;					//Timer Mode
	T0TCR |=0X01;				//Enable the clock
	//T0MCR=0x0003;				//Interrupt and Reset Timer on Match
	//T0MR0=(50000/TIMER_FREQ);	
}

void timer0Match(int mr, unsigned long int freq, char mode)
{
	unsigned long int mfreq = tFreq/freq;		//Get the Match Register value by dividing timer frequency by request match frequency

	switch(mr)
	{
		case 0:
			T0MR0 = mfreq;
			T0MCR = (mode<<0);		
			break;
		case 1:
			T0MR1 = mfreq;
			T0MCR = (mode<<3);
			break;
		case 2:
			T0MR2 = mfreq;
			T0MCR = (mode<<6);
			break;
		case 3:
			T0MR3 = mfreq;
			T0MCR = (mode<<9);
			break;
		default:
			break;
	}
}
