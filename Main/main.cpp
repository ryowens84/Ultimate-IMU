/*
Ultimate IMU Code

Written by Ryan Owens
SparkFun Electronics
10/1/10

See code repository for most updated version
link

license
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LPC214x.h"
#include "target.h"
#include <math.h>
//*******************************************************
//						C Libraries
//*******************************************************
extern "C"{
#include "main_msc.h"

#include "serial.h"
#include "rprintf.h"

#include "timer0.h"
#include "timer0ISR.h"
#include "timer1.h"
#include "timer1ISR.h"

#include "uart1.h"
#include "uart1ISR.h"
}

//*******************************************************
//						C++ Libs
//*******************************************************
#include "main.h"
#include "ADXL345.h"
#include "HMC5843.h"
#include "ITG3200.h"
#include "sensor.h"
#include "memory.h"
#include "EM408.h"

//*******************************************************
//					Core Functions
//*******************************************************
void bootUp(void);
void reset(void);
void runTest(void);

//*******************************************************
//					Global Variables
//*******************************************************
char sensors_updated=0;
char sensor_string[50]="Test";
long int timeout=0;

cMemory sensorData;


//*******************************************************
//					Main Code
//*******************************************************
int main (void)
{
	//Initialize ARM I/O
	bootUp();			//Init. I/O ports, Comm protocols and interrupts
	if(!memoryBegin())
	{
		rprintf("SD Card Initialization Failed");
	}
	XBEEon();
	LEDoff();

	timer0Init(1000000);
	timer0Match(0, 100, interruptOnMatch | resetOnMatch);
	
	timer1Init(1000000);
	timer1Match(0, 1000, interruptOnMatch | resetOnMatch);
	
	if(memoryExists("Test.txt")){
		rprintf("Testing...");	
		VICIntEnable |= INT_TIMER1;
		//Set the UART0 pins to I/O for initial XBee test
		PINSEL0 &= ~((3<<0) | (3<<2));	//Set P0.0 and P0.1 to GPIO
		IODIR0 |= (1<<0)|(1<<1);	//Set P0.0 and P0.1 to outputs
		
		for(int blink=0; blink < 10; blink++)
		{
			IOSET0 = (1<<0)|(1<<1);	//Turn on P0.0 and P0.1
			timeout = millis();
			while(millis() < timeout+50);
			IOCLR0 = (1<<0)|(1<<1);
			timeout = millis();
			while(millis() < timeout+50);
		}
		runTest();
		while(1);
	}
	rprintf("No Test");
	accelerometer.begin();
	gyro.begin();
	compass.begin();	

	sensorData.create("Sensor", ".csv");
	uart1RxInt(0);
	VICIntEnable |= INT_TIMER0|INT_TIMER1|INT_UART1;
	
	gps.begin(4800);
	timeout = millis();
	while(millis() < timeout+100);
	gps.on();
	timeout = millis();
	while(millis() < timeout+1000);
	gps.disable();
	timeout = millis();
	while(millis() < timeout+500);
	gps.enable(4,1);
	timeout = millis();
	while(millis() < timeout+100);

	while(1)
	{
		if(timer0IntFlag==1)
		{
			VICIntEnClr |= INT_TIMER0;
			timer0IntFlag=0;
			
			accelerometer.update();
			gyro.update();
			compass.update();
			sensors_updated=1;	
			
			if(IOPIN0 & LED)LEDoff();
			else LEDon();
			
			VICIntEnable |= INT_TIMER0;
		}
		
		if(uart1MessageComplete)
		{
			VICIntEnClr |= INT_UART1;
			uart1MessageComplete=0;
			
			strcpy(gps.message, uart1Message);
			rprintf("%s\n", gps.message);
			
			VICIntEnable |= INT_UART1;
		}
		
		if(sensors_updated)
		{
			sensors_updated=0;
			
			sprintf(sensor_string, "%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f\n\r",
					accelerometer.getX(), accelerometer.getY(), accelerometer.getZ(),
					gyro.getX(), gyro.getY(), gyro.getZ(),
					compass.getX(), compass.getY(), compass.getZ());

			sensorData.save(sensor_string);
		}
		
		//If a USB Cable gets plugged in, stop everything!
		if(IOPIN0 & (1<<23))
		{
			VICIntEnClr = INT_TIMER0 | INT_TIMER1;	//Stop all running interrupts			
			main_msc();								//Open the mass storage device
			reset();								//Reset to check for new FW
		}
		
	}
	
	
    return 0;
}

//Usage: bootUp();
//Inputs: None
//This function initializes the serial port, the SD card, the I/O pins and the interrupts
void bootUp(void)
{
	//Initialize UART for RPRINTF
    rprintf_devopen(putc_serial0); //Init rprintf
	init_serial0(9600);		
	
	//Initialize I/O Ports and Peripherals
	IODIR0 |= (LED| XBEE_EN);
	
    //Setup the Interrupts
	//Enable Interrupts
	VPBDIV=1;										// Set PCLK equal to the System Clock	
	VICIntSelect = ~(INT_TIMER0|INT_TIMER1|INT_UART1);
	VICVectCntl0 = 0x20 | 4;						//Timer 0 Interrupt
	VICVectAddr0 = (unsigned int)ISR_Timer0;
	VICVectCntl1 = 0x20 | 5;						//Timer 1 Interrupt
	VICVectAddr1 = (unsigned int)ISR_Timer1;
	VICVectCntl2 = (0x20 | 7);
	VICVectAddr2 = (unsigned int)ISR_UART1;			//UART 1 Interrupt
	
}

//Usage: reset();
//Inputs: None
//Description: Resets the LPC2148
void reset(void)
{
    // Intentionally fault Watchdog to trigger a reset condition
    WDMOD |= 3;
    WDFEED = 0xAA;
    WDFEED = 0x55;
    WDFEED = 0xAA;
    WDFEED = 0x00;
}

void runTest(void)
{
	char value;
	int gpstest;
	
	//Enable the serial port
	//Initialize UART for RPRINTF
    rprintf_devopen(putc_serial0); //Init rprintf
	init_serial0(9600);	

	//Test the GPS I/O
	IODIR0 &= ~((1<<8)|(1<<9)|(1<<12));	//Set P0.8, P0.9 and P0.12 to inputs
	gpstest = IOPIN0;
	gpstest = (gpstest>>8)&0x13;
	if(gpstest != 0x11)
	{
		rprintf("GPS Failed.");
		while(1);
	}
	
	//Test the Accelerometer
	accelerometer.begin();
	//Get the Device ID from the accelerometer
	value = DEVID;
	accelerometer.read(&value, 1);
	if(value != 0xE5)
	{
		rprintf("Accel Failed to ping");
		while(1);
	}
	
	//Test the Gyroscope
	gyro.begin();
	//Get the Device ID from the gyro
	value = WHO_AM_I;
	gyro.read(&value, 1);
	if((value & 0x68) != 0x68)
	{
		rprintf("Gyro failed to ping");
		while(1);
	}
	
	//Test the compass
	compass.begin();
	//Get the device ID from the compass
	value = ID_REGA;
	compass.read(&value, 1);
	if(value != 0x48)
	{
		rprintf("Compass failed to ping");
		while(1);
	}
	
	rprintf("Pass!");
	LEDon();
}
