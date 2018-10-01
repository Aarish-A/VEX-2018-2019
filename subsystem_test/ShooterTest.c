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
#include "../src/task.h"
#include "motors.h"
#include "sensors.h"
#include "joysticks.h"
#include "utilities.h"
#include "auto.h"

// Year-independent libraries (source)
#include "task.c"
#include "motors.c"
#include "sensors.c"
#include "joysticks.c"
#include "utilities.c"

#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_VAL (360.0*SHOOTER_GEAR_RATIO)
#define BTN_SHOOT Btn6U

void setShooter(word val)
{
	gMotor[shooterA].power = gMotor[shooterB].power = LIM_TO_VAL(val, 127);
	//motor[shooterA] = motor[shooterB] = LIM_TO_VAL(val, 127);
}

task updateVals()
{
	writeDebugStreamLine("%d Start update", npgmtime);
	while (true)
	{
		hogCPU();
		updateJoysticks();
		updateMotors();
		updateSensorInputs();
		updateSensorOutputs();
		//writeDebugStreamLine("Update-shooter:%d", gSensor[shooterEnc].value);
		releaseCPU();
		sleep(10);
	}
}

task main()
{
	clearDebugStream();
	setupSensors();
	setupMotors();
	setupJoysticks();
	tInit();

	//Setup Joysticks & Buttons
	enableJoystick(BTN_SHOOT);

	resetQuadratureEncoder(shooterEnc);
	startTask(updateVals);
	int shooterShotCount = 0;

	//gSensor[shooterEnc].value = 0;

	bool button, lstButton;
	writeDebugStreamLine("Start");
	const float breakKp = -20.0;
	float shooterBreakOffset = 60.0;
	while (true)
	{
		//button = vexRT[Btn6U];

		//if (button && !lstButton)
		if (RISING(BTN_SHOOT))
		{
			shooterShotCount++;
			int target = shooterShotCount * SHOOTER_RELOAD_VAL;

			setShooter(127);
			writeDebugStreamLine("%dStart shot %d, %d t:%d", npgmtime, shooterShotCount, gSensor[shooterEnc].value, target);
			while (gSensor[shooterEnc].value < (target-shooterBreakOffset)) sleep(10);
			//setShooter(0);
			writeDebugStreamLine("%dStart break %d, %d", npgmtime, shooterShotCount, gSensor[shooterEnc].value);

			do //Break
			{
				velocityCheck(shooterEnc, 2);
				float value = gSensor[shooterEnc].value;
				float velocity = gSensor[shooterEnc].velocity;

				int power = breakKp * abs(velocity);
				setShooter(power);
				writeDebugStreamLine("%d loc:%d vel:%f, pow:%f", npgmtime, value, velocity, power);

				sleep(10);
			} while(gSensor[shooterEnc].velocity > 0.5); //gSensor[shooterEnc].value < target &&

			setShooter(0);
			writeDebugStreamLine("%dEnd %d(t:%d), %d", nPgmTime, shooterShotCount, target, gSensor[shooterEnc].value);
			sleep(30);
			//writeDebugStreamLine("%After 30ms coast %d(t:%d), %d", nPgmTime, shooterShotCount, target, gSensor[shooterEnc].value);
		}

		//lstButton = button;
		sleep(10);
	}

	stopTask(updateVals);
}