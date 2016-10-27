#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include "CruizCoreGyro.h"
#include "MathFunctions.h"
#include "ibmcom3.h"  		/* for serial */
#include <ctype.h>  		/* required for the isalnum function */
#include <conio.h>
#include <math.h>
#include <dos.h>

#define MAXSIZE   100		/* GPS at most, sends 80 or so chars per message string.  So set maximum to 100 */
#define CR        0x0d		//terminating character for gps signal
#define COMMA	  0x2C
#define LF        0x0a

sruct Entry {
	bool getRest;
	string message;
}entry;

//needs to be saved between calls in case of receiving partial strings
unsigned char	 stringRead[MAXSIZE]; 		/* Buffer collects chars read from GPS */


using namespace std;

GPSRobot::GPSRobot(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[], char GPS_PORT[]) : CruizCoreGyro(float period, float track, float encoderScaleFactor, int COUNTS_REVOLUTION_in, char GYRO_PORT[])
{
	//initialize read_in variables
	//TODO: adjust packet size for GPS
	PACKET_SIZE = 100;
	SAMPLES = 1000;

 	//TODO: change port name in init_gps_port.sh
 	int i = system("./init_gps_port.sh");
 	if(i != 0) {
  		cout << "failure to initialize gps port" << endl;
  		exit(1);
 	}
 	else if(i == 0){
 		cout << "BAUD RATE SET" << endl;
 	}

	if(-1 == (file_descriptor = open(GPS_PORT,O_RDWR)))
	{
		cout << "Error opening port \n";
		cout << "Set port parameters using the following Linux command:\n stty -F /dev/ttyUSB0 9600 raw\n";
		cout << "You may need to have ROOT access";
	}
	cout << "GPS communication port is ready\n";

	//send initializing info to gps
	//usleep(1500);

	strcpy(mName,"GPSRobot");
	cout << "GPS Robot ready!\n";
	cout << "GPS_PORT: " << GYRO_PORT << endl;

	if (tcflush(file_descriptor, TCIOFLUSH) == 0)
       printf("The input and output queues have been flushed.\n");
    else {
       perror("tcflush error\n");
       exit(1);
   	}

   	readSensors();

   	cout << "GPS Robot ready!" << endl;


}

GPSRobot::~GPSRobot()
{
	close(file_descriptor);
	cout << "Closing GPS port";
}

