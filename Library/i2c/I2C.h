/*
*	I2C Library
*	Class for using the I2C hardware
*	(Does not use interrupts)
*	Written By Ryan Owens
*	SparkFun Electronics
*	8/25/10
*/

#ifndef I2C_h
#define I2C_h

//#include "LPC214x.h"
//#include "LPC21xx_SFE.h"

#define READ	1
#define WRITE	0

class I2C
{
	public:
		I2C(int i2c_port);	//Constructor defines which port on the LPC is used for I2C
		I2C(void);			//Overloaded contstructor assumes Port will be defined by inherited class.
		void configure(void);
		char send(char SLA, char * contents, char direction, char length);		
	protected:
		
		int _i2c_port;
		
};

/*I2C Control Register Bits */
#define	AA		2
#define	SI		3
#define	STO		4
#define	STA		5
#define I2EN	6

/*I2C Control Clear Reg. Bits */
#define	AAC		2
#define	SIC		3
#define	STAC	5
#define	I2ENC	6

#endif
