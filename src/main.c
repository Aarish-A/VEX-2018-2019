#pragma config(Sensor, in1,    ballDetector,   sensorLineFollower)
#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Motor,  port2,           driveLY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           intake,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           driveL,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooter,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterY,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           driveR,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int nBatteryLevel;

#define IGNORE_DISABLE 1

#define FORCE_AUTO 1

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
	gMotor[driveLY].power = gMotor[driveL].power = LIM_TO_VAL(left, 127);
	gMotor[driveRY].power = gMotor[driveR].power = LIM_TO_VAL(right, 127);
}

CREATE_MACHINE_3(drive, trackL, Idle, Break, Manual, float, Vel, int, Power);
//#include "driveTest.c"
#include "auto.c"
#include "drive_algs.h"
#include "drive_algs.c"

task driveSet()
{
	driveLogs = 1; // Toggle Logging

	sCycleData cycle;
	initCycle(cycle, 10, "drive");
	while (true)
	{
		switch(driveState)
		{
		case driveIdle: //0:
			{
				driveBlocked = 0;
				setDrive(0,0);

				driveStateCycCount++;
				break;
			}
		case driveBreak: //1:
			{
				driveBlocked = 0;
				float startVelL, startVelR;

				const float velThresh = 0.015;
				velocityCheck(trackL);
				velocityCheck(trackR);
				float left = gSensor[trackL].velocity;
				float right = gSensor[trackR].velocity;
				if (driveStateCycCount == 0)
				{
					startVelL = left;
					startVelR = right;
					LOG(drive)("BreakFirstVel %f, %f", startVelL, startVelR);
				}

				LOG(drive)("L: %f, r: %f", left, right);
				if (( abs(left) < velThresh || sgn(left)!=sgn(startVelL) ) && ( abs(right) < velThresh || sgn(right)!=sgn(startVelR) ) )
				{
					driveStateChange(driveIdle);
				}
				else
				{
					setDrive((abs(left) > velThresh? (LIM_TO_VAL(sgn(left) * -50, 12)) : 0), (abs(right) > velThresh? (LIM_TO_VAL(sgn(right) * -50, 12)) : 0) );
				}
				driveStateCycCount++;

				driveSafetyCheck(driveIdle);
				break;
			}
		case driveManual: //2:
			{
				driveBlocked = 0;
				//float angleToTen = atan2( (gPosition.x - 0), (gPosition.y - 10) );
				setDrive((LIM_TO_VAL((gJoy[JOY_THROTTLE].cur + DRIVE_TURN), 127)), (LIM_TO_VAL((gJoy[JOY_THROTTLE].cur - DRIVE_TURN), 127)));
				driveStateCycCount++;

				LOG(drive)("%d (%f,%f) a:%f", npgmtime, gPosition.x, gPosition.y, gPosition.a);

				break;
			}

			//ADD_FUNC_TO_SWITCH_VOID_4(driveFuncTest, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_7(followLineVec, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_6(followLine, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_6(moveToTargetSimple, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_12(moveToTarget, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_12(moveToTargetDis, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_7(turnToAngleNewAlg, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_9(turnToTargetNewAlg, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_VOID_7(sweepTurnToTarget, drive, driveBreak, driveIdle)
		}
		endCycle(cycle);
	}
}
void handleDrive()
{
	if (!gJoy[JOY_THROTTLE}.cur && !gJoy[JOY_TURN].cur && driveState == driveManual)
	{
		driveStateChange(driveBreak, false, 150);
	}
	else if(gJoy[JOY_THROTTLE}.cur || gJoy[JOY_TURN].cur)
	{
		driveStateChange(driveManual);
	}
}

CREATE_MACHINE_3(intake, trackL, Stop, Up, Down, int, Dir, int, Power);
#define INTAKE_UP_POWER 127
#define INTAKE_DOWN_POWER -127

