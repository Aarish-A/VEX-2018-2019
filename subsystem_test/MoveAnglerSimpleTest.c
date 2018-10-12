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
#pragma config(Motor,  port7,           driveR,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

void setAngler(word val)
{
	motor[angler] = LIM_TO_VAL(val, 127);
}

void moveAnglerSimple(int target)
{
	float percentFull = 0.85;
	float percentDecel = 0.95;
	int decelSpeed = 68;
	bool BtnAngleUp, BtnAngleUpLst;

	setAngler(127);
	while(SensorValue[anglerPoti]<(percentFull*target))
	{
		sleep(10);
	}

	while(SensorValue[anglerPoti]<(percentDecel*target))
	{
		int error = target-SensorValue[anglerPoti];
		setAngler(error*0.3);
		sleep(10);
	}

	setAngler(-20);
	while(SensorValue[anglerPoti]<target)
	{
		sleep(10);
	}

	sleep(100);
	motor[angler] = 15;
	//sleep(300);
	writeDebugStreamLine("Intake Podi Value: %d",SensorValue[anglerPoti]);
}

task main()
{
	bool bttn, bttnLst;
	while (true)
	{
		bttn = vexRT[Btn5D];

		if (bttn && !bttnLst)
		{
			moveAnglerSimple(1400);
		}
		else
		{
			motor[angler] = 9;
		}
		sleep(10);
		bttnLst = bttn;
	}
}
