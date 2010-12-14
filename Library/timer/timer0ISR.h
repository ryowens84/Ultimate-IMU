/*
Timer 0 Interrupt Service Routine
Header File
Written By Ryan Owens
9/27/10
*/
#ifndef timer0ISR_h
#define timer0ISR_h

#define INT_TIMER0	(1<<4)

extern char timer0IntFlag;
void ISR_Timer0(void);
long int millis(void);

#endif
