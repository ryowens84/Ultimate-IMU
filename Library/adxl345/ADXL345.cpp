/*
	ADXL345 Library
	
	This libary contains functions to interact with the ADXL345 Triple Axis Digital Accelerometer from Analog Devices written for the ATmega328p
	In order to use this libary, define the appropriate pins in the ADXL345.h file

	created 20 Aug 2009
	by Ryan Owens
	http://www.sparkfun.com
	
	Example Code:
	char values[6];
	char status=0;
	accelerometer.begin();
	values[0]=POWER_CTL;
	status=accelerometer.read(values, 1);
	
	status=0;
	while(!status){
		values[0]=POWER_CTL;
		values[1] = MEASURE;
		status=accelerometer.write(values, 2);
		if(status)
			status=accelerometer.read(values, 1);
	}
	rprintf("New Power 2: %02x\t%d\n\r", values[0], status);
	
	
	while(1){
		values[0]=DATAX0;
		status=accelerometer.read(values, 6);
		if(status)
			for(int i=0; i<3; i++)
				rprintf("%02x%02x\t", values[i*2], values[i*2+1]);
		rprintf("\n\r");
	}	
 
*/
#include "ADXL345.h"
#include <stdlib.h>
#include <stdio.h>
#include "LPC21xx_SFE.h"
#include "main.h"

#define GLOBALOBJECT

cADXL345 accelerometer(0, ADXL_ADDR);
char status=0;

cADXL345::cADXL345(int port, char i2c_address)
{
	_i2c_port = port;
	_i2c_address = i2c_address;
	
}

void cADXL345::begin(void)
{
	configure();
	
	//Put the accelerometer in MEASURE mode
	values[0]=POWER_CTL;
	values[1] = MEASURE;
	write(values, 2);		//Put the Accelerometer into measurement mode
	
	//Set the Range to +/- 4G
	values[0] = DATA_FORMAT;
	values[1] = RANGE_0;
	write(values, 2);
	
	//default ADXL345 rate is 100 Hz. Perfect!
	
	updated=0;
}

void cADXL345::powerDown(void)
{

}

char cADXL345::read(char * value, char length){
	
	write(value, 1);	//Set up the i2c address to read from
	return send(_i2c_address, value, READ, length);	
}

char cADXL345::write(char * value, char length){

	return send(_i2c_address, value, WRITE, length);
}

char cADXL345::update(void)
{
	values[0] = DATAX0;
	status=accelerometer.read(values, 6);
	if(status)
	{
		xg = (int16_t)((values[1]<<8)|values[0]);
		yg = (int16_t)((values[3]<<8)|values[2]);
		zg = (int16_t)((values[5]<<8)|values[4]);
	}
	else return 0;
	
	return 1;
}

double cADXL345::getX(void)
{
	xg=xg*0.0078;
	return xg;
}

double cADXL345::getY(void)
{
	yg=yg*0.0078;
	return yg;
}

double cADXL345::getZ(void)
{
	zg=zg*0.0078;
	return zg;
}
