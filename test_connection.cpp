#include <iostream>
#include <stdio.h>
#include <string.h>

#include "RoboteqDevice.h"
#include "ErrorCodes.h"
#include "Constants.h"

using namespace std;

int main()
{
	string response = "";
	RoboteqDevice device;
	int status = device.Connect("/dev/ttyACM0");

	cout << "made it past connection function..." << endl;

	if(status != RQ_SUCCESS)
	{
		cout<<"Error connecting to device: "<<status<<"."<<endl;
		return 1;
	}

	cout << "did something right" << endl;

	device.Disconnect();
	return 0;
}