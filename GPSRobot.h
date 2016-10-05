#ifndef GPSRobot_H
#define GPSRobot_H

#include <string>
#include <stdlib.h> 
#include "CruizCoreGyro.h"

class GPSRobot : public CruizCoreGyro
{
private:
	int PACKET_SIZE;
	int SAMPLES;
	int file_descriptor;
public:
		GPSRobot(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[]);
		virtual ~GPSRobot();
		virtual int readSensors();
};

#endif
