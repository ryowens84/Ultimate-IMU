/*
	Uart 0 Library
	(Actually only used to configure interrupts)
	Writtey by Ryan Owens
	11/16/10
*/
#include "LPC214x.h"
#include "uart0.h"

void uart0RxInt(char trigger_level)
{
	char level_setting = (trigger_level << 6);
	U0IER = (1<<0);	//Enable FIFO on UART with RDA interrupt (Receive Data Available)
	U0FCR &= (U0FCR & 0x3F) | level_setting;	//Set the trigger level for the Rx interrupt
}
