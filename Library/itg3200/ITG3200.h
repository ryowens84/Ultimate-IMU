#ifndef ITG3200_h
#define ITG3200_h

#include <stdint.h>
#include "I2C.h"

class cITG3200: public I2C
{
	public:
		char updated;
	
		cITG3200(int port, char i2c_address);
		void begin(void);
		char read(char * value, char length);
		char write(char * value, char length);
		char update(void);
		void calibrate(void);
		void setCalibrationValues(int x, int y, int z);
		int16_t getXcal(void);
		int16_t getYcal(void);
		int16_t getZcal(void);
				
		//Return the values in engineering units (degrees/second and degrees C)
		float getX(void);
		float getY(void);
		float getZ(void);
		float getTemp(void);
		
		
	private:
		//int16_t temp;
		int16_t x_cal;
		int16_t y_cal;
		int16_t z_cal;
		float xr;
		float yr;
		float zr;
		float tempr;
		char values[8];	
		char _i2c_address;
};
extern cITG3200 gyro;

/* ************************ Register map for the ITG3200 ****************************/
#define ITG_ADDR	0xD0

#define WHO_AM_I	0x00
#define SMPLRT_DIV	0x15
#define	DLPF_FS		0x16
#define INT_CFG		0x17
#define INT_STATUS	0x1A
#define	TEMP_OUT_H	0x1B
#define	TEMP_OUT_L	0x1C
#define GYRO_XOUT_H	0x1D
#define	GYRO_XOUT_L	0x1E
#define GYRO_YOUT_H	0x1F
#define GYRO_YOUT_L	0x20
#define GYRO_ZOUT_H	0x21
#define GYRO_ZOUT_L	0x22
#define	PWR_MGM		0x3E

//Sample Rate Divider
//Fsample = Fint / (divider + 1) where Fint is either 1kHz or 8kHz
//Fint is set to 1kHz
//Set divider to 9 for 100 Hz sample rate

//DLPF, Full Scale Register Bits
//FS_SEL must be set to 3 for proper operation
//Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
#define DLPF_CFG_0	(1<<0)
#define DLPF_CFG_1	(1<<1)
#define DLPF_CFG_2	(1<<2)
#define DLPF_FS_SEL_0	(1<<3)
#define DLPF_FS_SEL_1	(1<<4)

//Power Management Register Bits
//Recommended to set CLK_SEL to 1,2 or 3 at startup for more stable clock
#define PWR_MGM_CLK_SEL_0	(1<<0)
#define PWR_MGM_CLK_SEL_1	(1<<1)
#define PWR_MGM_CLK_SEL_2	(1<<2)
#define PWR_MGM_STBY_Z	(1<<3)
#define PWR_MGM_STBY_Y	(1<<4)
#define PWR_MGM_STBY_X	(1<<5)
#define PWR_MGM_SLEEP	(1<<6)
#define PWR_MGM_H_RESET	(1<<7)

//Interrupt Configuration Bist
#define INT_CFG_ACTL	(1<<7)
#define INT_CFG_OPEN	(1<<6)
#define INT_CFG_LATCH_INT_EN	(1<<5)
#define INT_CFG_INT_ANYRD	(1<<4)
#define INT_CFG_ITG_RDY_EN	(1<<2)
#define INT_CFG_RAW_RDY_EN	(1<<0)

#endif
