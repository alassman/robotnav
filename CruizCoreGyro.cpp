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

const int PACKET_SIZE = 8;
const int SAMPLES = 1000;

//Define global variables
int file_descriptor;

CruizCoreGyro::CruizCoreGyro(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[]) : Archer(period, track, encoderScaleFactor, COUNTS_REVOLUTION_in)
{
	if(-1 == (file_descriptor = open(GYRO_PORT,O_RDONLY)))
	{
		cout << "Error opening port \n";
		cout << "Set port parameters using the following Linux command:\n stty -F /dev/ttyUSB0 115200 raw\n";
		cout << "You may need to have ROOT access";
	}
	cout << "CruizCoreR1050 communication port is ready\n";


	//Read sensors a first time in order to initialize some of the states
	readSensors();
	strcpy(mName,"CruizCore");
	cout << "CruizCore Gyro Robot ready!\n";
	cout << "GYRO_PORT: " << GYRO_PORT << endl;
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

	//Get angle from CruizCore gyro

	short header;
	short rate_int;
	short angle_int;
	float rate_float;
	float angle_float;
	short check_sum;
	unsigned char data_packet[PACKET_SIZE*100];


	read(file_descriptor,data_packet,PACKET_SIZE*100);
	
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

	//mRotation = new_angle/100.0; //CruizCore angle must be inverted
	// mRotation = math_functions::deg2rad(mRotation);
	// mRotation = math_functions::unwrap(mRotation);

 // 	cout << "Gyro: " << math_functions::rad2deg(mRotation) << endl;


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

