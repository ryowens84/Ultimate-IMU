#ifndef RTC_h
#define RTC_h

#define INT_RTC		(1<<13)

//Interrupt Service Routine for the RTC
void ISR_RTC(void);

class cRTC
{
	public:	
		unsigned char updated;
	
		//Constructor doesn't do anything
		cRTC(void);
		//Used to initialize the RTC to the correct clock source. RTC is started by default
		void begin(unsigned int source);
		//Used to start the RTC
		void start(void);
		//Used to stop the RTC
		void stop(void);
		//Used to enable alarm interrupts
		//(See the RTC_ISR.c file for the Interrupt Service Routine)
		void alarmEnable(unsigned char mask);
		//Used to enable increment alarms
		void incrementEnable(unsigned char mask);
		
		//Used to set the time
		void setTime(unsigned int hour, unsigned int minute, unsigned int second);
		//Used to set the hour
		void setHour(unsigned int hour);
		//Used to set the minute
		void setMinute(unsigned int minute);
		//Used to set the seconds
		void setSecond(unsigned int second);
		//Used to set the day of the week
		void setDow(unsigned int dow);
		//Used to set the day of the year
		void setDoy(unsigned int doy);
		//Used to set the day of the month
		void setDom(unsigned int dom);
		//Used to set the month
		void setMonth(unsigned int month);
		//Used to set year
		void setYear(unsigned int year);
		
		//Can be called to get the current hour
		unsigned int getHour(void);
		//Can be called to get the current minute
		unsigned int getMinute(void);
		//Can be called to get the current seconds
		unsigned int getSecond(void);
		//Can be called to get the current day of the week
		unsigned int getDow(void);
		//Can be called to get the curent day of the year
		unsigned int getDoy(void);
		//Can be used to get the current day of the month
		unsigned int getDom(void);
		//Can be used to get the month
		unsigned int getMonth(void);
		//Can be used to get the year
		unsigned int getYear(void);
		
		//Used to set the alarm second value
		void alarmSecond(unsigned int second);
		//Used to get the alarm second value
		unsigned int alarmSecond(void);
		//Used to set the alarm minute value
		void alarmMinute(unsigned int minute);
		//Used to get the alarm minute vlaue
		unsigned int alarmMinute(void);
		//Used to set the alarm hour
		void alarmHour(unsigned int hour);
		//Used to get the alarm hour
		unsigned int alarmHour(void);
};

//Make the global instance of RTC available
extern cRTC RTC;

/*
RTC Register Bit Definitions
*/

//Alarm Mask Register Bits
#define 	AMRSEC	0
#define		AMRMIN	1
#define		AMRHOUR	2
#define		AMRDOM	3
#define		AMRDOW	4
#define		AMRDOY	5
#define		AMRMON	6
#define		AMRYEAR	7

//counter Increment Interrupt Register Bits
#define 	IMSEC	0
#define		IMMIN	1
#define		IMHOUR	2
#define		IMDOM	3
#define		IMDOW	4
#define		IMDOY	5
#define		IMMON	6
#define		IMYEAR	7

//Clock Control Register Bits
#define	CLKEN	0
#define	CTCRST	1
#define CLKSRC	4

#endif
