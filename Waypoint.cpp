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
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include "Control.h"
#include "MathFunctions.h"

using namespace std;

Waypoint::Waypoint(Odometry *pOdometry, TCPServer *pServer) : Control(pOdometry)
{
	mpServer = pServer;
	mpServer->makeClient();
}

void Control::createWaypoints()
{

	//TCP server
	mpServer->rcvMessage();

	
	if (mpServer->message) {
		char * pWP;
		char * pXY;
		pWP = strstr(mpServer->message,"Nwp:");
		mWaypointLength = atoi(pWP + 4);
		mpWaypoints = new float*[mWaypointLength];
		mpWaypoints[0] = new float[Y_AXIS + 1];
		for (int i = 0, i < mWaypointLength, i = i + 1){
			mpWaypoints[i][0] = ;
		}

	}

	//mpWaypoints[0][X_AXIS] = 0.0;
	//mpWaypoints[0][Y_AXIS] = 0.0;
	mStatus = STARTING_STS;

}



