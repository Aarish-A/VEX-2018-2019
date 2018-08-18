#pragma config(Sensor, dgtl2,  enc,            sensorQuadEncoder)
#pragma config(Motor,  port2,           mtr,           tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#define GEAR_RATIO (5/3)
#define RELOAD_VAL 360*GEAR_RATIO

task main()
{
	sensorValue[enc] = 0;
	long startVal = sensorValue[enc];
	int shotCount = 0;

	bool lstButton;
	writeDebugStreamLine("start");
	while (true)
	{

		if (vexRT[btn6u] && !lstButton)
		{
			writeDebugStreamLine("%d button push", npgmtime);
			motor[mtr] = 127;
			shotCount++;

			while( (SensorValue[enc] - startVal) < (shotCount*RELOAD_VAL) )
			{
				sleep(50);
				writeDebugStreamLine("%d Sensor:%d", npgmtime, SensorValue[enc])
			}

			motor[mtr] = 0;
		}

		lstButton = vexRT[btn6u];
		sleep(10);
	}
}
