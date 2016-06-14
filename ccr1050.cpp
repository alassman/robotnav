// Microinfinity Cia. Ltd.
// This program demonstrates how to communicate with the CruizCore R1050 gyroscope
// This program assumes that the port 'COMM_PORT' has been properly initialized. 
// In linux this can be done is Linux using the following command line:
// 			stty -F /dev/ttyUSB0 115200 raw
// This program can be compiled using:
// 			g++ -o ccr1050 ccr1050.cpp

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <sstream>

using namespace std;

//Define constants
const char COMM_PORT[] = "/dev/ttyUSB0";
const int PACKET_SIZE = 8;
const int SAMPLES = 1000;

//Define global variables
int file_descriptor;


// Open serial port
bool ccr1050_init()
{
	if(-1 == (file_descriptor = open(COMM_PORT,O_RDONLY)))
	{
		cout << "Error opening port \n";
		cout << "Set port parameters using the following Linux command:\n stty -F /dev/ttyUSB0 115200 raw\n";
		cout << "You may need to have ROOT access";
		return false;
	}
	cout << "CruizCoreR1050 communication port is ready\n";
	return true;
}

// Close serial port
void ccr1050_close()
{
	close(file_descriptor);
	cout << "Closing communication port";
}

// Get a data packet and parse it
bool ccr1050_getvalue()
{
	short header;
	short rate_int;
	short angle_int;
	float rate_float;
	float angle_float;
	short check_sum;
	unsigned char data_packet[PACKET_SIZE];
	
	if(PACKET_SIZE != read(file_descriptor,data_packet,PACKET_SIZE))
		return false;

	// Verify data packet header 
	memcpy(&header,data_packet,sizeof(short));
	if(header != (short)0xFFFF)
	{
		cout << "Header error !!!\n";
		return false;
	}

	// Copy values from data string 
	memcpy(&rate_int,data_packet+2,sizeof(short));
	memcpy(&angle_int,data_packet+4,sizeof(short));
	memcpy(&check_sum,data_packet+6,sizeof(short));

	// Verify checksum
	if(check_sum != (short)(0xFFFF + rate_int + angle_int))
	{
		cout<< "Checksum error!!\n";
		return false;
	}

	// Apply scale factors
	rate_float = rate_int/100.0;
 	angle_float = angle_int/100.0;
	
	cout << "rate_float:" << rate_float << " [deg/sec]\t angle_float:" << angle_float << " [deg]\n";
	return true;
}

int main()
{
	// Open communication channel
	if(!ccr1050_init())
		return 0;
	
	// Get, parse and display data
	for(int i=1; i<SAMPLES; i++) {
		ccr1050_getvalue();
	}
	
	// Close communication
	ccr1050_close();
	return 1;
}
