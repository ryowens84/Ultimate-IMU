/*
Timer 1 Interrupt Service Routine
Header File
Written By Ryan Owens
9/27/10
*/
#ifndef timer1ISR_h
#define timer1ISR_h

#define INT_TIMER1	(1<<5)

extern char timer1IntFlag;

void ISR_Timer1(void);


#endif
