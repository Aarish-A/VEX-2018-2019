#pragma config(Sensor, in1,    anglerPoti,     sensorPotentiometer)
#pragma config(Sensor, in2,    decapperPoti,   sensorPotentiometer)
#pragma config(Sensor, in8,    ballDetector,   sensorLineFollower)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  LED1,           sensorLEDtoVCC)
#pragma config(Motor,  port1,           decapper,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           driveLY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           intake,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           driveL,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooter,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterY,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           driveR,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
//#define FORCE_AUTO

int gBatteryLevel;
int gBackupBatteryLevel;

#define MAX_ANGLE_TIME 1000
#define SHOOTER_SHOOT_UNLOADED_TIME 790
#define SHOOTER_SHOOT_LOADED_TIME 500

#define DECAPPER_STATE_LOGS 1

//#define ANGLER_LOGS 1
#define ANGLER_STATE_LOGS 1
bool anglerDatalog = false;
bool anglerAlgLogs = false;

#define SHOOTER_LOGS 1

#define MACRO_LOGS 1

#define AUTO_LOGS 1

#include "../src/log_toggle.h"

#define LOG(machineIn) if(machineIn##Logs) writeDebugStreamLine

unsigned char _hogLevel = 0;
void tHog()
{
	if (!_hogLevel++)
		hogCPU();
}

void tRelease()
{
	if (_hogLevel && !--_hogLevel)
		releaseCPU();
}

typedef enum _tTurnDir
{
	turnNone,
	turnCW,
	turnCCW
} tTurnDir;

/* Includes */
// Year-independent libraries (headers)
#include "../src/cycle.h"
#include "motors.h"
#include "utilities.h"
#include "tracking.h"
#include "custom_turning.h"

// Year-independent libraries (source)
#include "cycle.c"
#include "motors.c"
#include "utilities.c"
#include "tracking.c"
#include "custom_turning.c"


void setDrive(word left, word right)
{
	setMotor(driveL, left);
	setMotor(driveLY, left);
	setMotor(driveR, right);
	setMotor(driveRY, right);
}

bool autoLogs = true;
#include "drive_algs.h"
#include "drive_algs.c"

#define RED_FRONT_X (69 - BACK_OFFSET)
#define RED_FRONT_Y 11
#define RED_BACK_X (115.875 - BACK_OFFSET)
#define RED_BACK_Y 11

#define BACK_OFFSET (3.875+S_DISTANCE_IN)

task main()
{
	//clearDebugStream();
	datalogClear();
	gBatteryLevel = nImmediateBatteryLevel;
	writeDebugStreamLine("%d battery:%d", nPgmTime, gBatteryLevel);
	setupMotors();
	resetTracking(gPosition, gVelocity, (144-BACK_OFFSET), 14, -90);
	sleep(100);

	//turnToAngleAccurate((23-90), cw, 70, -15, 0);

	//setDrive(31, -31);
	//while (true)
	//{
	//	writeDebugStreamLine("%f", gVelocity.a*100);
	//datalogAddValueWithTimeStamp(0, gVelocity.a);
	//sleep(10);
	//}

	//setDrive(80, -80);
	//while (gVelocity.a < 1.5) sleep(10);
	//sTurnState state;
	//state.time = nPgmTime;
	//state.lstTime = state.time;
	//state.nextDebug = 0;
	//state.input = gVelocity.a;
	//state.power = state.error = state.integral = 0;
	//state.target = 0.749;
	//while (true)
	//	{
	//		//float a = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
	//		turnAccurateInternalCw(200, state);
	//	}

	turnToTargetAccurate(10, 63, ch, 50, 50, 0);

	// reset
	//setDrive(-60,-15);
	//LOG_AUTO(("%d motor:%d", nPgmTime, gMotor[driveL].powerCur));
	//sleep(350);
	//do{
	//	LOG_AUTO(("%d vel:%f", nPgmTime, gVelocity.a));
	//	sleep(10);
	//}while(gVelocity.a < -0.05);
	//setDrive(0,0);
	//setDrive(-15,-60);
	//LOG_AUTO(("%d motor:%d", nPgmTime, gMotor[driveL].powerCur));
	//sleep(350);
	//do{
	//	LOG_AUTO(("%d vel:%f", nPgmTime, gVelocity.a));
	//	sleep(10);
	//}while(gVelocity.a > 0.05);
	//setDrive(-16,-16);


	//Set up turn state structure variables for the internal turnAccurate algorithms
	//sTurnState state;
	//state.time = nPgmTime;
	//state.lstTime = state.time;
	//state.nextDebug = 0;
	//state.input = gVelocity.a;
	//state.power = state.error = state.integral = 0;

	//setDrive(-80, 80);
	//while (gVelocity.a > -6) sleep(10);
	//		LOG(auto)("%d done fast", nPgmTime);

	//		unsigned long slowStartTime = nPgmTime;

	//		while ((nPgmTime-slowStartTime) < 4000)
	//		{
	//			//float target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) - gPosition.a, PI * 2);
	//			turnAccurateInternalCcw(-90, state);
	//		}

	//turnToTargetAccurate(40, 0, ch, 80, 80, 0);
	//turnToTargetP(10, 0, 35, 10, true, 60);

}
