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

bool driveLogs = false;
bool driveStateLogs = false;

bool intakeLogs = false;
bool intakeStateLogs = false;

bool decapperStateLogs = true;
bool decapperLogs = true;

bool anglerDatalog = false;
bool anglerStateLogs = false;
bool anglerLogs = false;
bool anglerAlgLogs = false;

bool shooterLogs = true;

bool macroLogs = true;

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

task main()
{
	clearDebugStream();
	gBatteryLevel = nImmediateBatteryLevel;
	writeDebugStreamLine("%d battery:%d", nPgmTime, gBatteryLevel);
	setupMotors();
	resetPositionFull(gPosition, 62, 3.6+S_DISTANCE_IN, 0);
	startTask(trackPositionTask);

	//moveToTargetY(gPosition.y+1.5, 50, 30, stopHarsh);
	turnToTargetP(11, 16, 40, 20, true, 70);
	//turnToTargetSide(11, 16, 70, -15, 10, true);

	//moveToTargetDis(0, 5, 127, 35, 0, 0, 70, 0, stopNone, mttProportional);
	//turnToTargetNewAlg(11, 15, cw, 0.7, 40, 15, true);
	//moveToTarget(24, 24, 127, 7, 15, 3, 50, 0, (stopSoft|stopHarsh), mttProportional);


}
