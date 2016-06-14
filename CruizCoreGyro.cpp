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
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include "CruizCoreGyro.h"
#include "MathFunctions.h"

using namespace std;

CruizCoreGyro::CruizCoreGyro(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[]) : Archer(period, track, encoderScaleFactor, COUNTS_REVOLUTION_in)
{
	//initialize read_in variables
	PACKET_SIZE = 8;
	SAMPLES = 1000;

	if(-1 == (file_descriptor = open(GYRO_PORT,O_RDONLY)))
	{
		cout << "Error opening port \n";
		cout << "Set port parameters using the following Linux command:\n stty -F /dev/ttyUSB0 115200 raw\n";
		cout << "You may need to have ROOT access";
	}
	cout << "CruizCoreR1050 communication port is ready\n";

	//change output rate of gyro to 10Hz
	if(Write("$MIA,,,,R,10,,,*EA")) {
		cout << "could not write to CruizCore" << endl;
		cout << "output rate incorrect" << endl;
		exit(1);
	}

	strcpy(mName,"CruizCore");
	cout << "CruizCore Gyro Robot ready!\n";
	cout << "GYRO_PORT: " << GYRO_PORT << endl;

	if (tcflush(file_descriptor, TCIOFLUSH) == 0)
       printf("The input and output queues have been flushed.\n");
    else
       perror("tcflush error\n");


}

CruizCoreGyro::~CruizCoreGyro()
{
	close(file_descriptor);
	cout << "Closing CruizCoreGyro port";
}

int CruizCoreGyro::readSensors()
{
	// if (tcflush(file_descriptor, TCIOFLUSH) == 0)
 //           printf("The input and output queues have been flushed.\n");
 //        else
 //           perror("tcflush error\n");


	//may need to double packet size to deal with extra info in buffer
	//may need to do this because


	//Get Encoder information
	Archer::readSensors();

	//read from file
	//8 bites

	//Get angle from CruizCore gyro

	short header;
	short rate_int;
	short angle_int;
	float rate_float;
	float angle_float;
	short check_sum;
	unsigned char data_packet[PACKET_SIZE];


	//read(file_descriptor,data_packet,PACKET_SIZE*100);

	//if you flush right here then there is not enough time for the buffer to completely refill
	//you only get a few bytes when readin, not a full 8
	

	cout << "1" << endl;

	int hold;
	cout << "file_descriptor: " << file_descriptor << endl;
	cout << "data_packet: " << data_packet << endl;
	cout << "PACKET_SIZE: " << PACKET_SIZE << endl;
	hold = read(file_descriptor,data_packet,PACKET_SIZE);
	cout << "1.5" << endl;
	if(PACKET_SIZE != hold) {
		cout << "ERROR: # of bytes actually read(8): " << hold << endl;
		return 0;
	}

	cout << "2" << endl;

	// Verify data packet header 
	memcpy(&header,data_packet,sizeof(short));
	if(header != (short)0xFFFF)
	{
		cout << "ERROR: HEADER(" << (short)0xFFFF << "): " << header << endl;
		return 0;
	}

	cout << "3" << endl;

	// Copy values from data string 
	memcpy(&rate_int,data_packet+2,sizeof(short));
	memcpy(&angle_int,data_packet+4,sizeof(short));
	memcpy(&check_sum,data_packet+6,sizeof(short));

	cout << "4" << endl;

	// Verify checksum
	if(check_sum != (short)(0xFFFF + rate_int + angle_int))
	{
		cout<< "ERROR: checksum\n";
		return 0;
	}

	cout << "5" << endl;

	// Apply scale factors
	rate_float = rate_int/100.0;
 	angle_float = angle_int/100.0;
	
	cout << "rate_float:" << rate_float << " [deg/sec]" << endl << "angle_float: " << angle_float << " [deg]\n";

	mRotation = angle_float/100.0; //CruizCore angle must be inverted
	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);

 	cout << "Gyro: " << math_functions::rad2deg(mRotation) << endl;


/*

	static float s_last_angle = 0;
	short new_angle = pIic->Raw[mGyroPort][pIic->Actual[mGyroPort]][0]*256 + pIic->Raw[mGyroPort][pIic->Actual[mGyroPort]][1];
	mRotation = - (new_angle - s_last_angle)/100.0; //XGL angle must be inverted
	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);
	s_last_angle = new_angle;
	
	cout << "XG1300L: " << math_functions::rad2deg(mRotation) << endl;
*/
	return DATA_READY;
}

bool CruizCoreGyro::Write(string str)
{

	//cout<<"Writing: "<<ReplaceString(str, "\r", "\r\n");
	int countSent = write(file_descriptor, str.c_str(), str.length());

	//Verify weather the Transmitting Data on UART was Successful or Not
	if(countSent < 0) {
		return false;
	}

	return true;
}

