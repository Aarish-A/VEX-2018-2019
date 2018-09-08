#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Motor,  port2,            ,             tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           driveL1,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           driveL2,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooterA,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterB,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           driveR2,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           driveR1,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int nBatteryLevel;

//#define FORCE_AUTO 1

#define DATALOG_BATTERY -1

#define CHECK_POTI_JUMPS 1
#define DRIVE_TURN (gJoy[JOY_TURN].cur - gJoy[JOY_TURN].deadzone * sgn(gJoy[JOY_TURN].cur))

//#include "state_test.h"
#include "state.h"
#include "func_to_state.h"

// Year-independent libraries (headers)
#include "task.h"
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

// Other includes
#include "Vex_Competition_Includes_Custom.c"
#include "controls.h"

/* Drive Controls */
void setDrive(word left, word right)
{
	//writeDebugStreamLine("%d l:%d, r:%d", npgmtime, left, right);
	gMotor[driveL1].power = gMotor[driveL2].power = LIM_TO_VAL(left, 127);
	gMotor[driveR1].power = gMotor[driveR2].power = LIM_TO_VAL(right, 127);
}

CREATE_MACHINE_3(drive, trackL, Idle, Break, Manual, float, Vel, int, Power)
//#include "driveTest.c"
#include "auto.c"
#include "drive_algs.h"
#include "drive_algs.c"

//task driveSet()
//{
//	driveLogs = 0;

//	sCycleData drive;
//	initCycle(drive, 10, "drive");
//	while (true)
//	{
//		switch(driveState)
//		{
//			case driveIdle: //0:
//			{
//				driveBlocked = 0;
//				setDrive(0,0);

//				driveStateCycCount++;
//				break;
//			}
//			case driveBreak: //1:
//			{
//				driveBlocked = 0;
//				float startVelL, startVelR;

//				const float velThresh = 0.015;
//				velocityCheck(trackL);
//				velocityCheck(trackR);
//					float left = gSensor[trackL].velocity;
//					float right = gSensor[trackR].velocity;
//					if (driveStateCycCount == 0)
//					{
//						startVelL = left;
//						startVelR = right;
//						LOG(drive)("BreakFirstVel %f, %f", startVelL, startVelR);
//					}

//					LOG(drive)("L: %f, r: %f", left, right);
//					if (( abs(left) < velThresh || sgn(left)!=sgn(startVelL) ) && ( abs(right) < velThresh || sgn(right)!=sgn(startVelR) ) )
//					{
//						driveStateChange(driveIdle);
//					}
//					else
//					{
//						setDrive((abs(left) > velThresh? (LIM_TO_VAL(sgn(left) * -50, 12)) : 0), (abs(right) > velThresh? (LIM_TO_VAL(sgn(right) * -50, 12)) : 0) );
//					}
//					driveStateCycCount++;

//				driveSafetyCheck(driveIdle);
//				break;
//			}
//			case driveManual: //2:
//			{
//				driveBlocked = 0;
//				//float angleToTen = atan2( (gPosition.x - 0), (gPosition.y - 10) );
//				setDrive((LIM_TO_VAL((gJoy[JOY_THROTTLE].cur + DRIVE_TURN), 127)), (LIM_TO_VAL((gJoy[JOY_THROTTLE].cur - DRIVE_TURN), 127)));
//				driveStateCycCount++;
//				sVector newVector;
//				newVector.x = gPosition.x;
//				newVector.y = gPosition.y;
//				LOG(drive)("%d (%f,%f) a:%f", npgmtime, gPosition.x, gPosition.y, gPosition.a);

//				break;
//			}

//			//ADD_FUNC_TO_SWITCH_VOID_4(driveFuncTest, drive, driveBreak, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_7(followLineVec, drive, driveIdle, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_6(followLine, drive, driveIdle, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_6(moveToTargetSimple, drive, driveIdle, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_12(moveToTarget, drive, driveIdle, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_12(moveToTargetDis, drive, driveBreak, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_7(turnToAngleNewAlg, drive, driveBreak, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_9(turnToTargetNewAlg, drive, driveBreak, driveIdle)
//			ADD_FUNC_TO_SWITCH_VOID_7(sweepTurnToTarget, drive, driveBreak, driveIdle)
//		}
//		endCycle(drive);
//	}
//}
//void handleDrive()
//{
//	if (RISING(BTN_DRIVE_TEST))
//	{
//		LOG(drive)("Btn_Drive_Test Pressed");
//		ASSIGN_FUNC_STATE_7(followLineVec, 0, 35, 25, 60, mttProportional, true, (stopSoft | stopHarsh));
//		driveStateChange(drivefollowLineVec, 2000, 0.3, velEither);

