/*
 * Robot Navigation Program
 * www.robotnav.com
 *
 * (C) Copyright 2010 - 2014 Lauro Ojeda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>
#include <termios.h>
#include <algorithm>    // std::copy
#include <time.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include "CruizCore_test.h"
#include "MathFunctions.h"

using namespace std;

CruizCoreGyro::CruizCoreGyro(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[]) 
: Archer(period, track, encoderScaleFactor, COUNTS_REVOLUTION_in)
{
	//initialize read_in variables
	PACKET_SIZE = 8;
	SAMPLES = 1000;
	packet_read_in = data_packet;

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

	//cout<<"Writing: "<<ReplaceString(str, "\r", "\r\n");
	string str = "$MIA,,,,R,10,,,*EA";
	int countSent = write(file_descriptor, str.c_str(), str.length());
	//Verify weather the Transmitting Data on UART was Successful or Not
	if(countSent < 0) {
		cout << "could not write to CruizCore" << endl;
		cout << "output rate incorrect" << endl;
		exit(1);
	}
	else {
		cout << "CruizCore outout set to 10Hz" << endl;
	}

	strcpy(mName,"CruizCore");
	cout << "CruizCore Gyro Robot ready!\n";
	cout << "GYRO_PORT: " << GYRO_PORT << endl;

	if (tcflush(file_descriptor, TCIOFLUSH) == 0)
       printf("The input and output queues have been flushed.\n");
    else {
       perror("tcflush error\n");
       exit(1);
   	}

   	readSensors();

   	cout << "CruizCore Robot ready!" << endl;


}

CruizCoreGyro::~CruizCoreGyro()
{
	close(file_descriptor);
	cout << "Closing CruizCoreGyro port";
}

int CruizCoreGyro::readSensors()
{
	//Get Encoder information
	Archer::readSensors();

	//read from file
	//8 bites

	short header;
	short rate_int;
	short angle_int;
	//float rate_float;
	//float angle_float;
	short check_sum;

	int actual_packet_size;
	actual_packet_size = read(file_descriptor,packet_read_in,PACKET_SIZE * 10);
	//deal with packet role over
	if(actual_packet_size != PACKET_SIZE) {
		int difference = packet_read_in - data_packet;
		cout << "difference between old and new packet: " << difference << endl;
		int current_size = difference + actual_packet_size;	//current size of packet

		cout << "read packet size not equal to 8: " << actual_packet_size << endl;
		cout << "current packet size: " << current_size << endl;

		if(current_size < PACKET_SIZE) {	//cant form full packet
			packet_read_in += actual_packet_size;
			cout << "ERROR: packet size too small -- not enough bytes in buffer" << endl;
			cout << "-------------------------------------------" << endl;
			cout << "packet_read_in set to: " << packet_read_in << endl;
			cout << "data_packet set to: " << data_packet << endl;
			cout << "their difference: " << packet_read_in - data_packet << endl;
			cout << "-------------------------------------------" << endl;
			return 0;
		}
		else if(current_size % PACKET_SIZE == 0) {	//if packet size is a multiple of 8
			cout << "packet size is multiple of 8" << endl;
			//copy last packet into packet_use
			copy(data_packet + current_size - PACKET_SIZE, data_packet + current_size + PACKET_SIZE, packet_use);
			//reset packet_read_in to beg of data_packet
			packet_read_in = data_packet;
		}
		else if(current_size % PACKET_SIZE != 0) {	//this implies left over bytes were read
			switch(current_size / PACKET_SIZE) {
				case 1: {	//there is at most 1 packet
					cout << "at most 1 packet in data_packet" << endl;
					copy(data_packet, data_packet + PACKET_SIZE, packet_use);				//copy first 8 bytes to packet_use
					copy(packet_read_in, packet_read_in + actual_packet_size, data_packet);	//copy unused bytes from end of data_packet to beginning
					packet_read_in = data_packet + actual_packet_size;						//set packet_read_in ptr to end of valid data
				}
				break;
				case 2: {	//there is at most 2 packets
					cout << "at most 2 packets in data_packet" << endl;
					copy(data_packet + PACKET_SIZE, data_packet + (2 * PACKET_SIZE), packet_use); 
					copy(packet_read_in, packet_read_in + actual_packet_size, data_packet);
					packet_read_in = data_packet + actual_packet_size;
				}
				break;
				case 3: {	//there is at most 3 packets
					cout << "at most 3 packets in data_packet" << endl;
					copy(data_packet + (2 * PACKET_SIZE), data_packet + (3 * PACKET_SIZE), packet_use);
					copy(packet_read_in, packet_read_in + actual_packet_size, data_packet);
					packet_read_in = data_packet + actual_packet_size;
				}
				break;
			}
		}
	}


	// Verify data packet header 
	memcpy(&header,packet_use,sizeof(short));
	if(header != (short)0xFFFF)
	{
		cout << "ERROR: HEADER(" << (short)0xFFFF << "): " << header << endl;
		return 0;
	}

	// Copy values from data string 
	memcpy(&rate_int,packet_use+2,sizeof(short));
	memcpy(&angle_int,packet_use+4,sizeof(short));
	memcpy(&check_sum,packet_use+6,sizeof(short));

	// Verify checksum
	if(check_sum != (short)(0xFFFF + rate_int + angle_int))
	{
		cout<< "ERROR: checksum\n";
		return 0;
	}

	static float s_last_angle = 0;

	//cout << "rate_float:" << rate_float << " [deg/sec]" << endl << "angle_float: " << angle_float << " [deg]\n";

	mRotation = (angle_int - s_last_angle)/100.0;

	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);

	s_last_angle = angle_int;

 	cout << "Gyro: " << math_functions::rad2deg(mRotation) << endl;

	return DATA_READY;
}

