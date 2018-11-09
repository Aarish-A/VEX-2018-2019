#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Motor,  port8,           angler,        tmotorVex393HighSpeed_MC29, openLoop, reversed)

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

#define ANGLER_POTI_BOTTOM 125
#define ANGLER_POTI_TOP 3600
#define ANGLER_POS 1000

void setAngler(word val)
{
	motor[angler] = LIM_TO_VAL(val, 127);
}

void moveAngler(int target)
{
	writeDebugStreamLine("%d | Started moving to %d from %d", nPgmTime, target, SensorValue[anglerPoti]);
	unsigned long startTime = nPgmTime;

	int error = target - SensorValue[anglerPoti];
	int lastError = error;
	unsigned long lastTime = nPgmTime;
	float deltaTime;
	float deltaError;

	float kP = 0.14;
	float kI = 0.00;
	float kD = 0.00;

	float pValue = 0;
	float iValue = 0;
	float dValue = 0;

	int power = 0;

	unsigned long continueLoop = 0;
	bool cross = false;
	bool lastCross = false;

	while((nPgmTime - startTime) < 1000 && continueLoop < 100)
	{
		error = target - SensorValue[anglerPoti];
		deltaTime = (float)(nPgmTime - lastTime);
		deltaError = (float)(error - lastError);

		pValue = error;
		iValue = iValue + ((float)error * deltaTime);
		if (deltaTime > 0) dValue = (float)(deltaError / deltaTime);

		/* PID until target is passed, then break based on change in error (speed) */
		//if (error < 0) power = (pValue * kP) + (iValue * kI) + (dValue * kD);
		//else if (error > 0)
		//{
		//	power = deltaError * 0.15;
		//}

		/* If the angler passes the target, I is reset. Continue loop for 100ms after target is passed */
		if (error < 0)
		{
			cross = false;
		}
		else if (error > 0)
		{
			continueLoop += 10;
			cross = true;
			kP = 0.3;
		}
		if ((cross && !lastCross) || (!cross && lastCross)) iValue = 0;

		power = (pValue * kP) + (iValue * kI) + (dValue * kD);
		setAngler(power);

		writeDebugStreamLine("%d | Poti: %d, Error: %d, Power: %d, pVal: %f, iVal: %f", nPgmTime, SensorValue[anglerPoti], error, motor[angler], pValue * kP, iValue * kI);

		lastError = error;
		lastTime = nPgmTime;
		lastCross = cross;
		sleep(10);
	}
	writeDebugStreamLine("%d | Finished PID-Loop at %d, Error: %d, Power: %d, Took %d.%d seconds", nPgmTime, SensorValue[anglerPoti], error, motor[angler], (nPgmTime - startTime) / 1000, (nPgmTime - startTime) % 1000);
	writeDebugStreamLine("Started holding");
	setAngler(-8);

	/* See what the error does after holding is applied, angler should go to rest in this time */
	unsigned long loopTimer = nPgmTime;
	while((nPgmTime - loopTimer) < 300)
	{
			writeDebugStreamLine("%d | Poti: %d, Error: %d, Power: %d", nPgmTime, SensorValue[anglerPoti], target - SensorValue[anglerPoti], motor[angler]);
			sleep(10);
	}
}

task main()
{
	clearDebugStream();
	writeDebugStreamLine("Poti Position: %d", SensorValue[anglerPoti]);

	while(true)
	{
		writeDebugStreamLine("Press Button");

		/* Move angler to 300 ticks below current position */
		while(!vexRT[Btn6U]) sleep(10);
		moveAngler(SensorValue[anglerPoti] - 300);

		sleep(10);
	}
}
