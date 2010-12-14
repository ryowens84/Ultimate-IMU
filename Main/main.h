#ifndef main_h
#define main_h

#include <stdint.h>

#define LED	(1<<15)
#define XBEE_EN	(1<<10)

#define LEDon()		IOSET0 = LED
#define LEDoff()		IOCLR0 = LED

#define XBEEon()		IOSET0 = XBEE_EN
#define XBEEoff()		IOCLR0 = XBEE_EN

#define NUM_CONFIG_PARAMS	6

extern char major_version;
extern char minor_version;

void reset(void);

#endif
