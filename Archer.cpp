#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include "Archer.h"
#include "MathFunctions.h"

using namespace std;

//period is how often to read sensors
//distance/tick
//multiply 2 or 4 by gear ratio for encoder scale factor
//track=distance between two wheels

Archer::Archer(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in) 
	:Robot(period, track, encoderScaleFactor), COUNTS_REVOLUTION(COUNTS_REVOLUTION_in), 
		MAX_COMMAND_SPEED(75), left_motor(1), right_motor(2), LEFT(0), RIGHT(1)
{	
	//establish connection with SDC21XX
	make_connection();

	strcpy(mName, "Archer");
	//to initialize encoder readings because each read is relative to the last read
	this->readSensors();
	cout << "Archer is ready!\n";
}

void Archer::make_connection() {
	int status = device.Connect("/dev/ttyACM0");
	if(status != RQ_SUCCESS)
	{
		cout<<"Error connecting to device..."<<endl;
		cout << "testing connection 2 times" << endl;
		for (int i = 0; i < 2; ++i)
		{
			status = device.Connect("/dev/ttyACM0");
			usleep(1000);
		}
		status = device.Connect("/dev/ttyACM0");
		if(status != RQ_SUCCESS) {
			cout << "Check power connection for MC" << endl;
			exit(1);
		}
	}
	device.SetConfig(_AMOD, 3, 1);	
}

Archer::~Archer() {
	device.SetCommand(_EX);
	sleepms(10);
	int status;
	//left_motor command
	if((status = device.SetCommand(_S, left_motor, 0)) != RQ_SUCCESS) {
		cout<<"left_motor speed_set failed with exit status: " << status << endl;
		//exit(1);
	}
	//right_motorcommand
	if((status = device.SetCommand(_S, right_motor, 0)) != RQ_SUCCESS) {
		cout<<"right_motorspeed_set failed with exit status: " << status << endl;
		//exit(1);
	}
	sleepms(10);
	device.SetCommand(_MG);
	sleepms(10);
	device.Disconnect();
	cout << "Archer Robot Closed!\n";
}

//no sleep time in code
//reduciton ratio 1:71
int Archer::readSensors()
{
	// Get robot displacement from encoders
	int rel_count_1;
	int rel_count_2;
	int status;

	if((status = device.GetValue(_CR, left_motor, rel_count_1)) != RQ_SUCCESS) {
		cout <<"ERROR: left_motor encoder reading failed with exit status: " << status << endl;
		cout << endl;
		//exit(1);
	}
	if((status = device.GetValue(_CR, right_motor, rel_count_2)) != RQ_SUCCESS) {
		cout <<"ERROR: right_motorencoder reading failed with exit status: " << status << endl;
		cout << endl;
		//exit(1);
	}

	//read RPM values
	//Read RPM value
	int result = -10;
	if((status = device.GetValue(_S, left_motor, result)) != RQ_SUCCESS)
		cout<<"RPM 1 failed --> "<<status<<endl;
	else
		cout<<"motor 1 RPM "<<result<<endl;
	if((status = device.GetValue(_S, right_motor, result)) != RQ_SUCCESS)
		cout<<"RPM 2 failed --> "<<status<<endl;
	else
		cout<<"motor 2 RPM "<<result<<endl;



	//Compute wheel linear displacements
	mDisplacementLeft = rel_count_1 * mEncoderScaleFactor;
	mDisplacementRight= rel_count_2 * mEncoderScaleFactor;
	
	//Compute robot average displacement and rotation
	mDisplacement = (mDisplacementLeft + mDisplacementRight) / 2.0;
	mRotation = -(mDisplacementRight - mDisplacementLeft) / mTrack;  //Changed to 0 by MQ
	
	cout << "ARCHER ACTUAL SPEED: " << " " << mDisplacementLeft/mEncoderScaleFactor/mPeriod << " " << mDisplacementRight/mEncoderScaleFactor/mPeriod << " " << mDisplacement << " " << math_functions::rad2deg(mRotation) << endl;
	
	//*****************//
	//CHECK analog sensors
	int analog_input_channel = 3;
	if((status = device.GetValue(_AI, analog_input_channel, mCurrent)) != RQ_SUCCESS)
		cout<<"ERROR: could not retrieve motor amps: "<< status << endl;


	cout << "Motor Amps: " << mCurrent << endl;
	//****************//


	return DATA_READY;
}

void Archer::setActuators(vector<int> &MotorSpeed) {
	//assuming MotorSpeed is an array of size two
	//first value is speed of left_motor in [rot/min]
	//second value is speed of right_motor in [rot/min]

	if(MotorSpeed[LEFT] > MAX_COMMAND_SPEED) {
		MotorSpeed[LEFT] = MAX_COMMAND_SPEED;
	}
	if(MotorSpeed[RIGHT] > MAX_COMMAND_SPEED) {
		MotorSpeed[RIGHT] = MAX_COMMAND_SPEED;
	}

	cout << "speed received in vector function: " << endl;
	cout << "\t\tLEFT RPM: " << MotorSpeed[0] << endl;
	cout << "\t\tRIGHT RPM: " << MotorSpeed[1] << endl;

	bool stop = false;
	if(MotorSpeed[LEFT] == 0 && MotorSpeed[RIGHT] == 0) {
		device.SetCommand(_EX);
		stop = true;
	}
	
	int status;
	//left_motor command
	if((status = device.SetCommand(_S, left_motor, MotorSpeed[LEFT])) != RQ_SUCCESS) {
		cout<<"left_motor speed_set failed with exit status: " << status;
		//exit(1);
	}
	//right_motorcommand
	if((status = device.SetCommand(_S, right_motor, MotorSpeed[RIGHT])) != RQ_SUCCESS) {
		cout<<"right_motorspeed_set failed with exit status: " << status;
		//exit(1);
	}

	cout << "ARCHER SET SPEED: " << MotorSpeed[LEFT] << " " << MotorSpeed[RIGHT] << endl;

	if(stop) {
		device.SetCommand(_MG);
	}
	checkTimming();
}

void Archer::setActuators(float speed, float rate)
{
	int counts_sec_aux[2];
	speedRate2Counts(speed, rate, counts_sec_aux);
	std::vector<int> counts_sec;
	counts_sec.push_back(counts_sec_aux[0]);
	counts_sec.push_back(counts_sec_aux[1]);

	// Make sure that if not zero, it sets some speed
	if(!counts_sec[0] && counts_sec_aux[0]) counts_sec[0] = (counts_sec_aux[0] > 0) ? 1 : -1;
	if(!counts_sec[1] && counts_sec_aux[1]) counts_sec[1] = (counts_sec_aux[1] > 0) ? 1 : -1;

	//convert from [counts/sec] to [rot/min]
	counts_sec[0] *= 1/COUNTS_REVOLUTION * 60;
	counts_sec[1] *= 1/COUNTS_REVOLUTION * 60; 

	//Send motor commands
	setActuators(counts_sec);
	//cout << "ARCHER SPEED RATE: " << speed << " " << math_functions::rad2deg(rate) << endl;
}

int Archer::read_encoders() {
	int rel_count_1;
	int status;

	if((status = device.GetValue(_CR, left_motor, rel_count_1)) != RQ_SUCCESS) {
		cout <<"left_motor encoder reading failed with exit status: " << status << endl;
		//exit(1);
	}
	

	return rel_count_1;
}

