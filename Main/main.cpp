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

#include "uart0.h"
#include "uart0ISR.h"
#include "uart1.h"
#include "uart1ISR.h"
}

//*******************************************************
//						C++ Libs
//*******************************************************
#include "main.h"
#include "configuration.h"
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
void runTest(void);
void initPeripherals(void);

//*******************************************************
//					Global Variables
//*******************************************************
//Version Information.
//KEEP THIS UPDATED!
char major_version=0;
char minor_version=1;

char sensors_updated=0;
char sensor_string[70]="";
char sensor_log_string[255]="";

cMemory sensorData;
cMemory gpsData;
cMemory configData;

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
	XBEEon();	//XBee can always be enabled. To get battery savings...take the XBEE module off!
	LEDoff();

	timer0Init(1000000);
	timer0Match(0, 1000, interruptOnMatch | resetOnMatch);
	//Timer 1 doesn't need to be initialized. It's being used for delay_ms()
	
	//Find out if the device needs to go through the test procedure for QA.
	if(configuration.sd_enabled){
		if(memoryExists("Test.txt")){
			rprintf("Testing...");	
			runTest();
			while(1);
		}
	}
	
	//Get the configuration settings
	if(configuration.sd_enabled){
		if(memoryExists("configuration.txt")){
			readConfigFile(configData);
		}
		//If the configuration file doesn't exist, create it with default settings and reset the device.
		else
		{
			createConfigFile(configData);
			reset();
		}
	}

	LEDon();
	//Initialize peripherals with settings from configuration file.
	initPeripherals();
	delay_ms(100);	

	//Create the necessary log files
	if(configuration.log_sensor)sensorData.create("Sensor", ".csv");
	if(configuration.log_gps)gpsData.create("NMEA", ".csv");

	//Set the UART interrupts to trigger when a single character is received
	uart1RxInt(RX1_TRIG_LEV_0);
	uart0RxInt(RX0_TRIG_LEV_0);
	
	LEDoff();
	VICIntEnable |= INT_UART0;
	if(configuration.log_gps)VICIntEnable |= INT_UART1;
	if(configuration.log_sensor)VICIntEnable |= INT_TIMER0;
	while(1)
	{
		if(timer0IntFlag>10)
		{
			VICIntEnClr |= INT_TIMER0;
			
			if(timer0IntFlag >= 20)LEDon();
			else LEDoff();
			
			timer0IntFlag=0;
			
			accelerometer.update();
			gyro.update();
			compass.update();
			sensors_updated=1;	
			
			VICIntEnable |= INT_TIMER0;
		}
		
		if(uart1MessageComplete)
		{
			VICIntEnClr |= INT_UART1;
			uart1MessageComplete=0;
			
			if(configuration.log_gps){
				strcpy(gps.message, uart1Message);
				gpsData.save(gps.message);
			}

			if(configuration.output_messages >=2)rprintf("%s\n", gps.message);
			
			VICIntEnable |= INT_UART1;
		}
		
		if(uart0MessageComplete)
		{
			VICIntEnClr |= INT_TIMER0|INT_UART1|INT_UART0;
			uart0MessageComplete=0;
			
			//Save all the current data and close the files.
			if(configuration.log_gps)gpsData.close();
			if(configuration.log_sensor){
				sensorData.save(sensor_log_string);
				strcpy(sensor_log_string, "");
				sensorData.close();
			}
			
			configMenu(configData);
			configData.close();
			
			//Configure the sensors with the new settings
			initPeripherals();
			
			//Create new files, if needed.
			if(configuration.log_sensor)sensorData.create("Sensor", ".csv");
			if(configuration.log_gps)gpsData.create("NMEA", ".csv");			
			
			VICIntEnable |= INT_TIMER0|INT_UART1|INT_UART0;
		}
		
		if(sensors_updated)
		{
			sensors_updated=0;
			
			//If we're not supposed to output the euler angles, just copy the time and sensor values to the string.
			//(Sensor Values are in engineering units - g's, degrees/sec, and Ga)
			if(!configuration.output_euler){
				sprintf(sensor_string, "%06ld,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f\n\r",
						millis(),
						gyro.getX(), gyro.getY(), gyro.getZ(),
						accelerometer.getX(), accelerometer.getY(), accelerometer.getZ(),
						compass.getX(), compass.getY(), compass.getZ());
			}
			else{
				//Calculate Euler Angles and tilt compensated heading
				filter.last_time=filter.this_time;
				filter.this_time=millis();	//Get the current number of milliseconds
				//Calculate Interval Time in milliseconds
				filter.interval=filter.this_time-filter.last_time;
				
				//Populate the RwAcc Array
				filter.fillRwAcc(accelerometer.getX(), accelerometer.getY(), accelerometer.getZ());		
				
				//Normalize the Accelerometers gravity vector
				filter.normalizeVector(filter.RwAcc);
				
				//The measurements of the first run get screwed up since we don't have a previous angle
				//to derive the current angle from. So, we skip the calculation on the first time through this loop.
				if(filter.first_run)
				{
					for(int w=0; w<3; w++)filter.RwGyro[w] = filter.RwAcc[w];
					filter.first_run=0;			
				}
				else
				{
					//If the previous estimated values is too small, don't calc. a new one as the error will be large.
					if(filter.RwEst[2] < 0.1)
					{
						for(int w=0; w<3; w++)filter.RwGyro[w]=filter.RwEst[w];
					}
					//Else, find the 'gyro angle' and calculate the weighted average to find attitude of device.
					else
					{
						//Get the current deg/sec from gyroscope.
						filter.x_rate=gyro.getX();	
						filter.y_rate=gyro.getY();
						//find the Angle difference between the last reading and this one.
						filter.x_angle=filter.x_rate*(filter.interval/1000.0);	//degree/sec * seconds == degrees
						filter.y_angle=filter.y_rate*(filter.interval/1000.0);
						
						//Find the current angle based on the previously measured angle
						filter.Axz = atan2(filter.RwEst[0], filter.RwEst[2])*180/PI;	//Get previous angle in degrees
						filter.Axz += filter.x_angle;	//Add the current angle to the previous one to get current angle.
						filter.Ayz = atan2(filter.RwEst[1], filter.RwEst[2])*180/PI;
						filter.Ayz += filter.y_angle;
					}
					
					if(filter.RwAcc[2] >=0)filter.signRzGyro=1;
					else filter.signRzGyro=-1;

					//Use Axz to find RxGyro
					filter.RwGyro[0] = sin(filter.Axz * (PI/180));
					filter.RwGyro[0] /= sqrt(1+cos(filter.Axz * (PI/180))*cos(filter.Axz * (PI/180))
											*tan(filter.Ayz*(PI/180))*tan(filter.Ayz*(PI/180)));
					//Use Ayz to find RyGyro
					filter.RwGyro[1] = sin(filter.Ayz * (PI/180));
					filter.RwGyro[1] /= sqrt(1+cos(filter.Ayz * (PI/180))*cos(filter.Ayz * (PI/180))
											*tan(filter.Axz*(PI/180))*tan(filter.Axz*(PI/180)));
					
					filter.RwGyro[2] = filter.signRzGyro * sqrt(1-pow(filter.RwGyro[0],2)-pow(filter.RwGyro[1],2));	
				}
				//Now we have the gravity force vector from both accelerometer and gyro. Combine them using weighted average
				//to find Rw
				for(int w=0; w<3; w++)
				{
					filter.RwEst[w] = (filter.RwAcc[w] + filter.RwGyro[w] * filter.gyro_weight)/(1+filter.gyro_weight);
				}	
				filter.normalizeVector(filter.RwEst);
				
				//filter.AccTheta=atan2(filter.RwAcc[0], filter.RwAcc[2])*180/PI;
				filter.EstAxz=atan2(filter.RwEst[0], filter.RwEst[2])*180/PI;
				filter.EstAyz=atan2(filter.RwEst[1], filter.RwEst[2])*180/PI;
				
				//Now that we have the euler angles, calculate the tilt-compensated heading.
				filter.RwMag[0] = compass.getX();
				filter.RwMag[1] = compass.getY();
				filter.RwMag[2] = compass.getZ();
				
				//Find the X and Y heading values using the tilt information
				filter.x_h = filter.RwMag[0]*cos(filter.EstAyz*(PI/180))+
					filter.RwMag[1]*sin(filter.EstAxz*(PI/180))*sin(filter.EstAyz*(PI/180))-
					filter.RwMag[2]*cos(filter.EstAxz*(PI/180))*sin(filter.EstAyz*(PI/180));
					
				filter.y_h=filter.RwMag[1]*cos(filter.EstAxz*(PI/180))+filter.RwMag[2]*sin(filter.EstAxz*(PI/180));
				
				//Find the heading.
				if(filter.x_h < 0)filter.heading = 180 - atan2(filter.y_h, filter.x_h)*180/PI;
				else if((filter.x_h > 0) && (filter.y_h < 0))filter.heading = -atan2(filter.y_h, filter.x_h)*180/PI;
				else if((filter.x_h > 0) && (filter.y_h > 0))filter.heading = 360 - atan2(filter.y_h, filter.x_h)*180/PI;
				else if((filter.x_h == 0)&& (filter.y_h < 0))filter.heading = 90;
				else if((filter.x_h == 0)&& (filter.y_h > 0))filter.heading = 270;
				
				sprintf(sensor_string, "%06ld,%5.2f,%5.2f,%5.2f\n\r", millis(), filter.EstAxz, filter.EstAyz, filter.heading);	
			}
			
			if(configuration.log_sensor){
				//Copy the data to a larger buffer. This keeps the number of 'saves' lower, which results
				//in less overruns in reading the sensors.
				strcat(sensor_log_string, sensor_string);
				if(strlen(sensor_log_string) > 195)
				{
					sensorData.save(sensor_log_string);
					strcpy(sensor_log_string, "");
				}
			}
			
			if(configuration.output_messages == 1 || configuration.output_messages ==3){
				rprintf("%s", sensor_string);
			}
			
		}
		
		//If a USB Cable gets plugged in, stop everything!
		if(IOPIN0 & (1<<23))
		{
			VICIntEnClr = INT_TIMER0 |INT_UART1;	//Stop all running interrupts			
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
	VICIntSelect = ~(INT_TIMER0|INT_UART1|INT_UART0);
	VICVectCntl0 = 0x20 | 4;						//Timer 0 Interrupt
	VICVectAddr0 = (unsigned int)ISR_Timer0;
	VICVectCntl1 = (0x20 | 7);
	VICVectAddr1 = (unsigned int)ISR_UART1;			//UART 1 Interrupt
	VICVectCntl2 = (0x20 | 6);
	VICVectAddr2 = (unsigned int)ISR_UART0;			//UART 1 Interrupt
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

void initPeripherals(void)
{
	//Configure the sensor settings.
	accelerometer.begin(configuration.range_accel);
	gyro.begin();
	compass.begin(configuration.range_compass);	
	
	//set the sensor offsets
	accelerometer.setCalibrationValues(configuration.cal_ax, configuration.cal_ay, configuration.cal_az);
	gyro.setCalibrationValues(configuration.cal_gx, configuration.cal_gy, configuration.cal_gz);
	
	gps.begin(4800);
	delay_ms(100);
	gps.on();
	delay_ms(500);
	gps.disable();
	delay_ms(50);
	for(int i=0; i<6; i++)
	{
		if(configuration.gps_messages[i]==1)gps.enable(i, 1);
		delay_ms(50);
	}
	
	delay_ms(100);
}

void runTest(void)
{
	char value;
	int gpstest;
	
	//Set the UART0 pins to I/O for initial XBee test
	PINSEL0 &= ~((3<<0) | (3<<2));	//Set P0.0 and P0.1 to GPIO
	IODIR0 |= (1<<0)|(1<<1);	//Set P0.0 and P0.1 to outputs
	
	for(int blink=0; blink < 10; blink++)
	{
		IOSET0 = (1<<0)|(1<<1);	//Turn on P0.0 and P0.1
		delay_ms(50);
		IOCLR0 = (1<<0)|(1<<1);
		delay_ms(50);
	}	
	
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
	accelerometer.begin(1);
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
	compass.begin(1);
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
	memoryDelete("Test.txt");
}


