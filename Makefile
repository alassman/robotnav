OBJS = Robot.o Archer.o Odometry.o Control.o CruizCoreGyro.o RoboteqDevice.o InputKeys.o Keyboard.o IrRemote.o MathFunctions.o
CC = g++
CFLAGS = -Wall -static
TARGET = main

all: $(TARGET)
$(TARGET): $(OBJS) main.cpp
	$(CC) $(CFLAGS) $(OBJS) main.cpp -o $@ 
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
test_MC_connection: Robot.o Archer.o RoboteqDevice.o MathFunctions.o test_connection.cpp
	$(CC) $(CFLAGS) Robot.o Archer.o RoboteqDevice.o MathFunctions.o test_connection.cpp -o $@ 
test_Gyro_connection: ccr1050.cpp
	g++ ccr1050.cpp -o test_Gyro_connection
clean:
	\rm *.o $(TARGET)
