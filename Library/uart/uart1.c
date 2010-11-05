/*
	Uart 1 Library
	(Actually only used to configure interrupts)
	Writtey by Ryan Owens
	9/27/10
*/
#include "LPC214x.h"
#include "uart1.h"

void uart1RxInt(char trigger_level)
{
	char level_setting = (trigger_level << 6);
	U1IER = (1<<0);	//Enable FIFO on UART with RDA interrupt (Receive Data Available)
	U1FCR &= (U1FCR & 0x3F) | level_setting;	//Set the trigger level for the Rx interrupt
}
