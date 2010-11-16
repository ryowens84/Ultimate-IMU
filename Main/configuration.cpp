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

extern "C"{
#include "rprintf.h"
#include "serial.h"
}

#define GLOBALOBJECT

cConfig configuration;
//char log_string[101];

cConfig::cConfig(void)
{
	configuration.log_sensor=1;
	configuration.log_gps=0;
	configuration.accel_scale=4;
	configuration.compass_gain=10;
	for(int i=0; i<6; i++)configuration.gps_messages[i]=0;
	configuration.gps_messages[4]=1;
	configuration.menu_enabled=0;
}

void configMenu(void){
	char selection=0;
	char exit=0;

	while(!exit){
		rprintf("\n- Ultimate IMU Configuration -\n");
		rprintf("- Version %d.%d -\n", major_version, minor_version);
		rprintf("_______________________________\n");
		rprintf("1.) Log Sensor Data (%d)\n", configuration.log_sensor);
		rprintf("2.) Log GPS Data (%d)\n", configuration.log_gps);
		rprintf("3.) Accelerometer Scale (%d)\n", configuration.accel_scale);
		rprintf("4.) Compass Gain (%d)\n", configuration.compass_gain);
		rprintf("5.) GPS Messages (%d,%d,%d,%d,%d,%d)\n",configuration.gps_messages[0],configuration.gps_messages[1],
			configuration.gps_messages[2],configuration.gps_messages[3],configuration.gps_messages[4],
			configuration.gps_messages[5]);
		rprintf("6.) Calibrate IMU\n");
		rprintf("7.) Exit\n");
		
		rprintf("\nSelection: ");
		selection=getc0();
		rprintf("%c\n\n", selection);
		
		switch (selection)
		{
			case '1':
				rprintf("Log Sensor Data\n");
				rprintf("Currently %d\n", configuration.log_sensor);
				selection = enablePrompt();
				if(selection > '0')configuration.log_sensor=1;
				else configuration.log_sensor=0;
				break;
		
			case '2':
				rprintf("Log GPS Data\n");
				rprintf("Currently %d\n", configuration.log_gps);
				selection=enablePrompt();
				if(selection > '0')configuration.log_gps=1;
				else configuration.log_gps=0;				
				break;
		
			case '3':
				rprintf("Accelerometer Scale\n");
				rprintf("Select Scale:\n1.)2g\n2.)4g\n3.)8g\n4.)16g\n");
				rprintf("Selection: ");
				selection = getc0();
				rprintf("%c\n", selection);
				if(selection > '0' && selection < '5')selection -='0';
				configuration.accel_scale=selection-1;	//Get the bit setting for the g-scale
				break;
			
			case '4':
				rprintf("Compass Gain\n");
				rprintf("Currently %d\n", configuration.compass_gain);
				break;
				
			case '5':
				rprintf("Configure GPS Messages\n");
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
				rprintf("Calibration\n");
				rprintf("Set the IMU in it's normal orientation and protect it from any movement.\n");
				rprintf("Press any key to start calibration");
				rprintf("When calibration is finished, the config menu will be presented and you may\n");
				rprintf("continue normal operation.\n");
				break;
				
			case '7':
				exit=1;
				break;
		
			default:
				exit=1;
				break;
		}
	}	
	
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
	char index=0;
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

	//Get the 'Accelerometer Scale' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.accel_scale = strtol((const char *)result, NULL, 10);	

	//Get the 'Compass Gain' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	result = strtok(NULL, delim);
	configuration.compass_gain = strtol((const char *)result, NULL, 10);

	//Get the 'Enabled GPS Messages' parameter
	if(!configData.readLine(fileData))error=1;
	result = strtok(fileData, delim);
	while(result!=NULL){
		result = strtok(NULL, delim);
		configuration.gps_messages[index++]=strtol((const char *)result, NULL, 10);
	}

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
	sprintf(log_string, "log_sensor=%d\nlog_gps=%d\naccel_scale=%d\ncompass_gain=%d\nmsgs=%d,%d,%d,%d,%d,%d\n",
			configuration.log_sensor, configuration.log_gps, configuration.accel_scale,
			configuration.compass_gain, configuration.gps_messages[0],configuration.gps_messages[1],configuration.gps_messages[2],
			configuration.gps_messages[3],configuration.gps_messages[4],configuration.gps_messages[5]);	
	rprintf("Saving...\n%s", log_string);
	configData.save(log_string);
}