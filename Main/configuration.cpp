/*
Small Library containing functions for configuration of the Ultimate IMU
Ryan Owens
11/11/10
*/

#include "main.h"
#include "configuration.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "ADXL345.h"
#include "ITG3200.h"

extern "C"{
#include "rprintf.h"
#include "serial.h"
}

#define GLOBALOBJECT

cConfig configuration;
char log_string[101];

cConfig::cConfig(void)
{
	configuration.log_sensor=1;
	configuration.log_gps=0;
	configuration.range_accel=1;
	configuration.range_compass=1;
	for(int i=0; i<6; i++)configuration.gps_messages[i]=0;
	configuration.gps_messages[4]=1;
	configuration.sd_enabled=1;
	configuration.output_euler=1;
	configuration.output_messages=2;
	configuration.cal_ax=0;
	configuration.cal_ay=0;
	configuration.cal_az=0;
	configuration.cal_gx=0;
	configuration.cal_gy=0;
	configuration.cal_gz=0;
}

void configMenu(cMemory configData){
	char selection=0;
	char exit=0;

	while(!exit){
		rprintf("\n- Ultimate IMU Configuration -\n");
		rprintf("- Version %d.%d -\n", major_version, minor_version);
		rprintf("_______________________________\n");
		rprintf("1.) Log Sensor Data ");
			if(configuration.log_sensor)rprintf("(Enabled)\n");
			else rprintf("(Disabled\n");
		rprintf("2.) Log GPS Data ");
			if(configuration.log_gps)rprintf("(Enabled)\n");
			else rprintf("(Disabled)\n");		
		rprintf("3.) Accelerometer Range (+/-");
			if(configuration.range_accel==0)rprintf("2g)\n");
			else if(configuration.range_accel==1)rprintf("4g)\n");
			else if(configuration.range_accel==2)rprintf("8g)\n");
			else rprintf("16g)\n");
		rprintf("4.) Compass Range (+/-");
			switch(configuration.range_compass){
				case 0:rprintf("0.7");
					break;
				case 1:rprintf("1.0");
					break;
				case 2:rprintf("1.5");
					break;
				case 3:rprintf("2.0");
					break;
				case 4:rprintf("3.2");
					break;
				case 5:rprintf("3.8");
					break;
				case 6:rprintf("4.5");
					break;
				case 7:rprintf("6.5");
					break;
				default: 
					break;
			}
			rprintf("Ga)\n");
		rprintf("5.) GPS Messages (");
			if(configuration.gps_messages[0])rprintf("GGA, ");
			if(configuration.gps_messages[1])rprintf("GLL, ");
			if(configuration.gps_messages[2])rprintf("GSA, ");
			if(configuration.gps_messages[3])rprintf("GSV, ");
			if(configuration.gps_messages[4])rprintf("RMC, ");
			if(configuration.gps_messages[5])rprintf("VTG, ");
			rprintf(")\n");
		rprintf("6.) Calibrate IMU\n");
		rprintf("7.) Sensor Output (");
			if(configuration.output_euler)rprintf("Euler)\n");
			else rprintf("Raw)\n");
		rprintf("8.) Serial Output Messages (");
			if(configuration.output_messages==0)rprintf("None)\n");
			else if(configuration.output_messages==1)rprintf("Sensor Data)\n");
			else if(configuration.output_messages==2)rprintf("GPS Data)\n");
			else rprintf("Sensor and GPS data)\n");
		rprintf("9.) Exit\n");
		
		rprintf("\nSelection: ");
		selection=getc0();
		rprintf("%c\n\n", selection);
		
		switch (selection)
		{
			case '1':
				rprintf("\nLog Sensor Data\n");
				selection = enablePrompt();
				if(selection > '0')configuration.log_sensor=1;
				else configuration.log_sensor=0;
				break;
		
			case '2':
				rprintf("\nLog GPS Data\n");
				selection=enablePrompt();
				if(selection > '0')configuration.log_gps=1;
				else configuration.log_gps=0;				
				break;
		
			case '3':
				rprintf("\nAccelerometer Range\n");
				rprintf("Select Range:\n1.)2g\n2.)4g\n3.)8g\n4.)16g\n");
				rprintf("Selection: ");
				selection = getc0();
				rprintf("%c\n", selection);
				if(selection > '0' && selection < '5'){
					selection -='0';
					configuration.range_accel=selection-1;	//Get the bit setting for the g-scale
				}
				break;
			
			case '4':
				rprintf("\nCompass Range\n");
				rprintf("Select Range:\n1.) +/-0.7Ga\n2.) +/-1.0Ga\n3.) +/-1.5Ga\n 4.) +/-2.0Ga\n");
				rprintf("5.) +/-3.2Ga\n6.) +/-3.8Ga\n7.) +/-4.5Ga\n8.) +/-6.5Ga\n");
				rprintf("Selection: ");
				selection = getc0();
				rprintf("%c\n", selection);
				if(selection > '0' && selection < '9'){
					selection -='0';
					configuration.range_compass = selection -1;
				}
				break;
				
			case '5':
				rprintf("\nConfigure GPS Messages\n");
				for(int message=0; message<6; message++)
				{
					if(message==0)rprintf("GGA Messages ");
					if(message==1)rprintf("GLL Messages ");
					if(message==2)rprintf("GSA Messages ");
					if(message==3)rprintf("GSV Messages ");
					if(message==4)rprintf("RMC Messages ");
					if(message==5)rprintf("VTG Messages ");
					rprintf("%d\n", configuration.gps_messages[message]);
					selection = enablePrompt();
					if(selection > '0')configuration.gps_messages[message]=1;
					else configuration.gps_messages[message]=0;
				}
				break;
				
			case '6':
				rprintf("\nCalibration\n");
				rprintf("Set the IMU in it's normal orientation and protect it from any movement.\n");
				rprintf("Press any key to start calibration\n");
				rprintf("When calibration is finished, the config menu will be presented and you may\n");
				rprintf("continue normal operation.\n");
				selection = getc0();
				rprintf("Starting Calibration, don't move the device!\n");
				accelerometer.calibrate();
				gyro.calibrate();
				configuration.cal_ax=accelerometer.getXcal();
				configuration.cal_ay=accelerometer.getYcal();
				configuration.cal_az=accelerometer.getZcal();
				configuration.cal_gx=gyro.getXcal();
				configuration.cal_gy=gyro.getYcal();
				configuration.cal_gz=gyro.getZcal();
				rprintf("Finished!\n");
				break;
				
			case '7':
				rprintf("\nChoose 'enable' for Euler Angle Output or 'disable' for Raw Output\n");
				rprintf("Setting will apply to the log file data (if enabled) and the serial output (if enabled)\n");
				selection = enablePrompt();
				if(selection == '0')configuration.output_euler=0;
				else configuration.output_euler=1;
				break;
		
			case '8':
				rprintf("\nChoose which messages to send to the serial port.\n");
				rprintf("Caution: The more messages that are enabled, the slower the sensors are updated!\n");
				rprintf("1.) No messages\n2.) Sensor Data\n3.) GPS Data\n4.) Sensor and GPS Data\n");
				rprintf("Selection: ");
				selection = getc0();
				rprintf("%c\n", selection);
				if(selection > '0' && selection < '5'){
					selection -= '0';
					configuration.output_messages = selection -1;
				}
				break;
			default:
				exit=1;
				break;
		}
	}
	configData.close();
	configData.open("configuration.txt");
	saveConfigData(configData);
}

