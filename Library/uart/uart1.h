/*
	Uart 1 Library
	(Actually only used to configure interrupts)
	Writtey by Ryan Owens
	9/27/10
*/
#ifndef uart1_h
#define uart1_h

void uart1RxInt(char trigger_level);

#define	RX1_TRIG_LEV_0	0
#define RX1_TRIG_LEV_1	1
#define RX1_TRIG_LEV_2	2
#define RX1_TRIG_LEV_3	3

#endif
