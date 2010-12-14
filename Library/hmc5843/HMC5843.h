#ifndef HMC5843_h
#define HMC5843_h

#include "I2C.h"
#include <stdint.h>

class cHMC5843: public I2C
{
	public:
		char updated;
	
		cHMC5843(int port, char i2c_address);
		void begin(char range);
		char read(char * values, char length);
		char write(char * values, char length);
		char update(void);
		
		float getX(void);
		float getY(void);
		float getZ(void);
		float getHeading(void);
		
	private:
		//int16_t x;
		//int16_t y;
		//int16_t z;	
		float xc;
		float yc;
		float zc;
		float heading;
		int gain;
		
		char values[6];
		char _i2c_address;
};
extern cHMC5843 compass;

/* ************************ Register map for the HMC5843 ****************************/
//I2C Address for the HMC5843
#define HMC_ADDR	0x3C

//HMC5843 Register Addresses
#define	CONFIG_REGA	0x00
#define	CONFIG_REGB	0x01
#define	MODE_REG	0x02
#define	DATA_OUT_X_H	0x03
#define DATA_OUT_X_L	0x04
#define DATA_OUT_Y_H	0x05
#define	DATA_OUT_Y_L	0x06
#define	DATA_OUT_Z_H	0x07
#define	DATA_OUT_Z_L	0x08
#define	STATUS_REG		0x09
#define ID_REGA		0x0A
#define	ID_REGB		0x0B
#define	ID_REGC		0x0C

//Configuration Register A Bigs
#define CONFIG_REGA_MS0	(1<<0)
#define CONFIG_REGA_MS1	(1<<1)
#define CONFIG_REGA_DO0	(1<<2)
#define CONFIG_REGA_DO1	(1<<3)
#define CONFIG_REGA_DO2	(1<<4)

//Configuration Register B Bits
#define CONFIG_REGB_GN0	(1<<5)
#define CONFIG_REGB_GN1	(1<<6)
#define	CONFIG_REGB_GN2	(1<<7)

//Mode Register Bits
#define MODE_REG_MD0	(1<<0)
#define MODE_REG_MD1	(1<<1)

//Status Register Bits
#define STATUS_REG_RDY	(1<<0)
#define STATUS_REG_LOCK	(1<<1)
#define STATUS_REG_REN	(1<<2)

#endif
