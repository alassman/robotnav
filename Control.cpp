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
#include "Control.h"
//#include "Waypoint.h" //Added by MQ
#include "MathFunctions.h"
using namespace std;

//Free heading control constants
const float DIST_ANGLE_SPEED_GAIN = 0.5;//0.3;//0.5; //Distance to target control
const float CURRENT_SPEED_GAIN = 0.3;//0.2;//0.3; //Speed control
const float DIST_ANGLE_ANGLE_RATE_GAIN = 0.3;//0.6;//0.5; //Rate control
const float ANGLE_RATE_GAIN = 1.6;//1.7;//1.5; 
const float K_D = 10;
const float K_D2 = 0;
const float K_I = 0.1;
const float K_I2 = 0.1;

//Dynamic constants
const float	MAX_RATE = math_functions::deg2rad(90.0);//90.0); //[deg/sec]
const float	MAX_SPEED = 170.0;//150.0;//200.0; //[mm/sec]
const float	MIN_SPEED = 0;//149.0;//100.0; //[mm/sec]
const float	TARGET_DIST = 250.0;//200.0; //[mm] //100.0
const float	MIN_TARGET_DIST = 250.0;//100.0;  //[mm] //50.0
const float	TARGET_ANGLE = math_functions::deg2rad(7.0); //[deg]
const float	MIN_TARGET_ANGLE = TARGET_ANGLE / 2.0; //[deg]

Control::Control(Odometry *pOdometry)
{
	mpOdometry = pOdometry;
	mPeriod = mpOdometry->getPeriod();

	mWaypointLength = 0;
	reset();
	
	cout << "Control ready\n";
}

Control::~Control()
{
	reset();
	cout << "Control closed\n";
}

void Control::getTargetSpeedRate(float &rSpeed, float &rRate)
{
	cout << "CONTROL " << mStatus << endl;
	switch(mStatus)
	{
		case STANBY_STS:
			//Do not change speed or rate while in this status
			return;
		case STARTING_STS:
			cout << "Creating Waypoints" << endl;
			createWaypoints();
			mStatus = TURNING_STS;
			break;
		case TURNING_STS:
			if(faceTarget()) 
				mStatus = MOVING_STS;
			break;
		case MOVING_STS:
			int C_STATUS = freeHeading();
			if(COMPLETED_WAYPOINT == C_STATUS)
				mStatus = STANBY_STS;
			else if(FOUND_NEW_WAYPOINT == C_STATUS)
				mStatus = TURNING_STS;
			break;
/*		case FACING_ZERO_STS:
			if(faceTarget(0.0))
			{
				cout << "Finished commands! ...\n";
				mStatus = STANBY_STS;
			}
			break; */
	}
	rSpeed = mSpeed;
	rRate = mRate;
}

//Free heading control
int Control::freeHeading()
{
	static float s_last_dist = TARGET_ANGLE;
	static float s_last_target_ang_err2 = 0.0;
	static float target_ang_err_int2 = 0;
	if(mpWaypointIndex >= mWaypointLength)
		return COMPLETED_WAYPOINT;

	float target_dist;
	float target_ang;
	cmpTargetDirDist(target_dist, target_ang);

	//Compute control errors or difference to reach correct target angle at current point
	float target_ang_err = - math_functions::unwrap(target_ang - mpOdometry->mHeading);
	float speed = mpOdometry->mSpeed;
	float accel = DIST_ANGLE_SPEED_GAIN * target_dist * cos(target_ang_err) - CURRENT_SPEED_GAIN * speed;
	float d_target_ang_err  = target_ang_err - s_last_target_ang_err2;
	target_ang_err_int2 += target_ang_err;
	
	cout << "WPT: " << mpWaypoints[mpWaypointIndex][X_AXIS] << " " << mpWaypoints[mpWaypointIndex][Y_AXIS] << " " << math_functions::rad2deg(target_ang) << " " << math_functions::rad2deg(target_ang_err) << " " << mpWaypointIndex << " " << mWaypointLength << endl;
	
	mSpeed = ((speed < MIN_SPEED) ? MIN_SPEED : speed) + accel * mPeriod;
	if(mSpeed < MIN_SPEED) mSpeed = MIN_SPEED;
	if(mSpeed > MAX_SPEED) mSpeed = MAX_SPEED;
	
	//mRate = DIST_ANGLE_ANGLE_RATE_GAIN * target_dist / speed * sin(target_ang_err) + d_target_ang_err * K_D2; //changed to - by MQ
	mRate = 0*( target_ang_err * ANGLE_RATE_GAIN + d_target_ang_err * K_D2 + target_ang_err_int2 * K_I2); //changed to - by MQ
	if(fabsf(mRate) > MAX_RATE)
		mRate = (mRate > 0.0) ? MAX_RATE : -MAX_RATE;

	if(target_dist < TARGET_DIST)
	{
		mSpeed = MIN_SPEED;
		if(s_last_dist < target_dist || target_dist < MIN_TARGET_DIST)
		{
			mpWaypointIndex++;
			s_last_dist = TARGET_ANGLE;
			return ((mpWaypointIndex >= mWaypointLength) ? COMPLETED_WAYPOINT : FOUND_NEW_WAYPOINT);
		}
 		s_last_dist = target_dist;
	}
	return MOVING_TO_WAYPOINT;
}

