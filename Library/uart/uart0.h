/*
	Uart 1 Library
	(Actually only used to configure interrupts)
	Writtey by Ryan Owens
	9/27/10
*/
#ifndef uart0_h
#define uart0_h

void uart0RxInt(char trigger_level);

#define	RX0_TRIG_LEV_0	0
#define RX0_TRIG_LEV_1	1
#define RX0_TRIG_LEV_2	2
#define RX0_TRIG_LEV_3	3

#endif
