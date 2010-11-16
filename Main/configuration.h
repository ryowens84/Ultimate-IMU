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
		char sensor_freq;
		char accel_scale;
		char compass_gain;
		char gps_messages[6];
		char menu_enabled;
};

extern cConfig configuration;

void configMenu(void);
void createConfigFile(cMemory configData);
void readConfigFile(cMemory configData);
char enablePrompt(void);
void saveConfigData(cMemory configData);

#endif
