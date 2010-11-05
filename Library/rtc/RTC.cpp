/*
	RTC Library
	
	by Ryan Owens
	http://www.sparkfun.com
 
	Example Code:
	RTC.begin(1);	//Initialize the RTC to use the external clock
	RTC.incrementEnable(1<<IMMIN);	//Enable Increment Interrupts on the Minute update	
 	//Make sure the RTC interrupts are enabled
	VICIntEnable = INT_RTC;
	//Wait for an update(triggered by the RTC interrupt)
	while(!RTC.updated);
	RTC.updated=0;
	rprintf("%02d:%02d:%02d\r", RTC.getHour(), RTC.getMinute(), RTC.getSecond());
*/
#include "RTC.h"
#include <stdlib.h>
#include <stdio.h>
#include "LPC214x.h"
#include "main.h"

extern "C"{
#include "rprintf.h"
}

#define GLOBALOBJECT
#define DEBUG

//Create a 'global' instance of the RTC class
cRTC RTC;

cRTC::cRTC(void)
{
	//Do Nothing
}

//Used to initialize the RTC to the correct clock source. RTC is started by default
void cRTC::begin(unsigned int source)
{
	CCR = (1<<CTCRST);	//Disable and reset the RTC
	
	//Reset the time
	HOUR=0;
	MIN=0;
	SEC=0;
	
	//Initialized the 'updated' flag
	updated=0;
	
	//If source is 0, set prescaler as RTC source
	if(source == 0)
		CCR &= ~(1<<CLKSRC);
	//If source is 1, set external 32.758 kHz clock as source
	else
		CCR |= (1<<CLKSRC);
	
	CCR &= ~(1<<CTCRST);	//Take the RTC out of reset
	CCR |= (1<<CLKEN);	//Start the RTC.
}

//Used to start the RTC
void cRTC::start(void)
{
	CCR |= (1<<CLKEN);
}

//Used to stop the RTC
void cRTC::stop(void)
{
	CCR &= ~(1<<CLKEN);
}

//Used to enable alarm interrupts
//(See the RTC_ISR.c file for the Interrupt Service Routine)
//Inputs: mask - The mask which enables different components (HOUR, MIN, SEC, etc...) of the alarm
//NOTE: See RTC.h for the bit definitions that can be used for the mask setting.
void cRTC::alarmEnable(unsigned char mask)
{
	AMR = mask;	//Enable the alarm masks for the Alarm
}

//Used to enable increment alarms
//(See the RTC_ISR.c file for the Interrupt Service Routine)
//Inputs: mask - The mask which enables different components (HOUR, MIN, SEC, etc...) of the alarm
//NOTE: See RTC.h for the bit definitions that can be used for the mask setting.
void cRTC::incrementEnable(unsigned char mask)
{
	CIIR = mask;
}

//Used to set the time
void cRTC::setTime(unsigned int hour, unsigned int minute, unsigned int second){
	HOUR = hour;
	MIN = minute;
	SEC = second;
}

//Used to set the hour
void cRTC::setHour(unsigned int hour)
{
	HOUR = hour;
}

//Used to set the minute
void cRTC::setMinute(unsigned int minute)
{
	MIN = minute;
}

//Used to set the seconds
void cRTC::setSecond(unsigned int second)
{
	SEC = second;
}

//Used to set the day of the week
void cRTC::setDow(unsigned int dow)
{
	DOW = dow;
}

//Used to set the day of the year
void cRTC::setDoy(unsigned int doy)
{
	DOY = doy;
}

//Used to set the day of the month
void cRTC::setDom(unsigned int dom)
{
	DOM = dom;
}

//Used to set the month
void cRTC::setMonth(unsigned int month)
{
	MONTH = month;
}

//Used to set year
void cRTC::setYear(unsigned int year)
{
	YEAR = year;
}

//Can be called to get the current hour
unsigned int cRTC::getHour(void)
{
	return HOUR;
}

//Can be called to get the current minute
unsigned int cRTC::getMinute(void)
{
	return MIN;
}

//Can be called to get the current seconds
unsigned int cRTC::getSecond(void)
{
	return SEC;
}

//Can be called to get the current day of the week
unsigned int cRTC::getDow(void)
{
	return DOW;
}

//Can be called to get the curent day of the year
unsigned int cRTC::getDoy(void)
{
	return DOY;
}

//Can be used to get the current day of the month
unsigned int cRTC::getDom(void)
{
	return DOM;
}

//Can be used to get the month
unsigned int cRTC::getMonth(void)
{
	return MONTH;
}

//Can be used to get the year
unsigned int cRTC::getYear(void)
{
	return YEAR;
}

//Used to set the alarm second value
void cRTC::alarmSecond(unsigned int second)
{
	ALSEC = second;
}

//Used to get the alarm second value
unsigned int cRTC::alarmSecond(void)
{
	return ALSEC;
}

//Used to set the alarm minute value
void cRTC::alarmMinute(unsigned int minute)
{
	ALMIN = minute;
}

//Used to get the alarm minute vlaue
unsigned int cRTC::alarmMinute(void)
{
	return ALMIN;
}

//Used to set the alarm hour
void cRTC::alarmHour(unsigned int hour)
{
	ALHOUR = hour;
}

//Used to get the alarm hour
unsigned int cRTC::alarmHour(void)
{
	return ALHOUR;
}

//Usage: None (Automatically Called by FW)
//Inputs: None
//Description: Called when the RTC alarm goes off.  This wakes
//				the Package Tracker from sleep mode.
void ISR_RTC(void)
{	
	//Clear the Alarm Interrupt bit from the ILR
	ILR = ((1<<1)|(1<<0));
	//RTC_Set=1;	//Set the RTC Increment Flag
	RTC.updated=1;
	//wake_event=RTC_TIMEOUT_WAKE;
	VICVectAddr =0;		//Update the VIC priorities
}
