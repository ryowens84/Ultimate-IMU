/*
*	I2C Library
*	Class for using the I2C hardware
*	(Does not use interrupts)
*	Written By Ryan Owens
*	SparkFun Electronics
*	8/25/10
*/

#include "I2C.h"
#include <stdlib.h>
#include <stdio.h>
//#include "LPC214x.h"
#include "LPC21xx_SFE.h"
#include "main.h"
#define GLOBALOBJECT

I2C::I2C(int i2c_port)
{
	_i2c_port = i2c_port;
}

I2C::I2C(void)
{
	//do nothing
}

void I2C::configure(void)
{
	//Enable the Port 0 I2C pins
	//NOTE: This should be modified to take the _i2c_port as an argument
	PINSEL0 |= 0x00000050;
	
	//Configure the I2C Clock for 100 kHz rate and 50% duty cycle
	I2SCLH = 295;
	I2SCLL = 295;
}

char I2C::send(char SLA, char * contents, char direction, char length)
{
	char STATE, error=0, exit=0;

	//Initialize the I2CONSET register
	I2CONSET = (1<<I2EN);	//Enable the I2C module;
	//I2CONCLR = (1<<SIC);	//Make sure the SI bit is cleared.
	
	//Set the Start Bit to start I2C communication
	I2CONSET |= (1<<STA);	//Send the Start Bit.
	I2CONCLR = (1<<SIC);		//Make sure the SI bit is cleared.
	
	
	while(!exit && !error){
		//rprintf("I2CON: %X, length: %d, error: %d\n\r", I2CONSET, length, error);
		//rprintf("Waiting...");
		while(!(I2CONSET & (1<<SI)));
		//rprintf("ACK\n\r");
		STATE = I2STAT;	//Get the value in the status register
		switch (STATE){
			//The Start condition has been sent.Load the Slave Device Address and the direction bit into the Data Register
			case 0x08:	//rprintf("Start Sent\n\r");
						I2DAT = SLA+direction; //Put the Slave Device Address into the data register.
						I2CONCLR = ((1<<STA)|(1<<STO)|(1<<SI));	//Don't send another start condition
						
				break;
			//A repeated START condition has been sent (shouldn't happen! If it does there was an error)
			case 0x10:	error=1;
						//rprintf("Error 10");
				break;
			//SLA+W has been written and ACK has been received.
			case 0x18:	//rprintf("Write address Sent");
						if(length-- > 0){
							//rprintf("Writing Contents, %d left\n\r", length);
							I2DAT = *contents++;	//Write the contents to be sent to the I2C module
							I2CONCLR = (1<<SI);
							//rprintf(" %x\n\r", I2CONSET);
						}
						else{
							error=1;	//IF the length was 0 or less there is nothing to write!
							//rprintf("Error 18");
						}
				break;
			//SLA+W has been writtten and NOT ACK was received.
			case 0x20: 	I2CONCLR = (1<<STA);
						error=1;	 //This shouldn't happen!
						//rprintf("Error 20");
				break;
			//Data byte has been transmitted and ACK was received.
			case 0x28:	//rprintf("Data sent\n\r");
						if(length-- > 0){
							I2DAT = *contents++;	//If there's more data to send then load it up!
							//rprintf("Writing Contents, %d left", length);
							I2CONCLR = (1<<STA)|(1<<STO)|(1<<SI);
						}
						else{
							//rprintf("Finished Writing");
							exit=1;
							I2CONSET = (1<<STO);
							I2CONCLR = (1<<STA)|(1<<SI);
						}
						//Else we're finished and can send the stop condition
				break;
			//Data byte has been transmitted and NOT ACK was received
			case 0x30: 	I2CONCLR = (1<<STA);
						I2CONSET = (1<<STO);
						error=1;	//This should happen.
						//rprintf("Error 30");
				break;
			//Arbitration lost in NOT ACK bit (don't even know what this means! definately an error)
			case 0x38: 	error=1;
						//rprintf("Error 38");
				break;
			//SLA+R has been sent and an ACK has been received. Make sure ack is configured and then data will be automatically received by the I2C module
			case 0x40:	//rprintf("Read Address sent\n\r");
						if(--length > 0)I2CONSET = (1<<AA);	//If there's more info to read enable the AcK
						else I2CONCLR = (1<<AA);	//If we're done, disable the ack so we can send the stop bit
						I2CONCLR = (1<<STA)|(1<<STO)|(1<<SI);
				break;
			//SLA+R has been sent and NOT ACK was received (This is an error, shouldn't happen
			case 0x48: 	I2CONCLR = (1<<STA);
						error=1;
						//rprintf("Error 48");
				break;
			//Data byte has been received and ACK has been returned
			case 0x50:	if(--length > 0)I2CONSET = (1<<AA);	//If there's more info to read enable the AcK
						else I2CONCLR = (1<<AA);	//If we're done, disable the ack so we can send the stop bit
						//rprintf("Reading Contents...");
						*contents++=I2DAT;	//Read the received contents of the I2C module.
						I2CONCLR = (1<<STA)|(1<<STO)|(1<<SI);
				break;
			//Data byte has been received and NOT ACK has been returned
			case 0x58:  if(length > 0){
							error=1;	//If there was more data to read and we got a NOT ACK there was an error
							//rprintf("Error 58");
						}
						else{ *contents++=I2DAT;	//Load the received I2C contents
							//rprintf("Reading Contents...");
						}
						I2CONCLR |= (1<<STA)|(1<<SI);
						I2CONSET |= (1<<STO);
						exit=1;
				break;
			default:	error=1;
						//rprintf("Unknown Error");
				break;
		}
		
	}
							
	if(error==1)return 0;
	else return 1;
}
