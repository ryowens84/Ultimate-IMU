#ifndef timer1_h
#define timer1_h

void timer1Init(unsigned long int freq);
void timer1Match(int mr, unsigned long int freq, char mode);


//Timer Modes
#define interruptOnMatch	(1<<0)
#define resetOnMatch		(1<<1)
#define stopOnMatch			(1<<2)

#endif
