/*
Library for filtering Accelerometer and Gyroscope Values
Based on "A Guide to Using IMU in Embedded Applications" by Starlino
http://starlino.com/imu_guide.html

Rewritten for LPC2148 by Ryan Owens
10/18/10
*/

#ifndef sensor_h
#define sensor_h

#include <stdint.h>

#define PI	3.1415

class cSensor{
	public:
		cSensor();
		void fillAccelValues(float x, float z);
		void normalizeVector(double * vector);
		
		//unsigned int sensor_readings[3];	//Keep track of Zg, Xg and Xr
		double RwEst[2];	//Esitmated orientation based on RwAcc, RwGyro and a 'weight'
		double RwAcc[2];	//Gravity force Vecor components measured from Accelerometer (measured in G's)
		double RwGyro[2];	//Gyro gravity force vector (uses current and last gyro measurmements and time differential)
		
		double Axz;
		double gyro_weight;
		char signRzGyro;
		double AccTheta, EstTheta;
		unsigned long last_time, this_time;
		double interval;
		double this_angle, this_rate;
		
		char first_run;
};

extern cSensor filter;

#endif
