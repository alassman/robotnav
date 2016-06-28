/*
 * Robot Navigation
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
 *
 */

#include "Archer.h"
#include <iostream>
#include "CruizCoreGyro.h"
//#include "LegoGyro.h"
#include "Odometry.h"
#include "Control.h"
#include "Keyboard.h"
#include "IrRemote.h"
//#include "Buttons.h"
#include "MathFunctions.h"

//TCP
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tcpconnector.h"

using namespace std;

//Left and right motor ports, as shown in EV3 brick labels
//const char LEFT_MOTOR_PORT = 'D';
//const char RIGHT_MOTOR_PORT = 'A';
//
//char MOTOR_PORTS[] = {LEFT_MOTOR_PORT, RIGHT_MOTOR_PORT};

//Sensor ports, as shown in EV3 brick labels
const char GYRO_PORT[] = "/dev/ttyUSB0";

//Platform measurements
const float TRACK = 347.0; // [mm]
const int COUNTS_REVOLUTION = 1920; // [count/rev] was 568
const float WHEEL_DIAMETER = 155.0; // [mm]
const float ENCODER_SCALE_FACTOR = PI * WHEEL_DIAMETER / (COUNTS_REVOLUTION); // [mm/count]

// Runtime constant
const float INC_SPEED_MM_SECOND = 100.0; //[mm/sec]
const float INC_RATE_RAD_SECOND = math_functions::deg2rad(10.0); //[rad/sec]
const float PERIOD = 0.1; //[sec]


int main()
{
    //Only one robot can be created at the time
    //Archer robot(PERIOD, TRACK, ENCODER_SCALE_FACTOR, COUNTS_REVOLUTION); //Odometry only
    
	//Only one robot can be created at the time
	CruizCoreGyro robot(PERIOD, TRACK, ENCODER_SCALE_FACTOR, COUNTS_REVOLUTION, (char *)GYRO_PORT); //Gyro Enhanced

	Odometry odometry(&robot); 

	Keyboard user_input;
	Control control(&odometry);
	
	// TCP Setup
	const char* server = "35.2.51.190";
	int port = 9999;
    // TCP Setup
    //int len;
    string message;
    //char line[256];
    //TCPConnector* connector = new TCPConnector();
    //TCPStream* stream = connector->connect(server, port);

	//Create and initialize speed variables
	float speed = 0;
	float rate = 0;
	bool quit_program = false;

	//Enter main loop
	while(!quit_program)
	{
		//Read sensors
		robot.readSensors();

		//Compute position
		odometry.updatePosition();

		//Define control instructions
		//User interaction 
		switch(user_input.getKey())
		{
		case MOVE_FORWARD:
			speed += INC_SPEED_MM_SECOND;
			break;
		case MOVE_BACKWARDS:
			speed -= INC_SPEED_MM_SECOND;
			break;
		case TURN_LEFT:
			rate += INC_RATE_RAD_SECOND;
			break;
		case TURN_RIGHT:
			rate -= INC_RATE_RAD_SECOND;
			break;
		case ENABLE_CONTROL:
			control.enable();
			break;
		case EXIT:
			quit_program = true;
		case RESET:
			odometry.reset();
		case STOP_ROBOT: //space bar
			speed = 0;
			rate = 0;
			control.disable();
			robot.setActuators(speed, rate);
			break;
		}
		//High level control
		control.getTargetSpeedRate(speed, rate);
		
		//Execute the instructions
		robot.setActuators(speed, rate);


		// TCP Instructions
		TCPConnector* connector = new TCPConnector();
		TCPStream* stream = connector->connect(server, port, 1);
    	if (stream) {
      		message = "%i\n",mCurrent;
      		stream->send(message.c_str(), message.size());
      		printf("sent - %s\n", message.c_str());
      		//len = stream->receive(line, sizeof(line));
      		//line[len] = 0;
      		//printf("received - %s\n", line);
      		delete stream;
    	}
	}

	return 1;
}
