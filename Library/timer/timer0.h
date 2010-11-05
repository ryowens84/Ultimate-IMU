#ifndef timer0_h
#define timer0_h

void timer0Init(unsigned long int freq);
void timer0Match(int mr, unsigned long int freq, char mode);

//Timer Modes
#define interruptOnMatch	(1<<0)
#define resetOnMatch		(1<<1)
#define stopOnMatch			(1<<2)

#endif
