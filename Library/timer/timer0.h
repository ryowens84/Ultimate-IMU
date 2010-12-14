#ifndef timer0_h
#define timer0_h

void timer0Init(unsigned long int freq);
void timer0Match(int mr, unsigned long int freq, char mode);

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