task intakeSet()
{
	intakeLogs = 1; // Toggle Logging

	sCycleData cycle;
	initCycle(cycle, 10, "drive");
	while (true)
	{
		switch(intakeState)
		{
			case intakeStop:
				gMotor[intake].power = 0;
				break;
			case intakeUp:
				gMotor[intake].power = INTAKE_UP_POWER;
				break;
			case intakeDown:
				gMotor[intake].power = INTAKE_DOWN_POWER;
				break;
		}
		endCycle(cycle);
	}
}
void handleIntake()
{
		if (RISING(BTN_INTAKE_UP))
		{
			LOG(intake)("%d Intake Up Pressed", npgmtime);
			if (intakeState != intakeStop)
				intakeStateChange(intakeStop, false);
			else
				intakeStateChange(intakeUp, false);
		}
		else if (RISING(BTN_INTAKE_DOWN))
		{
			LOG(intake)("%d Intake Down Pressed", npgmtime);
			if (intakeState != intakeStop)
				intakeStateChange(intakeStop, false);
			else
				intakeStateChange(intakeDown, false);
		}
}

CREATE_MACHINE_5(shooter, shooterEnc, Break, Idle, Reload, Hold, Shoot, int, Dir, int, Power);
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_VAL (360.0*SHOOTER_GEAR_RATIO)
#define SHOOTER_RELOAD_HOLD 11
#define SHOOTER_RELOAD_POS 145

int shooterShotCount;

void setShooter(word val)
{
	motor[shooter] = motor[shooterY] = LIM_TO_VAL(val, 127);
}

task shooterSet()
{
	shooterShotCount = 0;
	float shooterBreakOffset = 6;

	resetQuadratureEncoder(shooterEnc);

	shooterLogs = 1; // Toggle Logging

	sCycleData cycle;
	initCycle(cycle, 10, "shooter");
	while (true)
	{
		switch(shooterState)
		{
			case shooterBreak;
				shooterShotCount--;
				setShooter(-90);
				unsigned long timerStart = npgmtime;
				WHILE( shooter, (npgmTime-timerStart) < 100 ) sleep(10);
				setShooter(0);
				PlayTone(300, 50);

				shooterStateChange(shooterIdle);
				break;
			case shooterIdle:
				setShooter(0);
				break;
			case shooterReload:
				shooterTimeout = 10;

				writeDebugStreamLine("Shooter TO: %d", shooterTimeout);

				setShooter(127);
				LOG(shooter)("Setup shot #%d encoder start= %d", shooterShotCount,gSensor[shooterEnc].value);
				int target = shooterShotCount * SHOOTER_RELOAD_VAL;
				unsigned long reloadStartTime = npgmtime;
				WHILE(shooter, gSensor[shooterEnc].value < (target + SHOOTER_RELOAD_POS))
				{
					shooterVelSafetyCheck();
					sleep(10);
					//LOG(shooter)("Enc: %d", gSensor[shooterEnc].value);
				}
				LOG(shooter)("Enc: %d", gSensor[shooterEnc].value);
				LOG(shooter)("Reload time = %d", npgmtime-reloadStartTime);
				setShooter(SHOOTER_RELOAD_HOLD);

				shooterSafetyCheck();
				shooterStateChange(shooterHold);
				break;
			case shooterHold:
				setShooter(SHOOTER_RELOAD_HOLD);
				break;
			case shooterShoot:
				//unsigned long shotStartTime = npgmtime;
				//shooterShotCount++;
				//target = shooterShotCount * SHOOTER_RELOAD_VAL;
				//setShooter(127);
				//LOG(shooter)("Start shot #%d at %d, Tgt: %d, Enc: %d, Err: %d", shooterShotCount, nPgmTime, gSensor[shooterEnc].value, target, target - gSensor[shooterEnc].value);

				//bool shotTargReached = ( gSensor[shooterEnc].value > (target-shooterBreakOffset) );
				//WHILE (shooter, !shotTargReached && (gSensor[shooterEnc].value < (target-80) || gSensor[shooterEnc].value > (target-15) || gSensor[ballDetector].value < 1000))
				//{
				//	shotTargReached = ( gSensor[shooterEnc].value > (target-shooterBreakOffset) );

				//	unsigned long timeElpsd = npgmtime-shotStartTime;
				//	sleep(10);
				//}
				//if (!shotTargReached)
				//{
				//	LOG(shooter)("%d Ball gone: Val:%d, Time: %d Pos:%d, Targ:%d ", npgmtime, gSensor[ballDetector].value, npgmtime-shotStartTime, gSensor[shooterEnc].value, target);
				//	shooterStateChange(shooterBreak);
				//}
				//else
				//{
				//	LOG(shooter)("%d Shot done accelerating: Time: %d Pos:%d ", npgmtime, npgmtime-shotStartTime, gSensor[shooterEnc].value);
				//	setShooter(-22);
				//	unsigned long startBreakTime = npgmtime;
				//	WHILE (drive, (npgmtime-startBreakTime) < 80)
				//	{
				//		LOG(shooter)("%d Break", npgmtime);
				//		if (gSensor[ballDetector].value > 2000)
				//		{
				//			LOG(shooter)("	%d Ball hit: Time: %d", npgmtime, npgmtime-shotStartTime);
				//		}
				//		sleep(10);
				//	}
				//	setShooter(0);
				//	LOG(shooter)("%d, Shot done in %dms, Tgt: %d, Enc: %d, Err: %d", nPgmTime, npgmtime-shotStartTime, target, gSensor[shooterEnc].value, target - gSensor[shooterEnc].value);

				//	shooterSafetyCheck();
				//	shooterStateChange(shooterReload);
				//}
				break;
		}
		endCycle(cycle);
	}
}
void handleShooter()
{
	bool cancelledPrint = false;
	int target = shooterShotCount * SHOOTER_RELOAD_VAL;
	if (vexRT[BTN_SHOOT])
	{
		if (gSensor[shooterEnc].value < (target+SHOOTER_RELOAD_POS-10))
		{
			shooterStateChange(shooterReload); //, false, 1000, 0.0001, velUp);
		}
		else if(gSensor[ballDetector].value > 1000)
		{
			if (!cancelledPrint)
			{
				LOG(shooter)("%d No Ball - Shot Cancelled");
				shooterStateChange(shooterIdle);
				cancelledPrint = true;
			}
		}
		else if(gSensor[ballDetector].value >= 250 & gSensor[ballDetector].value <= 252)
		{
			if (!cancelledPrint)
			{
				LOG(shooter)("%d Ball Detector Unplugged - Shot Cancelled");
				shooterStateChange(shooterIdle);
				cancelledPrint = true;
			}
		}
		else
		{
			shooterStateChange(shooterShoot);
			cancelledPrint = true;
		}
	}
}

