#pragma config(Sensor, in1,    ballDetector,         sensorLineFollower)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Motor,  port2,            ,             tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           driveL1,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           driveL2,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooterA,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterRevSplit, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           driveR1,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_VAL (360.0*SHOOTER_GEAR_RATIO)

int gPow;
/*
task printEnc()
{
	while (true)
	{
		datalogDataGroupStart();
		datalogAddValue(1, SensorValue[shooterEnc]);
		datalogAddValue(2, gPow);
		datalogDataGroupEnd();

		sleep(10);
	}
}
*/


void setShooter(word val)
{
	//gMotor[shooterA].power = gMotor[shooterB].power = LIM_TO_VAL(val, 127);
	gPow = LIM_TO_VAL(val, 127);
	motor[shooterA] = motor[shooterRevSplit] = LIM_TO_VAL(val, 127);
}


task main()
{
	//startTask(printEnc);
	clearDebugStream();
	int nBatteryLevel = nImmediateBatteryLevel;
	writeDebugStream("%d Battery: %d", nPgmTime, nBatteryLevel);
	int shooterShotCount = 0;
	SensorValue[shooterEnc] = 0;

	word button, lstButton;
	writeDebugStreamLine("Start");
	const float breakKp = -25.0;
	float shooterBreakOffset = 6;
	while (true)
	{
		button = vexRT[Btn6U];

		if (button && !lstButton)
		{
			setShooter(127);
			writeDebugStreamLine("Setup shot #%d encoder start= %d", shooterShotCount,SensorValue[shooterEnc]);
			int target = shooterShotCount * SHOOTER_RELOAD_VAL;

			while (SensorValue[shooterEnc] < (target + 145)) {
				sleep(100);
				writeDebugStreamLine("Enc: %d", SensorValue[shooterEnc]);
			}
			writeDebugStreamLine("Enc: %d", SensorValue[shooterEnc]);
			setShooter(11);

			while (!vexRT[Btn6U]) sleep(10);

			if( SensorValue[ballDetector] > 1000) {
				setShooter(0);
			}
			else{

				shooterShotCount++;
				target = shooterShotCount * SHOOTER_RELOAD_VAL;
				setShooter(127);
				writeDebugStreamLine("Fired shot #%d at %d, Tgt: %d, Enc: %d, Err: %d", shooterShotCount, nPgmTime, SensorValue[shooterEnc], target, target - SensorValue[shooterEnc]);
				while (SensorValue[shooterEnc] < (target-shooterBreakOffset) &&  SensorValue[ballDetector] < 1000) sleep(10);
				writeDebugStreamLine("Forwards done at %d, Enc: %d", nPgmTime, SensorValue[shooterEnc]);
				setShooter(-22);
				sleep(80);
				writeDebugStreamLine("Break done at %d, Enc: %d", nPgmTime, SensorValue[shooterEnc]);
				setShooter(0);
				writeDebugStreamLine("Hold done at %d, Tgt: %d, Enc: %d, Err: %d", nPgmTime, target, SensorValue[shooterEnc], target - SensorValue[shooterEnc]);
				//writeDebugStreamLine("%dStart break %d, %d", npgmtime, shooterShotCount, SensorValue[shooterEnc]);
			}
		}
		lstButton = button;

		sleep(10);
	}
	//stopTask(printEnc);
}
