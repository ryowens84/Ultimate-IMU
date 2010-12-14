/*
Small Library containing functions for configuration of the Ultimate IMU
Ryan Owens
11/11/10
*/
#ifndef configuration_h
#define	configuration_h

#include "memory.h"
#include <stdint.h>

class cConfig{
	public:
		cConfig(void);
		char log_sensor;
		char log_gps;
		char range_accel;
		char range_compass;
		char gps_messages[6];
		char sd_enabled;
		char output_euler;
		char output_messages;
		int cal_ax;
		int cal_ay;
		float cal_az;
		int cal_gx;
		int cal_gy;
		int cal_gz;		
};

extern cConfig configuration;

void configMenu(cMemory configData);
void createConfigFile(cMemory configData);
void readConfigFile(cMemory configData);
char enablePrompt(void);
void saveConfigData(cMemory configData);

#endif
