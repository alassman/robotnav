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

#ifndef CruizCore_GYRO_H
#define CruizCore_GYRO_H

#include <string>
#include <stdlib.h> 
#include "Archer.h"

class CruizCoreGyro : public Archer
{
private:
	int PACKET_SIZE;
	int SAMPLES;
	int file_descriptor;
	unsigned char data_packet[24]; //3*DATA_PACKET = 24
	unsigned char *packet_read_in; //pointer to beginning of data_packet
	unsigned char packet_use[8];

public:
		CruizCoreGyro(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[]);
		virtual ~CruizCoreGyro();
		virtual int readSensors();
};

#endif
