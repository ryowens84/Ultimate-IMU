#ifndef EM408_h
#define EM408_h

//Assumes Enable Pin is on Port 0
typedef struct{
	char * position;
	char * direction;
} sPosition;

typedef struct{
	sPosition latitude, longitude;
	//char Time[10], Date[6], Speed[6], Heading[6];
	char * time;
	char * status;
	char * speed;
	char * heading;
	char * date;
	char * mag;
	char * cksum;

} sGPSdata;

class cEM408
{
	public:
		//Variables
		sGPSdata data;
		char message[100];
		char updated;
	
		//Constructor doesn't do anything
		cEM408(unsigned int port, unsigned int en_pin);
		//Used to configure LPC for appropriate serial port and baud rate
		//Must be called before any GPS functions can be used.
		void begin(unsigned int baud_rate);
		//Used to enable the GPS module
		void on(void);
		//Used to disable the GPS module
		void off(void);
		//Used to enable a gps message to a specific frequency
		void enable(unsigned const char type, unsigned const char freq);
		//Used to enable all gps messages
		void enable(void);
		//Used to disable a gps message
		void disable(unsigned const char type);
		//Used to disable all gps messages
		void disable(void);
		//Used to parse a GPS message
		//NOTE: Currently only parses RMC strings
		//TODO: recognize message type and parse accordingly
		//Returns: 1 - success
		//		   0 - Invalid Checksum
		//		   -1 - No GPS Fix
		int parse(char * inmessage);
		
	private:
		//Used to configure the frequency of a GPS message type
		void configure(unsigned const char type, unsigned const char freq);		
		unsigned int _en_pin;
		unsigned int _port;
};
extern cEM408 gps;

#define GPS_CHECKSUM(mstr, mx)  {mx=0; for(int mi = 0; mi < strlen(mstr);mi++ ) mx ^= mstr[mi];}

#define GGA	0
#define	GLL	1
#define GSA	2
#define GSV	3
#define	RMC	4
#define VTG	5

#endif
