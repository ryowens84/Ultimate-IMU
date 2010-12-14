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
		void fillRwAcc(float x, float y, float z);
		void normalizeVector(double * vector);
		
		double RwEst[3];	//Esitmated orientation based on RwAcc, RwGyro and a 'weight'
		double RwAcc[3];	//Gravity force Vecor components measured from Accelerometer (measured in G's)
		double RwGyro[3];	//Gyro gravity force vector (uses current and last gyro measurmements and time differential)
		double RwMag[3];	//Magenetic vector
		
		double Axz, Ayz;
		double EstAxz, EstAyz;
		double gyro_weight;
		char signRzGyro;
		double AccTheta, EstTheta;
		unsigned long last_time, this_time;
		double interval;
		double x_angle, x_rate, y_angle, y_rate;
		double x_h, y_h;
		double heading;
		
		
		char first_run;
};

extern cSensor filter;

#endif
