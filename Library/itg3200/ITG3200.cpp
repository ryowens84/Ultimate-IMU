/*
	ITG3200 Library
	
	This libary contains functions to interact with the ITG3200
	I2C.h must be included as the I2C class is inherited

	created 8/30/10
	by Ryan Owens
	http://www.sparkfun.com
	
	Example Code:
	char values[6];
	
	gyro.begin();
	values[0]=WHO_AM_I;
	gyro.read(values, 1);
	rprintf("Who am I: %02x\n\r", values[0]);	
 
*/

#include "ITG3200.h"

#include <stdlib.h>
#include <stdio.h>
#include "LPC21xx_SFE.h"
#include "main.h"

extern "C"
{
	#include "rprintf.h"
	#include "timer1ISR.h"	
}

#define GLOBALOBJECT

cITG3200 gyro(0, ITG_ADDR);


cITG3200::cITG3200(int port, char i2c_address)
{
	_i2c_port = port;
	_i2c_address = i2c_address;
	
	x_cal=0;
	y_cal=0;
	z_cal=0;
}

void cITG3200::begin(void)
{
	configure();
	
	//Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
	values[0]=DLPF_FS;	
	values[1]=(DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0);
	write(values, 2);	//Write the new data to the HMC register.
	
	//Set sample rate divider for 100 Hz operation
	values[0] = SMPLRT_DIV;
	values[1] = 9;	//Fsample = Fint / (divider + 1) where Fint is 1kHz
	write(values, 2);
	
	//Setup the interrupt to trigger when new data is ready.
	values[0]=INT_CFG;
	values[1]=INT_CFG_RAW_RDY_EN | INT_CFG_ITG_RDY_EN;
	write(values, 2);
		
	//Select X gyro PLL for clock source
	values[0] = PWR_MGM;
	values[1] = PWR_MGM_CLK_SEL_0;
	write(values, 2);

	//Wait for the PLL clocks to stabilize
	values[0]=INT_STATUS;
	read(values, 1);
	while(!(values[0] & INT_CFG_ITG_RDY_EN))
	{
		values[0]=INT_STATUS;
		read(values, 1);
	}	
	
	updated=0;
}


char cITG3200::read(char * value, char length){
	
	write(value, 1);	//Set up the i2c address to read from
	return send(_i2c_address, value, READ, length);	
}

char cITG3200::write(char * value, char length){

	return send(_i2c_address, value, WRITE, length);
}

char cITG3200::update(void)
{
	values[0]=INT_STATUS;
	read(values, 1);
	
	if(values[0] == (1<<0))
	{
		values[0]=TEMP_OUT_H;
		if(read(values, 8))
		{
			tempr = (int16_t)((values[0]<<8)|values[1]);
			xr = (int16_t)((values[2]<<8)|values[3]);
			yr = (int16_t)((values[4]<<8)|values[5]);
			zr = (int16_t)((values[6]<<8)|values[7]);
		}
		else return 0;
	}
	else return 0;
	return 1;
}

void cITG3200::calibrate(void)
{
	long int timeout=0;
	for(int i=0; i<16; i++)
	{
		update();	//Get new values while device is not moving
		x_cal+=(int16_t)xr;
		y_cal+=(int16_t)yr;
		z_cal+=(int16_t)zr;
		timeout=millis();
		while(millis() < timeout+100);
	}
	x_cal/=16;
	y_cal/=16;
	z_cal/=16;
}

float cITG3200::getX(void)
{
	xr -= x_cal;
	xr = xr/14.375;
	return xr;
}

float cITG3200::getY(void)
{
	yr -= y_cal;
	yr = yr/14.375;
	return yr;
}

float cITG3200::getZ(void)
{
	zr -= z_cal;
	zr = zr/14.375;
	return zr;
}

float cITG3200::getTemp(void)
{
	tempr = -13200-tempr;	//Get the offset temp
	tempr = tempr/280;	//Convert the offset to degree C
	tempr += 35;	//Add 35 degrees C to compensate for the offset
	return tempr;
}
