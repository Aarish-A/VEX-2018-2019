#pragma config(Sensor, in1,    autoPoti,       sensorPotentiometer)
#pragma config(Sensor, in2,    mobilePoti,     sensorPotentiometer)
#pragma config(Sensor, in3,    liftPoti,       sensorPotentiometer)
#pragma config(Sensor, in4,    armPoti,        sensorPotentiometer)
#pragma config(Sensor, in5,    limLift,         sensorAnalog)
#pragma config(Sensor, in6,    lsBarL,         sensorReflection)
#pragma config(Sensor, in7,    lsBarR,         sensorReflection)
#pragma config(Sensor, in8,    lsMobile,       sensorReflection)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  sonarL,         sensorSONAR_mm)
#pragma config(Sensor, dgtl9,  limArm,         sensorTouch)
#pragma config(Sensor, dgtl10, jmpSkills,      sensorDigitalIn)
#pragma config(Sensor, dgtl11, sonarR,         sensorSONAR_mm)
#pragma config(Motor,  port2,           liftR,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           driveL1,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           driveL2,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           arm,           tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           mobile,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           driveR2,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           driveR1,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           liftL,         tmotorVex393HighSpeed_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#define DATALOG_BATTERY -1

#define CHECK_POTI_JUMPS 1
#define DRIVE_TURN (gJoy[JOY_TURN].cur - gJoy[JOY_TURN].deadzone * sgn(gJoy[JOY_TURN].cur))
// Year-independent libraries (headers)
#include "../src/task.h"
#include "motors.h"
#include "sensors.h"
#include "joysticks.h"
#include "cycle.h"
#include "utilities.h"
#include "auto.h"

// Year-independent libraries (source)
#include "task.c"
#include "motors.c"
#include "sensors.c"
#include "joysticks.c"
#include "cycle.c"
#include "utilities.c"

#include "state.h"
#include "func_to_state.h"
//#include "state_test.h"

/* Example of how to use state.h and func_to_state.h libraries */
CREATE_MACHINE_3(drive, trackL, Idle, Break, Manual, float, Vel, int, Power); //Used in main.c file to create a state machine that has two variables it looks at (in this case, float driveVel and float drivePower)
																																							//The sensor that you pass it (trackL in this example), is used by the safety functions for velocity-safety-checking
																																							//CREATE_MACHINE_stateCount(machine, sensor, state0, state1, state2, ..stateX, type1, arg1Name, type2, arg2Name);

/* ///////////////////////////////////////////////////////////////////////////////////////////////////////// */
/* Used to declare a function in its respective header file */
PREP_FUNC_STATE_VOID_4(void, foo, int, a, int, b, int, c, int, d); //Use PREP_FUNC_STATE_argCount(functionType, functionName, argType1, arg1, argType2, arg2, ...argTypeX, argX);
																																		//in lieu of declaring that function. It declares the function and sets up all the helper functions/variables that the state machine lib needs.
PREP_FUNC_STATE_4(int, bar, int, a, int, b, int, c, int, d);
ADD_FUNCS_TO_MACHINE_5(drive, foo, bar, b, c, d); //Prepares functions so that they can be linked to a state machine
																										//The suffix is the amount of functions you want to add to the state machine
																										//The arguments are the function names

/* ///////////////////////////////////////////////////////////////////////////////////////////////////////// */

/* The following code goes in the main c file */
void foo(int a, int b, int c, int d) //Example function that will be linked to the drive state machine (declared above)
{
	LOG(drive)("%d Start foo", npgmtime);
	WHILE(drive, npgmtime < a)
	{
		sleep(10);
	}
	LOG(drive)("%d time passed - exit foo", nPgmTime); //Another example function that will allso be linked to the drive state machine
}
int bar(int a, int b, int c, int d)
{
	LOG(drive)("%d Start bar", npgmtime);
	return a;
}

task driveSet() //For any subsystem-state-machine, declare a task called machine##Set
									//Put state definitions inside of this task
{
	driveLogs = true; //Global bool machine##Logs is used to turn logging for a subsystem on or off
	while (true)
	{
		switch(driveState)
		{
			case driveIdle: //state 0
				driveBlocked = false;
				break;
			case driveBreak: //state 1
				driveBlocked = false;
				break;
			case driveManual: //state 2
				driveBlocked =  false;
				break;

			ADD_FUNC_TO_SWITCH_VOID_4(foo, drive, driveIdle, driveIdle); //state 3
																																		//Use ADD_FUNC_TO_SWITCH_argCountOfFunc(func, machine, nextState, safetyState);
																																		//to link a function state to a state machine
			ADD_FUNC_TO_SWITCH_4(bar, drive, driveIdle, driveIdle); //state 4
		}
		sleep(10);
	}
}

void driveCode() //Arbitrary sequence of actions that is executed in main task
{
	writeDebugStreamLine("Log drive %d", driveLogs);
	ASSIGN_FUNC_STATE_4(foo, 3000, 2, 3, 4); //Use ASSIGN_FUNC_STATE_argCountOfFunc(func, arg1, arg2, arg3, arg4);
																						//to assign to the arguments of a function-state
	driveStateChange(drivefoo, 1, 7000); //Use machine##StateChange(int stateIn, bool await =  0, long timeout = -1, float velSafetyThresh = -1, tVelDir velDir = -1, type1 arg1In = -1, type2 arg2In = -1);
																				//to put a machine into stateIn.
																				//If await is set to true, it will wait for the state machine to exit the state you put it in, before executing the next line of code
																				//If a value other than -1 is passed to timeout and velSafetyThresh, the machine will return to its safetyState (defined with CREATE_MACHINE_3) if either of the safeties are tripped
																				//Last two arguments let you pass arguments to the state machine. These are used when entering a non-function-state state, that used the stateMachineArguments

	//TODO:
		//In the driveStateChange function calls, try changing await to 0. See how the output compares
		//Try changing the safety arguments you pass to driveStateChange (i.e pass a timeout argument of less than 3000)

	ASSIGN_FUNC_STATE_4(bar, 9, 2, 3, 4);
	driveStateChange(drivebar, 1,  2000);
	LOG(drive)("%d a = %d", npgmtime, barRet); //Will only display log if driveLogs is set to true
}

task main()
{
	startTask(driveSet); //Run the machine##Set task asynchronously in the background of main tasks that use that subsystem
	driveCode(); //Do something
	while(true) //Blocking code to prevent us from exiting the task
	{
		sleep(10);
	}

	stopTask(driveSet);
}
