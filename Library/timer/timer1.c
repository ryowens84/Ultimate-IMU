#include "timer1.h"
#include "LPC214x.h"
#include "target.h"	//Needed for Fcclk
#include "rprintf.h"
#include "timer1ISR.h"

static unsigned long int tFreq;

void timer1Init(unsigned long int freq)
{
	tFreq=freq;
	T1PR=Fcclk/tFreq;			//Set up the prescaler for a 'freq' frequency
	T1CTCR=0;					//Timer Mode
	T1TCR |=0X01;				//Enable the clock
}


void timer1Match(int mr, unsigned long int freq, char mode)
{
	unsigned long int mfreq = tFreq/freq;		//Get the Match Register value by dividing timer frequency by request match frequency

	switch(mr)
	{
		case 0:
			T1MR0 = mfreq;
			T1MCR = (mode<<0);		
			break;
		case 1:
			T1MR1 = mfreq;
			T1MCR = (mode<<3);
			break;
		case 2:
			T1MR2 = mfreq;
			T1MCR = (mode<<6);
			break;
		case 3:
			T1MR3 = mfreq;
			T1MCR = (mode<<9);
			break;
		default:
			break;
	}
}