void startTasks()
{
	resetPositionFull(gPosition, 0, 0, 0);

	//tStart(autoMotorSensorUpdateTask);

	tStart(driveSet);
	tStart(intakeSet);
	tStart(shooterSet);
}

void stopTasks()
{
	tStop(trackPositionTask);

	//tStop(autoMotorSensorUpdateTask);

	tStop(driveSet);
	tStop(intakeSet);
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
	enableJoystick(BTN_INTAKE_UP);
	enableJoystick(BTN_INTAKE_DOWN);
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
	tStart(autoMotorSensorUpdateTask);
	startTasks();
	writeDebugStreamLine("%d Start Autonomous, %d", npgmtime, nBatteryLevel);

	startTasks();
	sleep(100);
	writeDebugStreamLine("%d Starting", npgmtime);
	shooterStateChange(shooterReload);

	sCycleData cycle;
	initCycle(cycle, 10, "auto");
	while(true)
	{
		endCycle(cycle);
	}
	stopTasks();
	tStop(autoMotorSensorUpdateTask);
	stopTasks();
}

task usercontrol()
{
	sCycleData cycle;
	initCycle(cycle, 10, "usercontrol");

	writeDebugStream("%d Start usercontrol",npgmtime);

	startTasks();

	while (true)
	{
		updateJoysticks();
		updateMotors();
		updateSensorInputs();
		updateSensorOutputs();

		handleDrive();
		handleIntake();
		handleShooter();

		//setMobileState();

		endCycle(cycle);
	}
	stopTasks();
}
