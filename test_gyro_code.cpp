// Microinfinity Cia. Ltd.
// This program demonstrates how to communicate with the CruizCore R1050 gyroscope
// This program assumes that the port 'COMM_PORT' has been properly initialized. 
// In linux this can be done is Linux using the following command line:
// 			stty -F /dev/ttyUSB0 115200 raw
// This program can be compiled using:
// 			g++ -o ccr1050 ccr1050.cpp

#include <iostream>
#include <unistd.h>
#include <algorithm>    // std::copy
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

unsigned char data_packet[24]; //3*DATA_PACKET = 24
unsigned char *packet_read_in; //pointer to beginning of data_packet
unsigned char packet_use[8];

//Define global variables
int file_descriptor;


// Open serial port
bool ccr1050_init()
{
	int i = system("./init_gyro_port.sh");
 	if(i != 0) {
  		cout << "failure to initialize gyro port" << endl;
  		exit(1);
 	}
 	else if(i == 0){
 		cout << "BAUD RATE SET" << endl;
 	}


	if(-1 == (file_descriptor = open(GYRO_PORT,O_RDWR)))
	{
		cout << "Error opening port \n";
		cout << "Set port parameters using the following Linux command:\n stty -F /dev/ttyUSB0 115200 raw\n";
		cout << "You may need to have ROOT access";
	}
	cout << "CruizCoreR1050 communication port is ready\n";

	//software reset
	string str1 = "$MIB,RESET*87";
	int countSent1 = write(file_descriptor, str1.c_str(), str1.length());
	//Verify weather the Transmitting Data on UART was Successful or Not
	if(countSent1 < 0) {
		cout << "could not write to CruizCore" << endl;
		cout << "could not reset software" << endl;
		exit(1);
	}
	else {
		cout << "CruizCore reset" << endl;
	}

	usleep(1500);

	if (tcflush(file_descriptor, TCIOFLUSH) == 0)
       printf("The input and output queues have been flushed.\n");
    else {
       perror("tcflush error\n");
       exit(1);
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
	


	int actual_packet_size;
	actual_packet_size = read(file_descriptor,packet_read_in,PACKET_SIZE * 10);

	if(actual_packet_size != PACKET_SIZE) {
		int difference = packet_read_in - data_packet;
		int current_size = difference + actual_packet_size;	//current size of packet

		if(current_size < PACKET_SIZE) {	//cant form full packet
			packet_read_in += actual_packet_size;
			cout << "ERROR: packet size too small -- not enough bytes in buffer" << endl;
			return 0;
		}
		else if(current_size % PACKET_SIZE == 0) {	//if packet size is a multiple of 8
			//copy last packet into packet_use
			copy(data_packet + current_size - PACKET_SIZE, data_packet + current_size + PACKET_SIZE, packet_use);
			//reset packet_read_in to beg of data_packet
			packet_read_in = data_packet;
		}
		else if(current_size % PACKET_SIZE != 0) {	//this implies left over bytes were read
			switch(current_size / PACKET_SIZE) {
				case 1: {	//there is at most 1 packet
					copy(data_packet, data_packet + PACKET_SIZE, packet_use);				//copy first 8 bytes to packet_use
					copy(packet_read_in, packet_read_in + actual_packet_size, data_packet);	//copy unused bytes from end of data_packet to beginning
					packet_read_in = data_packet + actual_packet_size;						//set packet_read_in ptr to end of valid data
				}
				break;
				case 2: {	//there is at most 2 packets
					copy(data_packet + PACKET_SIZE, data_packet + (2 * PACKET_SIZE), packet_use); 
					copy(packet_read_in, packet_read_in + actual_packet_size, data_packet);
					packet_read_in = data_packet + actual_packet_size;
				}
				break;
				case 3: {	//there is at most 3 packets
					copy(data_packet + (2 * PACKET_SIZE), data_packet + (3 * PACKET_SIZE), packet_use);
					copy(packet_read_in, packet_read_in + actual_packet_size, data_packet);
					packet_read_in = data_packet + actual_packet_size;
				}
				break;
			}
		}
	}

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
