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

task driveSet()
{
	bool autoLogs = 1;
	driveLogs = 1;

	sCycleData drive;
	initCycle(drive, 10, "drive");
	while (true)
	{
		switch(driveState)
		{
			case driveIdle: //0:
			{
				setDrive(0,0);

				driveStateCycCount++;
				break;
			}
			case driveBreak: //1:
			{
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
				//float angleToTen = atan2( (gPosition.x - 0), (gPosition.y - 10) );
				setDrive((LIM_TO_VAL((gJoy[JOY_THROTTLE].cur + DRIVE_TURN), 127)), (LIM_TO_VAL((gJoy[JOY_THROTTLE].cur - DRIVE_TURN), 127)));
				driveStateCycCount++;
				sVector newVector;
				newVector.x = gPosition.x;
				newVector.y = gPosition.y;
				LOG(auto)("%d (%f,%f) a:%f", npgmtime, gPosition.x, gPosition.y, gPosition.a);

				break;
			}
			//ADD_FUNC_TO_SWITCH_4(driveFuncTest, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_6(followLine, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_6(moveToTargetSimple, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_12(moveToTarget, drive, driveIdle, driveIdle)
			ADD_FUNC_TO_SWITCH_12(moveToTargetDis, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_7(turnToAngleNewAlg, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_9(turnToTargetNewAlg, drive, driveBreak, driveIdle)
			ADD_FUNC_TO_SWITCH_7(sweepTurnToTarget, drive, driveBreak, driveIdle)
		}
		endCycle(drive);
	}
}
void handleDrive()
{
	if (RISING(BTN_DRIVE_TEST))
	{
		LOG(drive)("Btn_Drive_Test Pressed");
		ASSIGN_FUNC_STATE_6(followLine, 0, 10, 60, mttProportional, true, (stopSoft | stopHarsh));
		driveStateChange(drivefollowLine, 2000, 0.3, velEither);

		//ASSIGN_FUNC_STATE_6(moveToTargetSimple, 0, 10, 60, mttProportional, true, true);
		//driveStateChange(drivemoveToTargetSimple, 2000, 0.3, velEither);

	}
	else if (!gJoy[JOY_THROTTLE}.cur && !gJoy[JOY_TURN].cur && driveState == driveManual)
	{
		driveStateChange(driveBreak, 150);
	}
	else if(gJoy[JOY_THROTTLE}.cur || gJoy[JOY_TURN].cur)
	{
		driveStateChange(driveManual);
	}
}

/* Lift Controls */
#define LIFT_BOTTOM 800 //1100
#define LIFT_TOP (LIFT_BOTTOM + 1930)
#define LIFT_MID (LIFT_BOTTOM + 620)
#define LIFT_HOLD_DOWN_THRESHOLD (LIFT_BOTTOM + 50)
#define LIFT_HOLD_UP_THRESHOLD (LIFT_TOP - 100)

void setLift(word val)
{
gMotor[liftR].power = gMotor[liftL].power = LIM_TO_VAL(val, 127);
}

CREATE_MACHINE_5(lift, liftPoti, Idle, Hold, Manual, Move, MoveSimple, int, Target, int, Power)
task liftSet()
{
	sCycleData lift;
	initCycle(lift, 10, "lift");
	while (true)
	{
		switch (liftState)
		{
		case 0: //idle
			{
				setLift(0);
				break;
			}
		case 1: //hold
			{
				if (gSensor[liftPoti].value < LIFT_HOLD_DOWN_THRESHOLD) //Lift is at bottom
					setLift(!gSensor[limLift].value ? -15 : -90);
				else if (gSensor[liftPoti].value > LIFT_HOLD_UP_THRESHOLD) //Lift is at top
					setLift(gSensor[liftPoti].value >= LIFT_TOP ? 15 : 127);
				else //Lift is in middle
					setLift(gSensor[liftPoti].value > LIFT_MID ? 12 : 10);
				break;
			}
		case 2: //manual
			{
				short joy = gJoy[JOY_LIFT].cur;
				setLift(joy);

				velocityCheck(liftPoti);
				//LOG(lift)("power:%d, vel: %f", joy, gSensor[liftPoti].velocity);

				if ((sgn(joy) == -1 && gSensor[liftPoti].value < LIFT_HOLD_DOWN_THRESHOLD) || (sgn(joy) == 1 && gSensor[liftPoti].value > LIFT_HOLD_UP_THRESHOLD))
					liftStateChange(liftHold);
				break;
			}
		case 3: //move
			{
				if (liftTarget == -1)
			{
				liftStateChange(liftHold);
			}
			else
			{
				int dir = (liftTarget < gSensor[liftPoti].value)? -1 : 1;
				//abs velocity of arm is 0.3-2.5
				int firstTarg = liftTarget + (dir == 1? -150 : 150);
				float pB = (dir == 1)? 25 : 30; //base power
				float vKP = 0.006; //kP for targVel
				float pKP = 30.0; //kP for power
				float targVel, power;

				while ( (dir == 1)? (gSensor[liftPoti].value < firstTarg) : (gSensor[liftPoti].value > firstTarg) )
				{
					targVel = vKP * (liftTarget - gSensor[liftPoti].value);
					velocityCheck(liftPoti);
						power = LIM_TO_VAL( (pB + (pKP * (targVel - gSensor[liftPoti].velocity))), 127);

						if (abs(gSensor[liftPoti].velocity) < 1.0) // if stalled, apply enough power to move
						{
							if (dir == 1 && power < 27)
								power = 27;
							else if (dir == -1 && power > -27)
								power = -27;
						}

						if ( sgn(power) != sgn(dir) )
							LIM_TO_VAL_SET(power, 5);

						LOG(lift)("Power: %d in dir: %d. vel:%f, velTarg:%f, Loc: %d, Targ: %d", power, dir, gSensor[liftPoti].velocity, targVel, gSensor[liftPoti].value, firstTarg);
						setLift(power);
					sleep(10);
				}

				setLift(0);
				//LOG(lift)("targ: %d", liftTarget);
				velocityCheck(liftPoti);
					while ( abs(gSensor[liftPoti].velocity) > 0.75  && ( (dir == 1)? (gSensor[liftPoti].value < liftTarget) : (gSensor[liftPoti].value > liftTarget) ) )
					{
						velocityCheck(liftPoti);
						LOG(lift)("loc: %d, abs-vel: %f", gSensor[liftPoti].value, abs(gSensor[liftPoti].velocity));
						LOG(lift)("Power: 0 in dir: %d. Loc: %d, Targ: %d", gMotor[lift].power , dir, gSensor[liftPoti].value, liftTarget);
						sleep(10);
					}
				liftStateChange(liftHold);
			}
				break;
			}
		}
		endCycle(lift);
	}
}


void handleLift() //Decide which state to put machine in
{
	LOG(lift)("State = %d, Lift loc = %d, Lift Power = %d", liftState, gSensor[liftPoti].value, gMotor[liftR].power);

	short joy = gJoy[JOY_LIFT].cur;
	if (RISING(JOY_LIFT) && liftState != liftManual)
		{
			liftStateChange(liftManual);
		}
	else if (!joy && liftState == liftManual)//|| abs(gJoy[JOY_ARM].cur) < abs(gJoy[JOY_ARM].lst))
		{
			liftStateChange(liftHold);
		}
	if (RISING(BTN_LIFT_TEST) && liftState != liftMove)
	{
		liftStateChange(liftMove, 1200, -1, -1, LIFT_MID+300, -1);
		//LIFT_STATE(move, LIFT_MID+300, -1);
	}

}


/* Arm Controls */
#define RL_ARM_TOP 2700
#define ARM_TOP (RL_ARM_TOP - 100)

//Actual ARM_BOTTOM = 1020
#define ARM_BOTTOM (RL_ARM_TOP - 1450)

#define ARM_PRESTACK (RL_ARM_TOP - 800)
#define ARM_RELEASE (RL_ARM_TOP - 700)
#define ARM_CARRY (RL_ARM_TOP - 1040)
#define ARM_STACK (RL_ARM_TOP - 100)
//#define ARM_HORIZONTAL (RL_ARM_TOP - 1590)
#define ARM_HOLD_DOWN_THRESHOLD (ARM_BOTTOM + 50)

void setArm(word val)
{
gMotor[arm].power = LIM_TO_VAL(val, 127);
}

CREATE_MACHINE_5(arm, armPoti, Idle, Hold, Manual, Move, MoveSimple, int, Target, int, Power)
task armSet()
{
sCycleData arm;
initCycle(arm, 10, "arm");
while(true)
{
	armLogs = 1;

	switch (armState)
	{
	case 0: //idle
		{
			setArm(0);
			break;
		}
	case 1: //hold
		{
			if (gSensor[armPoti].value < ARM_HOLD_DOWN_THRESHOLD) //Arm is at bottom
				setArm(gSensor[limArm].value ? -15 : -40);
			else
				setArm(7);
			break;
		}
	case 2: //manual
		{
			short joy = -1 * (gJoy[JOY_ARM].cur);
			velocityCheck(armPoti);
			LOG(arm)("power: %d, vel: %f", joy, gSensor[armPoti].velocity);

			setArm(joy);

			if (	(sgn(joy) == -1 && gSensor[armPoti].value < (ARM_BOTTOM + 20)) || (sgn(joy) == 1 && gSensor[armPoti].value > (ARM_TOP - 20)) )
				armStateChange(armHold);

			break;
		}
	case 3: //move
		{
			if (armTarget == -1)
			{
				armStatechange(armHold);
			}
			else
			{
				int dir = (armTarget < gSensor[armPoti].value)? -1 : 1;
				//abs velocity of arm is 0.7-4.0
				int firstTarg = armTarget + (dir == 1? -50 : 200);
				float pB = (dir == 1)? 25 : 0; //base power
				float vKP = 0.006; //kP for targVel
				float pKP = 30.0; //kP for power
				float targVel, power;

				WHILE(drive, ((dir == 1)? (gSensor[armPoti].value < firstTarg) : (gSensor[armPoti].value > firstTarg)) )
				{
				targVel = vKP * (armTarget - gSensor[armPoti].value);
					velocityCheck(armPoti);
						power = LIM_TO_VAL( (pB + (pKP * (targVel - gSensor[armPoti].velocity))), 127);

						if (abs(gSensor[armPoti].velocity) < 2.6) // if stalled, apply enough power to move
						{
							if (dir == 1 && power < 26)
								power = 26;
							else if (dir == -1 && power > -7)
								power = -7;

						if ( sgn(power) != sgn(dir) )
							LIM_TO_VAL_SET(power, (dir == 1)? 4 : 8);

						LOG(arm)("Power: %d in dir: %d. vel:%f, velTarg:%f, Loc: %d, Targ: %d", power, dir, gSensor[armPoti].velocity, targVel, gSensor[armPoti].value, firstTarg);
						setArm(power);
					}
					sleep(10);
				}

				setArm(0);
				LOG(arm)("targ: %d", armTarget);
				velocityCheck(armPoti);
					while ( abs(gSensor[armPoti].velocity) > 0.75  && ( (dir == 1)? (gSensor[armPoti].value < armTarget) : (gSensor[armPoti].value > armTarget) ) )
					{
						velocityCheck(armPoti);
						LOG(arm)("loc: %d, abs-vel: %f", gSensor[armPoti].value, abs(gSensor[armPoti].velocity));
						LOG(arm)("Power: 0 in dir: %d. Loc: %d, Targ: %d", gMotor[arm].power , dir, gSensor[armPoti].value, armTarget);
						sleep(10);
					}
				armStateChange(armHold);
			}
			break;
		}
	case 4: //moveSimple
		{
			if (ArmTarget == -1)
			{
				armStateChange(armHold);
			}
			else
			{
				int dir = (armTarget < gSensor[armPoti].value)? -1 : 1;

				armPower = abs(armPower) * dir;
				LOG(arm)("move: set power %d, dir %d, poti %d", armPower, dir, gSensor[armPoti].value);
				setArm(armPower);
				WHILE(arm, (dir == 1)? (gSensor[armPoti].value < armTarget) : (gSensor[armPoti].value > armTarget) )
					sleep(10);
			}
			break;
		}
	}
	endCycle(arm);
}
}


void handleArm() //Decide which state to put machine in
{
short joy = gJoy[JOY_ARM].cur;
if (RISING(JOY_ARM))
{
	armStateChange(armManual);
}
else if (!joy && armState == armManual)//|| abs(gJoy[JOY_ARM].cur) < abs(gJoy[JOY_ARM].lst))
{
	armStateChange(armHold, -1, -1);
}

if (RISING(BTN_STACK))
{
	LOG(arm)("Stack button");
	armStateChange(armMove, 1500, 2, velEither, ARM_TOP - 800, 80);
}
}

/* Mobile Goal Controls */
#define MOBILE_TOP 2430
#define MOBILE_BOTTOM 900
#define MOBILE_MIDDLE_UP 1150
#define MOBILE_MIDDLE_DOWN 1650
#define MOBILE_MIDDLE_THRESHOLD 2250
#define MOBILE_HALFWAY 1550

#define MOBILE_UP_HOLD_POWER 12
#define MOBILE_DOWN_HOLD_POWER -15

typedef enum _tMobileStates
{
idle,
hold,
moveTop,
moveBottom,
moveUpToMid,
moveDownToMid,
holdMid
}tMobileStates;

tMobileStates gMobileState = idle;

void setMobile(word val)
{
gMotor[mobile].power = val;
}

task setMobileState()
{
sCycleData mobile;
initCycle(mobile, 10, "mobile");
while (true)
{
	switch (gMobileState)
	{
	case 0: //idle
		{
			setMobile(0);
			break;
		}
	case 1: //hold
		{
			setMobile(gSensor[mobilePoti].value < MOBILE_HALFWAY? -5 : 5);
			break;
		}
	case 2: //moveTop
		{
			setMobile(127);
			if (gSensor[mobilePoti].value > (MOBILE_TOP - 50))
				gMobileState = hold;
			break;
		}
	case 3: //moveBottom
		{
			setMobile(-127);
			if (gSensor[mobilePoti].value < (MOBILE_BOTTOM + 50))
				gMobileState = hold;
			break;
		}
	case 4: //moveUpToMid
		{
			setMobile(127);
			if (gSensor[mobilePoti].value > MOBILE_MIDDLE_UP)
				gMobileState = holdMid;
			break;
		}
	case 5: //moveDownToMid
		{
			setMobile(-127);
			if (gSensor[mobilePoti].value < MOBILE_MIDDLE_DOWN)
				gMobileState = holdMid;
			break;
		}
	case 6: //holdMid
		{
			if (gSensor[mobilePoti].value > MOBILE_MIDDLE_DOWN-100)
				setMobile(0);
			else
				setMobile(15);
			break;
		}
	}
	endCycle(mobile);
}
}

void handleMobile()
{
if (RISING(BTN_MOBILE_TOGGLE))
{
	writeDebugStreamLine("Toggle Mobile");
	if (gMobileState != holdMid)
	{
		gMobileState = (gSensor[mobilePoti].value < MOBILE_HALFWAY)? moveTop : moveBottom;
		writeDebugStreamLine("Mobile state: %d", gMobileState);
	}
	else
	{
		gMobileState = moveTop;
	}
}
if (RISING(BTN_MOBILE_MIDDLE))
{
	if (gMobileState != holdMid)
		gMobileState = (gSensor[mobilePoti].value < MOBILE_HALFWAY)? moveUpToMid : moveDownToMid;
	else
		gMobileState = moveBottom;
}
}

void startTasks()
{
	resetPositionFull(gPosition, 0, 0, 0);

	tStart(driveSet);
	tStart(liftSet);
	tStart(armSet);
	tStart(setMobileState);

}

void stopTasks()
{
	tStop(trackPositionTask);

	tStop(driveSet);
	tStop(liftSet);
	tStop(armSet);
	tStop(setMobileState);
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
enableJoystick(JOY_LIFT);
enableJoystick(JOY_ARM);
enableJoystick(BTN_MOBILE_TOGGLE);
enableJoystick(BTN_MOBILE_MIDDLE);
enableJoystick(BTN_STACK);
enableJoystick(BTN_LIFT_TEST);
enableJoystick(BTN_DRIVE_TEST);

gJoy[JOY_THROTTLE].deadzone = 15;
gJoy[JOY_TURN].deadzone = 15;
gJoy[JOY_LIFT].deadzone = 25;
gJoy[JOY_ARM].deadzone = 25;
}

void disabled()
{
updateSensorInputs();
//selectAuto();
//handleLcd();
}

task autonomous()
{
	sCycleData cycle;
	initCycle(cycle, 10, "autonomous");
	startTasks();

	while(true)
	{
		//updateJoysticks();
		updateMotors();
		updateSensorInputs();
		updateSensorOutputs();
		endCycle(cycle);
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

		handleDrive();
		handleLift();
		handleArm();
		handleMobile();

		//setMobileState();

		endCycle(cycle);
	}
	stopTasks();
}
