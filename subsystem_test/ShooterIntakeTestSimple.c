#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Motor,  port4,           intake,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           shooterA,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           shooterB,      tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_VAL (360.0*SHOOTER_GEAR_RATIO)

int gPow;

typedef enum _tSpinDir
{
	spinStop,
	spinUp,
	spinDown
} tSpinDir;

void setShooter(word val)
{
	//gMotor[shooterA].power = gMotor[shooterB].power = LIM_TO_VAL(val, 127);
	gPow = LIM_TO_VAL(val, 127);
	motor[shooterA] = motor[shooterB] = LIM_TO_VAL(val, 127);
}

task rollers()
{
	bool buttonUp, buttonUpLast, buttonDown, buttonDownLast;

	tSpinDir spinDir = spinStop;
	writeDebugStreamLine("Started intake task");

	while(true)
	{
		buttonUp = vexRT[Btn5U];
		buttonDown = vexRT[Btn5D];

		if (buttonUp && !buttonUpLast)
		{
			if (spinDir == spinUp){
				spinDir = spinStop;
				writeDebugStreamLine("stopUp");
			}
			else {
				spinDir = spinUp;
				writeDebugStreamLine("up");
			}
		}

		if (buttonDown && !buttonDownLast)
		{
			if (spinDir == spinDown) {
				spinDir = spinStop;
			 writeDebugStreamLine("stopDown");
			}
			else {
				spinDir = spinDown;
				writeDebugStreamLine("Down");
			}
		}

		switch(spinDir)
		{
		case spinStop:
			motor[intake] = 0;
			break;
		case spinUp:
			motor[intake] = 127;
			break;
		case spinDown:
			motor[intake] = -127;
			break;
		}

		buttonUpLast = buttonUp;
		buttonDownLast = buttonDown;
		sleep(10);
	}
}

task main()
{
	//startTask(printEnc);
	clearDebugStream();
	int nBatteryLevel = nImmediateBatteryLevel;
	writeDebugStream("Battery: %d", nBatteryLevel);
	writeDebugStreamLine("Start");
	startTask(rollers);

	int shooterShotCount = 0;
	SensorValue[shooterEnc] = 0;

	bool buttonShoot, buttonShootLast;

	const float breakKp = -25.0;
	float shooterBreakOffset = 6;

	while (true)
	{
		buttonShoot = vexRT[Btn6U];

		/* Shooter */
		if (buttonShoot && !buttonShootLast)
		{
			writeDebugStreamLine("Setup shot #%d", shooterShotCount);
			setShooter(127);
			while (SensorValue[shooterEnc] < (360 * shooterShotCount + 100)) sleep(10);
			setShooter(15);
			while(!vexRT[Btn6U]) sleep(10);
			shooterShotCount++;
			int target = shooterShotCount * SHOOTER_RELOAD_VAL;
			setShooter(127);
			writeDebugStreamLine("Fired shot #%d at %d, Tgt: %d, Enc: %d, Err: %d", shooterShotCount, nPgmTime, SensorValue[shooterEnc], target, target - SensorValue[shooterEnc]);
			while (SensorValue[shooterEnc] < (target-shooterBreakOffset)) sleep(10);
			writeDebugStreamLine("Forwards done at %d, Enc: %d", npgmtime, SensorValue[shooterEnc]);
			setShooter(-20);
			sleep(80);
			writeDebugStreamLine("Break done at %d, Enc: %d", nPgmTime, SensorValue[shooterEnc]);
			setShooter(0);
			writeDebugStreamLine("Hold done at %d, Tgt: %d, Enc: %d, Err: %d", nPgmTime, target, SensorValue[shooterEnc], target - SensorValue[shooterEnc]);
			writeDebugStreamLine(" ");

			/*
			setShooter(127);
			writeDebugStreamLine("Setup shot #%d", shooterShotCount);
			while (SensorValue[shooterEnc] < ((360 * shooterShotCount) + 90)) sleep(10);
			writeDebugStreamLine("Enc: %d", SensorValue[shooterEnc]);
			setShooter(13);

			while (!vexRT[Btn6U]) sleep(10);

			shooterShotCount++;
			int target = shooterShotCount * SHOOTER_RELOAD_VAL;
			setShooter(127);
			writeDebugStreamLine("Fired shot #%d at %d, Tgt: %d, Enc: %d, Err: %d", shooterShotCount, nPgmTime, SensorValue[shooterEnc], target, target - SensorValue[shooterEnc]);
			while (SensorValue[shooterEnc] < (target-shooterBreakOffset)) sleep(10);
			writeDebugStreamLine("Forwards done at %d, Enc: %d", npgmtime, SensorValue[shooterEnc]);
			setShooter(-20);
			sleep(80);
			writeDebugStreamLine("Break done at %d, Enc: %d", nPgmTime, SensorValue[shooterEnc]);
			setShooter(0);
			writeDebugStreamLine("Hold done at %d, Tgt: %d, Enc: %d, Err: %d", nPgmTime, target, SensorValue[shooterEnc], target - SensorValue[shooterEnc]);
			writeDebugStreamLine(" ");
			//writeDebugStreamLine("%dStart break %d, %d", npgmtime, shooterShotCount, SensorValue[shooterEnc]);
			*/
		}

		buttonShootLast = buttonShoot;

		sleep(10);
	}
	//stopTask(printEnc);
}
