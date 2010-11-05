/*
Common Delay Routines

Select timer using a define (TIMER1 or TIMER0)
Fcclk should be defined as system clock frequency in target.h
*/
#ifndef delay_h_
#define delay_h_

#define TIMER1	//Use Timer 1

void delay_ms(int count);
void delay_us(int count);

#endif
