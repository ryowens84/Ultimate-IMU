#ifndef timer1_h
#define timer1_h

#define DELAY	1

void timer1Init(unsigned long int freq);
void timer1Match(int mr, unsigned long int freq, char mode);

//Only include these delay functions if DELAY is defined.
#ifdef DELAY
void delay_ms(int count);
void delay_us(int count);
#endif

//Timer Modes
#define interruptOnMatch	(1<<0)
#define resetOnMatch		(1<<1)
#define stopOnMatch			(1<<2)

#endif