int GPSRobot::readSensors()
{

	//TODO
	//USE CODE FROM GPS_GGA_communication.c to read packets from gps
	//below is code for gyro
	unsigned char 	*pChar;
  	unsigned char  	dummyChar;
	unsigned char 	data_packet[PACKET_SIZE]; /* Buffer collects chars read from GPS */
	unsigned char  	tempString[MAXSIZE];
	unsigned char  	timeString[12];
	unsigned  long 	utcTime, estTime;		/* Coordinated Universal Time and Eastern Standard Time */
	unsigned  char 	lastCommaPosition;
	unsigned char  	latitudeString[11];
	unsigned char  	latitudeCardinalString[3];
	unsigned char  	longitudeString[12];
	unsigned char  	longitudeCardinalString[3];

	float 	latitude;
	int 	latDegrees;
	float 	latMinutes;

	float 	longitude;
	int 	longDegrees;
	float 	longMinutes;

	int 	numLinesRead = 0;

	//blocking reading
	dummyChar = 'A'; pChar = &dummyChar;

	unsigned char data_packet[PACKET_SIZE * 10];
	int bytes_read;
	//if not making it past this line, dont forget to type into terminal:
	// $stty -F /dev/ttyUSB0 4800 raw

	//does read wait for buffer to fill before reading i.e. blocking or not
	bytes_read = read(file_descriptor,data_packet,PACKET_SIZE * 10);

	if(bytes_read == 0) {
		printf("buffer empty\n");
		return 0;
	}
	unsigned int start_idx = 0;

	while(start_idx != bytes_read) {
		incomplete = false;
		unsigned int mesSize = 0;

		if(!entry.getRest) {
			for (; start_idx < PACKET_SIZE; ++start_idx) {
				if (data_packet[start_idx] == '$') {
					break;
				}
			}
			if (start_idx == bytes_read) {
				printf("no valid transmission sarting pointfound\n");
				return 0;
			}
		}
		//need to determine if it is a full packet
		do {
		  	numLinesRead++;
		  	stringRead[i] = data_packet[start_idx];
			if( (data_packet[start_idx] != '\0') && 
				(isalnum(data_packet[start_idx]) ||  
					isspace(data_packet[start_idx]) || 
					ispunct(data_packet[start_idx])) ) {
				start_idx++;
		 	}
		} while(data_packet[start_idx] != CR && start_idx != bytes_read);
		if(start_idx == bytes_read) {
			entry.getRest = true;
			mesSize = strlen(stringRead);
		}



		/* By this point, a possibly complete GPS string has been read*/
		/* Append the null terminator to the string read */
		stringRead[start_idx+1] = '\0';


		/* Analyze string that we collected */
	  	j = 0;
	  	pChar = stringRead;
	  	while(*(pChar+j) != COMMA && j != mesSize) {
	       tempString[j] = *(pChar+j);
	       j++;
	  	}
	  	if(j == mesSize) {
	  		entry.getRest = true;
	  		entry.message = "heading";
	  	}
	  	tempString[j] = '\0';

	  	/* Check if string we collected is the $GPGGA message */
		if(tempString[3] == 'G' && tempString[4] == 'G' && tempString[5] == 'A') {
			pChar = stringRead;

	      	/* Get UTC time */
	      	j = 7;  /* start of time field */
	      	k = 0;
	      	while(*(pChar+j) != COMMA) {
		   		timeString[k] = *(pChar+j);
		   		j++;
		   		k++;
	      	}
	      	lastCommaPosition = j;
			timeString[k] = '\0';
			sscanf(timeString, "%ld", &utcTime);
			utcHour = (utcTime/10000);   /* extract Hours from long */
			utcMinutes = (utcTime - (utcHour*10000))/100;  /* extract minutes from long */
			utcSeconds = utcTime - (utcHour*10000) - (utcMinutes*100); /* extract seconds from long */

			if(utcHour >= 4 && utcHour <= 23) 
					estHour = utcHour - 4;
			else 
				estHour = utcHour + 20;
			estMinutes = utcMinutes;
			estSeconds = utcSeconds;

			/* NB: %02ld formats long to print 2 chars wide, padding with 0 if necessary */
		    printf("%02ld:%02ld:%02ld UTC = %02ld:%02ld:%02ld EST", utcHour, utcMinutes, utcSeconds, estHour, estMinutes, estSeconds);

		    /* Get lattitude: ddmm.mmmm */
	      	pChar = stringRead;
	      	j = lastCommaPosition + 1;
	      	k = 0;
	      	while(*(pChar+j) != COMMA) {
		   		latitudeString[k] = *(pChar+j);
		   		j++;
		   		k++;
	      	}
	      	lastCommaPosition = j;
	      	latitudeString[k] = '\0';

	      	sscanf(latitudeString, "%f", &latitude);
			latDegrees = (int)(latitude/100);
			latMinutes = (float)(latitude - latDegrees*100);
			printf("\t%02d DEG\t%2.4f MIN", latDegrees, latMinutes);

			/* Get longitude: dddmm.mmmm */
			pChar = stringRead;
			j = lastCommaPosition + 1;
			k = 0;
			while(*(pChar+j) != COMMA) {
			   longitudeString[k] = *(pChar+j);
			   j++;
			   k++;
	      	}
	      	lastCommaPosition = j;
	      	longitudeString[k] = '\0';

	      	sscanf(longitudeString, "%f", &longitude);
	      	longDegrees = (int)(longitude/100);
	      	longMinutes = (float)(longitude - longDegrees*100);
	      	printf("\t%03d DEG\t%2.4f MIN", longDegrees, longMinutes);

	      	printf("\n");
	    }
	    else {
	    	printf("Not GGA format");
	    	/* TODO 
	    		1.) re-search buffer string for another $
	    		2.) have this running in background somehow, so that GGA is already established
	    	*/
	    }
	}


	// if (tcflush(file_descriptor, TCIOFLUSH) == 0)
 //           printf("The input and output queues have been flushed.\n");
 //        else
 //           perror("tcflush error\n");


	//may need to double packet size to deal with extra info in buffer
	//may need to do this because


	//Get Encoder information
	Archer::readSensors();

	//read from file
	//8 bites

	//Get angle from CruizCore gyro

	short header;
	short rate_int;
	short angle_int;
	//float rate_float;
	//float angle_float;
	short check_sum;
	unsigned char data_packet[PACKET_SIZE * 10];


	//read(file_descriptor,data_packet,PACKET_SIZE*100);

	//if you flush right here then there is not enough time for the buffer to completely refill
	//you only get a few bytes when readin, not a full 8
	int hold;
	//if not making it past this line, dont forget to type into terminal:
	// $stty -F /dev/ttyUSB0 115200 raw
	hold = read(file_descriptor,data_packet,PACKET_SIZE * 10);
	if(PACKET_SIZE != hold) {
		cout << "ERROR: # of bytes actually read(8): " << hold << endl;
		mRotation = 0;
		return 0;
	}

	// Verify data packet header 
	memcpy(&header,data_packet,sizeof(short));
	if(header != (short)0xFFFF)
	{
		cout << "ERROR: HEADER(" << (short)0xFFFF << "): " << header << endl;
		mRotation = 0;
		return 0;
	}

	// Copy values from data string 
	memcpy(&rate_int,data_packet+2,sizeof(short));
	memcpy(&angle_int,data_packet+4,sizeof(short));
	memcpy(&check_sum,data_packet+6,sizeof(short));

	// Verify checksum
	if(check_sum != (short)(0xFFFF + rate_int + angle_int))
	{
		cout<< "ERROR: checksum\n";
		mRotation = 0;
		return 0;
	}

	static float s_last_angle = 0;

	// Apply scale factors
	//rate_float = rate_int/100.0;
 	//angle_float = angle_int/100.0;
	
	//cout << "rate_float:" << rate_float << " [deg/sec]" << endl << "angle_float: " << angle_float << " [deg]\n";

	mRotation = (angle_int - s_last_angle)/100.0;

	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);

	s_last_angle = angle_int;

 	cout << "GPS location: " << math_functions::rad2deg(mRotation) << endl;


/*

	static float s_last_angle = 0;
	short new_angle = pIic->Raw[mGyroPort][pIic->Actual[mGyroPort]][0]*256 + pIic->Raw[mGyroPort][pIic->Actual[mGyroPort]][1];
	mRotation = - (new_angle - s_last_angle)/100.0; //XGL angle must be inverted
	mRotation = math_functions::deg2rad(mRotation);
	mRotation = math_functions::unwrap(mRotation);
	s_last_angle = new_angle;
	
	cout << "XG1300L: " << math_functions::rad2deg(mRotation) << endl;
*/
	return DATA_READY;
}

