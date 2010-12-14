/*
Library for filtering Accelerometer and Gyroscope Values
Based on "A Guide to Using IMU in Embedded Applications" by Starlino
http://starlino.com/imu_guide.html

Rewritten for LPC2148 by Ryan Owens
10/18/10
*/

#include "sensor.h"
#include <stdlib.h>
#include <stdio.h>
#include "LPC21xx_SFE.h"
#include "main.h"
#include <math.h>

#define GLOBALOBJECT

cSensor filter;

cSensor::cSensor(void)
{
	Axz=0, Ayz=0;
	EstAxz=0, EstAyz=0;
	gyro_weight=5;
	signRzGyro=0;
	AccTheta=0, EstTheta=0;
	last_time=0, this_time=0;
	interval=0;
	first_run=2;
}

void cSensor::fillRwAcc(float x, float y, float z)
{
	RwAcc[0] = x;
	RwAcc[1] = y;
	RwAcc[2] = z;
}

void cSensor::normalizeVector(double * vector)
{
	double R;
	R = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
	vector[0] /= R;
	vector[1] /= R;	
	vector[2] /= R;
}
