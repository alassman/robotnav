OBJS = Robot.o Archer.o Odometry.o Control.o CruizCoreGyro.o RoboteqDevice.o InputKeys.o Keyboard.o IrRemote.o MathFunctions.o tcpconnector.o tcpacceptor.o tcpstream.o TCPComm.o TCPServer.o Apriltags.o Waypoint.o
OBJS_t = Robot.o Archer.o Odometry.o Control.o CruizCore_test.o RoboteqDevice.o InputKeys.o Keyboard.o IrRemote.o MathFunctions.o tcpconnector.o tcpacceptor.o tcpstream.o TCPComm.o TCPServer.o Apriltags.o Waypoint.o
CC = g++
CFLAGS = -Wall -Werror -static
TARGET = main

all: $(TARGET)
$(TARGET): $(OBJS) main.cpp
	$(CC) $(CFLAGS) $(OBJS) main.cpp -o $@ 
Gyro_test: $(OBJS_t) main.cpp
	$(CC) $(CFLAGS) $(OBJS_t) main.cpp -o $@ 
CruizCore_test.o: CruizCore_test.cpp CruizCore_test.h
	$(CC) $(CFLAGS) -c CruizCore_test.cpp 	
Robot.o: Robot.cpp Robot.h
	$(CC) $(CFLAGS) -c Robot.cpp 
Archer.o: Archer.cpp Archer.h RoboteqDevice.o Robot.o MathFunctions.o
	$(CC) $(CFLAGS) -c Archer.cpp
Odometry.o: Odometry.cpp Odometry.h MathFunctions.o
	$(CC) $(CFLAGS) -c Odometry.cpp
Control.o: Control.cpp Control.h MathFunctions.o Odometry.o
	$(CC) $(CFLAGS) -c Control.cpp
CruizCoreGyro.o: CruizCoreGyro.cpp CruizCoreGyro.h Archer.o
	$(CC) $(CFLAGS) -c CruizCoreGyro.cpp
InputKeys.o: InputKeys.cpp InputKeys.h
	$(CC) $(CFLAGS) -c InputKeys.cpp
Keyboard.o: Keyboard.cpp Keyboard.h InputKeys.o
	$(CC) $(CFLAGS) -c Keyboard.cpp
IrRemote.o: IrRemote.cpp IrRemote.h InputKeys.o
	$(CC) $(CFLAGS) -c IrRemote.cpp
MathFunctions.o: MathFunctions.cpp MathFunctions.h
	$(CC) $(CFLAGS) -c MathFunctions.cpp
RoboteqDevice.o: RoboteqDevice.cpp RoboteqDevice.h ErrorCodes.h Constants.h
	$(CC) $(CFLAGS) -c RoboteqDevice.cpp
tcpstream.o: tcpstream.cpp tcpstream.h
	$(CC) $(CFLAGS) -c tcpstream.cpp
tcpacceptor.o: tcpacceptor.cpp tcpacceptor.h tcpstream.o
	$(CC) $(CFLAGS) -c tcpacceptor.cpp 
tcpconnector.o: tcpconnector.cpp tcpconnector.h tcpstream.o
	$(CC) $(CFLAGS) -c tcpconnector.cpp 
TCPComm.o: TCPComm.cpp TCPComm.h Odometry.o Archer.o tcpconnector.o
	$(CC) $(CFLAGS) -c TCPComm.cpp 
TCPServer.o: TCPServer.cpp TCPServer.h tcpconnector.o tcpacceptor.o
	$(CC) $(CFLAGS) -c TCPServer.cpp 
Apriltags.o: Apriltags.cpp Apriltags.h TCPServer.o Robot.o Odometry.o MathFunctions.o
	$(CC) $(CFLAGS) -c Apriltags.cpp
Waypoint.o: Waypoint.cpp Waypoint.h TCPServer.o Control.o Odometry.o MathFunctions.o
	$(CC) $(CFLAGS) -c Waypoint.cpp 
test_MC_connection: Robot.o Archer.o RoboteqDevice.o MathFunctions.o test_connection.cpp
	$(CC) $(CFLAGS) Robot.o Archer.o RoboteqDevice.o MathFunctions.o test_connection.cpp -o $@ 
test_Gyro_connection: ccr1050.cpp
	g++ ccr1050.cpp -o $@
test_Gyro_code: test_gyro_code.cpp
	g++ test_gyro_code.cpp -o $@
clean:
	\rm *.o $(TARGET)
