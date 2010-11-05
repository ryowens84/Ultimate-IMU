/*
	Memory Library
	
	Used as an interface for the sdFat library
	
	by Ryan Owens
	http://www.sparkfun.com
	
	Example Code:
	test.create("Test", ".csv");
	test.save("I got it working!");
	test.close();	
 
*/
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "LPC214x.h"
#include "string.h"


extern "C"{
	#include "rootdir.h"
	#include "fat16.h"
	#include "sd_raw.h"
	//#include "delay.h"
}

#define GLOBALOBJECT
#define DEBUG

cMemory::cMemory(void)
{
	handle=NULL;
}

int cMemory::create(const char * name, const char * extension)
{
	int file_number=0;
	//char * file_name;

	//&file_name[0]=strcpy(&file_name[0], name);
	strcpy(&file_name[0], name);
	sprintf(file_name+strlen(file_name), "%03d", file_number);
	//file_name = strcat(file_name, extension);
	strcat(&file_name[0], extension);
	
	//Check to see if the file already exists in the root directory.
    while(root_file_exists(file_name))
    {
        file_number++;	//If the file already exists, increment the file number and check again.
        if(file_number == 999)
        {
            return 0;
        }
		//file_name=strcpy(file_name, name);
		strcpy(&file_name[0], name);
		sprintf(file_name+strlen(file_name), "%03d", file_number);
		//file_name = strcat(file_name, extension);
		strcat(&file_name[0], extension);
    }
	
	//Get the file handle of the new file.  We will log the data to this file
	handle = root_open_new(file_name);	
	if(handle == NULL) return 0;
	
	return 1;
}

int cMemory::save(char * data)
{
	int error=0;
	
	if(handle == NULL)return 0;
	if(strlen(data)==0)return 0;
	
	while(error < 10)
	{
		if(fat16_write_file(handle, (const unsigned char *)data, strlen(data))<0)error+=1;
		else break;
		//delay_ms(100);
	}
	if(error == 10)return 0;
	
	error=0;
	while(error < 10)
	{
		if(!sd_raw_sync())error+=1;
		else break;
		//delay_ms(100);
	}
	if(error == 10) return 0;
	return 1;
}

void cMemory::close(void)
{
	fat16_close_file(handle);
}

void cMemory::open(void)
{
	handle = root_open(file_name);
}

char memoryBegin(void)
{
	//Bring up SD and Open the root directory
    if(!sd_raw_init())
    {
        return 0;
    }
    if(openroot())
    {
        return 0;
    }
	return 1;
}