//		//ASSIGN_FUNC_STATE_6(moveToTargetSimple, 0, 10, 60, mttProportional, true, true);
//		//driveStateChange(drivemoveToTargetSimple, 2000, 0.3, velEither);

//	}
//	else if (!gJoy[JOY_THROTTLE}.cur && !gJoy[JOY_TURN].cur && driveState == driveManual)
//	{
//		driveStateChange(driveBreak, 150);
//	}
//	else if(gJoy[JOY_THROTTLE}.cur || gJoy[JOY_TURN].cur)
//	{
//		driveStateChange(driveManual);
//	}
//}
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_VAL (360.0*GEAR_RATIO)
#define SHOOTER_LOAD_POS 90

void setShooter(word val)
{
	gMotor[shooterA].power = gMotor[shooterB].power = LIM_TO_VAL(val, 127);

}

CREATE_MACHINE_3(shooter, shooterEnc, Idle, Shoot, Load, int, shotCount, int, Power);
task shooterSet()
{
	sCycleData cycle;
	initCycle(cycle, 10, "lift");
	while (true)
	{
		switch (shooterState)
		{
		case shooterIdle: //0:
			{
				shooterBlocked = 0;
				setShooter(0);
				break;
			}
		case shooterShoot: //1:
			{
				shooterBlocked = 1;
				shooterShotCount++;
				break;
			}
		case shooterLoad: //2:
			{
				shooterBlocked = 1;
				break;
			}
		}
		endCycle(cycle);
	}
}


void handleShooter() //Decide which state to put machine in
{
	LOG(shooter)("State = %d, Shooter loc = %d", shooterState, gSensor[shooterEnc].value);

	if (RISING(BTN_SHOOT))
	{
		if (shooterShotCount == 0 && gSensor[shooterEnc].value < (SHOOTER_LOAD_POS-10)
			shooterStateChange(shooterLoad, 400, 0.2, velUp);
		else
		{
			shooterStateChange(shooterShoot, 500, 0.2, velUp);
			shooterStateChange(shooterLoad, 400, 0.2, velUp);
		}
	}

}


void startTasks()
{
	resetPositionFull(gPosition, 0, 0, 0);

	tStart(autoMotorSensorUpdateTask);

	//tStart(driveSet);
	tStart(shooterSet);

}

void stopTasks()
{
	tStop(trackPositionTask);

	tStop(autoMotorSensorUpdateTask);

	//tStop(driveSet);
	tStop(shooterSet);
}

void startup()
{
	clearDebugStream();
	setupSensors();
	setupMotors();
	setupJoysticks();
	tInit();

	//Setup Joysticks & Buttons
	enableJoystick(JOY_THROTTLE);
	enableJoystick(JOY_TURN);
	enableJoystick(BTN_SHOOT);

	gJoy[JOY_THROTTLE].deadzone = 15;
	gJoy[JOY_TURN].deadzone = 15;
}

void disabled()
{
	updateSensorInputs();
	//selectAuto();
	//handleLcd();
}

task autonomous()
{
	startTasks();
	writeDebugStreamLine("%d Start Autonomous, %d", npgmtime, nBatteryLevel);

	sCycleData auto;
	initCycle(auto, 10, "auto");
	while(true)
	{
		endCycle(auto);
	}

	stopTasks();
}

task usercontrol()
{
	sCycleData cycle;
	initCycle(cycle, 10, "usercontrol");

	startTasks();

	while (true)
	{
		updateJoysticks();
		updateMotors();
		updateSensorInputs();
		updateSensorOutputs();

		//handleDrive();
		handleShooter();

		//setMobileState();

		endCycle(cycle);
	}
	stopTasks();
}
