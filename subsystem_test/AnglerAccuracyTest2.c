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
	int error = target - SensorValue[anglerPoti];
	unsigned long startTime = nPgmTime;

	datalogDataGroupStart();

	//while(error > -50)
	//{
	//	setAngler(127);
	//	error = target - SensorValue[anglerPoti];
	//	writeDebugStreamLine("Poti: %d, Error: %d", SensorValue[anglerPoti], error);
	//	sleep(10);
	//}
	//setAngler(-10);
	//sleep(50);
	//writeDebugStreamLine("%d | Ended breaking at %d, Error: %d, Took %d.%d seconds", nPgmTime, SensorValue[anglerPoti], error, (nPgmTime - startTime) / 1000, (nPgmTime - startTime) % 1000);
	//error = target - SensorValue[anglerPoti];

	float kP = 0.14;
	//float kI = 0.005;
	//float kD = 0.6;
	float kI = 0.000;
	float kD = 0.0;

	unsigned long lastTime = nPgmTime;
	int lastError = error;

	float pValue = 0;
	float iValue = 0;
	float dValue = 0;

	unsigned long continueLoop = 0;
	bool cross = false;
	bool lastCross = false;

	while((nPgmTime - startTime) < 1500 && continueLoop < 100)
	//while((nPgmTime - startTime) < 600);
	{
		error = target - SensorValue[anglerPoti];
		float deltaTime = (float)(nPgmTime - lastTime);
		float deltaError = (float)(error - lastError);

		pValue = error;
		iValue = iValue + ((float)error * deltaTime);
		if (deltaTime > 0) dValue = (float)(deltaError / deltaTime);

		int power;
		//if (error < 0) power = (pValue * kP) + (iValue * kI) + (dValue * kD);
		//else if (error > 0)
		//{
		//	power = deltaError * 0.15;
		//}
		float powerFloat = pValue * kP;
		power = (pValue * kP) + (iValue * kI) + (dValue * kD);
		if (error > 0)
		{
			continueLoop += 10;
			cross = true;
			kP = 0.3;
		}
		if (cross && !lastCross) iValue = 0;


		setAngler(power);
		//writeDebugStreamLine("Poti: %d, Error: %d, Power: %d, pVal: %f, iVal: %f, dVal: %f", SensorValue[anglerPoti], error, power, pValue, iValue, dValue);
		writeDebugStreamLine("%d | Poti: %d, Error: %d, Power: %d, PowerF: %f, Continue Loop: %d, pVal: %f, iVal: %f", nPgmTime, SensorValue[anglerPoti], error, motor[angler], powerFloat, continueLoop, pValue * kP, iValue * kI);

		lastError = error;
		lastTime = nPgmTime;
		lastCross = cross;
		sleep(10);
	}
	writeDebugStreamLine("%d | Finished PID-Loop at %d, Error: %d, Took %d.%d seconds", nPgmTime, SensorValue[anglerPoti], error, (nPgmTime - startTime) / 1000, (nPgmTime - startTime) % 1000);
	writeDebugStreamLine("%d | Poti: %d, Power: %d", nPgmTime, SensorValue[anglerPoti], motor[angler]);
	//sleep(200);
	setAngler(-8);

	unsigned long loopTimer = nPgmTime;
	while((nPgmTime - loopTimer) < 300)
	{
			writeDebugStreamLine("%d | Poti: %d, Error: %d, Power: %d", nPgmTime, SensorValue[anglerPoti], target - SensorValue[anglerPoti], motor[angler]);
			sleep(10);
	}
	error = target - SensorValue[anglerPoti];
	//writeDebugStreamLine("%d | Finished rest at %d, Error: %d, Took %d.%d seconds", nPgmTime, SensorValue[anglerPoti], error, (nPgmTime - startTime) / 1000, (nPgmTime - startTime) % 1000);
}

task main()
{
	clearDebugStream();

	bool curButton, lastButton;
	writeDebugStreamLine("Poti Position: %d", SensorValue[anglerPoti]);

	while(true)
	{
		curButton = vexRT[Btn6U];

		//writeDebugStreamLine("press button");
		//while(!vexRT[Btn6U]) sleep(10);
		//moveAngler(SensorValue[anglerPoti] - 100);

		writeDebugStreamLine("press button");
		while(!vexRT[Btn6U]) sleep(10);
		moveAngler(SensorValue[anglerPoti] - 300);

		//writeDebugStreamLine("press button");
		//while(!vexRT[Btn6U]) sleep(10);
		//moveAngler(SensorValue[anglerPoti] - 300);

		lastButton = curButton;
		sleep(10);
	}
}
