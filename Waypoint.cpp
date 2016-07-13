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
#include <fstream>
#include <cmath>
#include <string.h>
#include <sstream>
//#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include "Control.h"
#include "Waypoint.h"
#include "MathFunctions.h"

using namespace std;

Waypoint::Waypoint(Odometry *pOdometry, Archer *pSensors, TCPServer *pServer) : Control(pOdometry)
{
	//mpOdometry = pOdometry;
	mpSensors = pSensors;
	mpServer = pServer;
	mpServer->makeServer();
}

void Waypoint::createWaypoints()
{
	char c1[2];
	//string strID = "WP";
	//mpWaypoints[0][0] = 1000.0;
	//mpWaypoints[0][1] = 1000.0;
	//cout << "Making Waypoints: " << str1 << endl;
	//cout << str2 << endl;
	//str1 = mpServer->rcvMessage();
	//size_t found = str2.find(strID);
	//cout << "Found?: " << found << endl;
	string strCall = "WP";
	string str2;
	cout << strCall << endl;
	mpServer->sndMessage(strCall);
	usleep(100000);
	str2 = mpServer->rcvMessage();
	size_t found = str2.find(strCall);
  	if (found != string::npos){
  		stringstream into;
		into << str2;
		into >> c1 >> mWaypointLength;
		cout << "WPLen?: " << mWaypointLength << endl;
		mpWaypoints = new float*[mWaypointLength];
		//mpWaypoints[0] = new float[2];//[Y_AXIS + 1];
		//int i = 0;
		//while (into >> mpWaypoints[i][0] >> mpWaypoints[i][1]){
		//	cout << mpWaypoints[i][0] << " " << mpWaypoints[i][1] << endl;
		//	i++;
		//}

		for (int i = 0; i < mWaypointLength; i++){
			mpWaypoints[i] = new float[2];
			into >> mpWaypoints[i][0] >> mpWaypoints[i][1];//mpWaypoints[i][X_AXIS] >> mpWaypoints[i][Y_AXIS];
			cout << mpWaypoints[i][0] << " " << mpWaypoints[i][1] << endl;
		}

	}
	else{
		mpWaypoints = new float*[1];
		mpWaypoints[0] = new float[2];//[Y_AXIS + 1];
		mpWaypoints[0][0] = 1000.0;
		mpWaypoints[0][1] = 1000.0;
	}
	cout << "WAYPOINTS: " << mpWaypoints[0][0] << " " << mpWaypoints[0][1] << endl;//[X_AXIS] << " " << mpWaypoints[0][Y_AXIS] << endl;
	//mpWaypoints[0][X_AXIS] = 0.0;
	//mpWaypoints[0][Y_AXIS] = 0.0;
	mStatus = STARTING_STS;
}

void Waypoint::getWaypoints()
{
	//string str1;
	string str2 = " ";
	string strCall = "WP";
	mpServer->sndMessage(strCall);
	usleep(100000);
	str2 = mpServer->rcvMessage();
	cout << str2 << endl;
	
}

void Waypoint::sendData()
{
	string strdata;
	stringstream ss;

	ss << "Current: " << mpSensors->mCurrent << " X: " << mpOdometry->mX << " Y: " << mpOdometry->mY << " Heading: " << mpOdometry->mHeading << endl;
	strdata = ss.str();
	cout << strdata << endl;
	mpServer->sndMessage(strdata);
}

/*
void Waypoint::getTargetSpeedRate(float &rSpeed, float &rRate)
{
	cout << "CONTROL " << mStatus << endl;
	switch(mStatus)
	{
		case STANBY_STS:
			//Do not change speed or rate while in this status
			return;
		case STARTING_STS:
			createWaypoints();
			mStatus = TURNING_STS;
			break;
		case TURNING_STS:
			if(faceTarget()) 
				mStatus = MOVING_STS;
			break;
		case MOVING_STS:
			if(COMPLETED_WAYPOINT == freeHeading())
				mStatus = FACING_ZERO_STS;
			break;
		case FACING_ZERO_STS:
			if(faceTarget(0.0))
			{
				cout << "Finished commands! ...\n";
				mStatus = STANBY_STS;
			}
			break;
	}
	rSpeed = mSpeed;
	rRate = mRate; 
} */