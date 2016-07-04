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

#include <iostream>
#include <cmath>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Apriltags.h"
#include "MathFunctions.h"

using namespace std;

Apriltags::Apriltags(Robot *pSensors, TCPServer *pServer) : Odometry(pSensors)
{
	//TCP Server Setup
	mpServer = pServer;
	mpServer->makeServer();
	
	
	//size_t n1 = message.find('ID:');
	//size_t n2 = message.find('x=');
	//size_t n3 = message.find('y=');
	//size_t n4 = message.find('roll=');
	//int ID_tag = atoi(message[n1+3])
	//float ID_tag = atof(message[n1+3])

	//mpSensors = pSensors;
	//mPeriod = mpSensors->getPeriod();
	//reset();
	//cout << "Odometry is ready! T=" << mPeriod << " [s]" << endl;
}

void Apriltags::updatePosition()
{
	Odometry::updatePosition();
	
	//TCP server
	mpServer->rcvMessage();

	
	if (mpServer->message) {
		char * pID;
		pID = strstr(mpServer->message,"ID:");
		aID = atoi(pID + 3);

		char * px;
		px = strstr(mpServer->message,"x=");
		ax = atof(px + 2);

		char * py;
		py = strstr(mpServer->message,"y=");
		ay = atof(py + 2);

		char * proll;
		proll = strstr(mpServer->message,"roll=");
		aroll = atof(proll + 5);

		switch(aID)
		{
			case 0:
				mHeading = 3*PI/2 - aroll;
				mX = ax*cos(mHeading) - ay*sin(mHeading) + 5;
				mY = ax*sin(mHeading) - ay*cos(mHeading);
				break;
			case 1:
				mHeading = 0 - aroll;
				mX = ax*cos(mHeading) - ay*sin(mHeading) + 10;
				mY = ax*sin(mHeading) - ay*cos(mHeading) + 5;
				break;
			case 2:
				mHeading = PI/2 - aroll;
				mX = ax*cos(mHeading) - ay*sin(mHeading) + 5;
				mY = ax*sin(mHeading) - ay*cos(mHeading) + 10;
				break;
			case 3:
				mHeading = PI - aroll;
				mX = ax*cos(mHeading) - ay*sin(mHeading);
				mY = ax*sin(mHeading) - ay*cos(mHeading) + 5;
				break;
		}
		mHeading = math_functions::unwrap(mHeading);
	}
	cout << "POSITION " << mpSensors->getName() << ": X: " <<mX << " Y: " << mY << " heading: " << math_functions::rad2deg(mHeading) << " speed: " << mSpeed << " rate: " << math_functions::rad2deg(mRate) << endl;
}