void createConfigFile(cMemory configData){
	configData.create("configuration.txt");
	
	configData.open();
	saveConfigData(configData);
	configData.close();
}

void readConfigFile(cMemory configData){
	char fileData[20];
	char error=0;
	int index=0;
	char delim[]="=,";
	char * result = NULL;


	configData.open("configuration.txt");
	
	//Get the 'Log Sensor Data' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.log_sensor = strtol((const char *)result, NULL, 10);

	//Get the 'Log GPS Data' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.log_gps = strtol((const char *)result, NULL, 10);	

	//Get the 'Accelerometer Range' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.range_accel = strtol((const char *)result, NULL, 10);	

	//Get the 'Compass Range' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.range_compass = strtol((const char *)result, NULL, 10);

	//Get the 'Enabled GPS Messages' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	while(result!=NULL){
		result = strtok(NULL, delim);
		configuration.gps_messages[index++]=strtol((const char *)result, NULL, 10);
	}
	
	//Get the 'Euler Format' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.output_euler = strtol((const char *)result, NULL, 10);
	
	//Get the 'Output messages' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.output_messages = strtol((const char *)result, NULL, 10);
	
	//Get the 'Accel X Calibration' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.cal_ax = strtol((const char *)result, NULL, 10);	

	//Get the 'Accel Y Calibration' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.cal_ay = strtol((const char *)result, NULL, 10);		
	
	//Get the 'Accel Z Calibration' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.cal_az = strtof((const char *)result, NULL);	

	//Get the 'Gyro X Calibration' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.cal_gx = strtol((const char *)result, NULL, 10);	

	//Get the 'Gyro Y Calibration' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.cal_gy = strtol((const char *)result, NULL, 10);		
	
	//Get the 'Gyro Z Calibration' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.cal_gz = strtol((const char *)result, NULL, 10);	

	if(error==1){
		rprintf("Invalid configuration.txt file");
		configData.close();
		memoryDelete("configuration.txt");
		reset();
	}
	configData.close();
}

char enablePrompt(void)
{
	char input;
	rprintf("Press 1 to enable or 0 to disable.\n");
	rprintf("Selection: ");
	input = getc0();
	rprintf("%c\n", input);
	return input;
}

void saveConfigData(cMemory configData)
{
	sprintf(log_string, "log_sensor=%d\nlog_gps=%d\nrange_accel=%d\nrange_compass=%d\nmsgs=%d,%d,%d,%d,%d,%d\neuler=%d\nserial=%d\n",
			configuration.log_sensor, configuration.log_gps, configuration.range_accel,
			configuration.range_compass, configuration.gps_messages[0],configuration.gps_messages[1],configuration.gps_messages[2],
			configuration.gps_messages[3],configuration.gps_messages[4],configuration.gps_messages[5],
			configuration.output_euler, configuration.output_messages);	
	//rprintf("Saving...\n%s", log_string);
	configData.save(log_string);
	
	sprintf(log_string, "cal_ax=%d\ncal_ay=%d\ncal_az=%5.2f\ncal_gx=%d\ncal_gy=%d\ncal_gz=%d\n",
			configuration.cal_ax, configuration.cal_ay, configuration.cal_az,
			configuration.cal_gx, configuration.cal_gy, configuration.cal_gz);
	configData.save(log_string);
}