void Control::cmpTargetDirDist(float &rRelativeDistance, float &rRelativeAngle)
{
	rRelativeDistance = sqrt((mpWaypoints[mpWaypointIndex][X_AXIS] - mpOdometry->mX) * (mpWaypoints[mpWaypointIndex][X_AXIS] - mpOdometry->mX) + (mpWaypoints[mpWaypointIndex][Y_AXIS] - mpOdometry->mY) * (mpWaypoints[mpWaypointIndex][Y_AXIS] - mpOdometry->mY));
	rRelativeAngle = atan2(mpWaypoints[mpWaypointIndex][Y_AXIS] - mpOdometry->mY, mpWaypoints[mpWaypointIndex][X_AXIS] - mpOdometry->mX); // -1* by MQ
}

bool Control::faceTarget(float targetAngle)
{
	static float s_last_target_ang_err = 0.0;
	static float target_ang_err_int = 0;
	float target_dist;
	//Impossible condition, is true when no argument is passed, in that case make the robot face the next WP
	if(targetAngle == FACE_NEXT_WAYPOINT)
	{
		cout << "WPT: " << mpWaypoints[mpWaypointIndex][X_AXIS] << " " << mpWaypoints[mpWaypointIndex][Y_AXIS] << " " << mpWaypointIndex << " " << mWaypointLength << endl;
		cmpTargetDirDist(target_dist, targetAngle);
	}
	float target_ang_err = - math_functions::unwrap(targetAngle - mpOdometry->mHeading);
	float d_target_ang_err  = target_ang_err - s_last_target_ang_err;
	target_ang_err_int += target_ang_err;
	cout << "FACING Target: " << math_functions::rad2deg(targetAngle) << " Current: " << math_functions::rad2deg(mpOdometry->mHeading) << " Diff: " << math_functions::rad2deg(target_ang_err) << endl;
	mRate = target_ang_err * ANGLE_RATE_GAIN + d_target_ang_err * K_D + target_ang_err_int * K_I; //changed to - by MQ
	
	if(fabsf(mRate) > MAX_RATE) mRate = (mRate > 0.0) ? MAX_RATE : -MAX_RATE;

	//Make sure that the robot speed is zero
	mSpeed = 0.0;
	
	if(fabsf(target_ang_err) < TARGET_ANGLE &&
		(s_last_target_ang_err * target_ang_err < 0.0 || fabsf(target_ang_err) < MIN_TARGET_ANGLE)) 
	{
		mRate = 0.0;
		return true;
	}
	s_last_target_ang_err = target_ang_err;
	return false;
}

void Control::enable()
{
	mStatus = STARTING_STS;
}

void Control::disable()
{
	mSpeed = 0.0;
	mRate = 0.0;
	mStatus = STANBY_STS;
}

void Control::createWaypoints()
{
	reset();
	mWaypointLength = 1;
	mpWaypoints = new float*[mWaypointLength];
	mpWaypoints[0] = new float[Y_AXIS + 1];
	mpWaypoints[0][X_AXIS] = 0.0;
	mpWaypoints[0][Y_AXIS] = 0.0;
	mStatus = STARTING_STS;
}

void Control::reset()
{
	if(mWaypointLength)
	{
		for(int i = 0; i< mWaypointLength; i++)
			delete [] mpWaypoints[i];
		delete [] mpWaypoints;
		mWaypointLength = 0;
	}
	mpWaypointIndex = 0;
	mSpeed = 0.0;
	mRate = 0.0;
	mStatus = STANBY_STS;
}


