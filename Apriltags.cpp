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
#include <sstream>
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
	
	//mpSensors = pSensors;
	//mPeriod = mpSensors->getPeriod();
	//reset();
	//cout << "Odometry is ready! T=" << mPeriod << " [s]" << endl;
}

void Apriltags::updatePosition()
{
	Odometry::updatePosition();
	int yOff = 3340;
	char c1[2];
	char c2;
	char c3;
	char c4[4];
	string str1;
	string strID = "ID";
	//TCP server
	str1 = mpServer->rcvMessage();
	size_t found = str1.find(strID);
  	if ((found != string::npos)){//&& (mRate == 0) && (mSpeed == 0)){
  		stringstream into;
		into << str1;
		into >> c1 >> aID >> c2 >> ax >> c3 >> ay >> c4 >> aroll;
		cout << "ATAGS: " << c1 << aID << c2 << ax << c3 << ay << c4 << aroll << endl;
  		ax = 1000*ax-150;
    	ay = 1000*ay;
    	aroll = -1*aroll;

	
		switch(aID)
		{
			case 0:
				//mHeading = -1*(3*PI/2 - aroll);
				mX = 1620 - (ax*cos(-mHeading) - ay*sin(-mHeading)); //May have to flip mX like mY
				mY = -1*(0 - (ax*sin(-mHeading) + ay*cos(-mHeading))) + yOff;
				break;
			case 1:
				//mHeading = -1*(0 - aroll);
				mX = 3360 - (ax*cos(-mHeading) - ay*sin(-mHeading));
				mY = -1*(1580 - (ax*sin(-mHeading) + ay*cos(-mHeading))) + yOff;
				break;
			case 2:
				//mHeading = -1*(PI/2 - aroll);
				mX = 1750 - (ax*cos(-mHeading) - ay*sin(-mHeading));
				mY = -1*(3340 - (ax*sin(-mHeading) + ay*cos(-mHeading))) + yOff;
				break;
			case 3:
				//mHeading = -1*(PI - aroll);
				mX = 0 - (ax*cos(-mHeading) - ay*sin(-mHeading));
				mY = -1*(1740 - (ax*sin(-mHeading) + ay*cos(-mHeading))) + yOff;
				break;
		}
		//mHeading = math_functions::unwrap(mHeading);
	//	cout << "Apriltag ID:" << pID << " X: " << ax << " Y: " << ay << " Roll: " << aroll << endl;
	
	cout << "POSITION " << "Aptag Update " << " X: " <<mX << " Y: " << mY << " heading: " << math_functions::rad2deg(mHeading) << " speed: " << mSpeed << " rate: " << math_functions::rad2deg(mRate) << endl;
	}
}
