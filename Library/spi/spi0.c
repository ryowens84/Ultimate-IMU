/*
    SPI Communication for LPC2148 SPI0.
	2/1/2008
	Ryan Owens
	
	NOTES:
	SCLK_PINSEL, MISO_PINSEL and MOSI_PINSEL must be defined in an external header file.  The definitions should
	correspond to the value that needs to be set in the PINSEL0 register.

*/

#include "spi0.h"
#include "LPC214x.h"
#include "delay.h"
#include <stdio.h>

void SPI0_Init(void)		//This function needs to go in bootup() of Main.c
{	
	PINSEL0 = (PINSEL0 & ~(3 << 8)) | (1 << 8); // Enable SCLK0 on P0.4
	PINSEL0 = (PINSEL0 & ~(3 << 10)) | (1 << 10); // Enable MISO0 on P0.5
	PINSEL0 = (PINSEL0 & ~(3 << 12)) | (1 << 12); // Enable MOSI0 on P0.6

}

void SPI0_send(char c)
{
    SPI0_send_recv(c);
}

char SPI0_recv(void)
{
    return SPI0_send_recv(0xff);
}

char SPI0_send_recv(char c)
{
    char in;

    S0SPDR = c;						//Place data to be sent into SPI data register
    while(!(S0SPSR & SPIF));		//Wait for transfer to complete
    in = S0SPDR&0xff;				//Return the character placed in the SPI data register by the slave
    return in;
}

