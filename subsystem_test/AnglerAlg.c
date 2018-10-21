#pragma config(Sensor, in1,    anglerPoti,     sensorPotentiometer)
#pragma config(Motor,  port2,           angler,        tmotorVex393HighSpeed_MC29, openLoop, reversed)

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

#define ANGLER_POTI_BOTTOM 125
#define ANGLER_POTI_TOP 3600
#define ANGLER_POS 1900

void setAngler(word val)
{
	Motor[angler] = LIM_TO_VAL(val, 127);
}

void moveAngler(int target)
{
	if (SensorValue[anglerPoti] < target)
	{
		unsigned long curTime = npgmtime;
		int error = 0;
		int lstError = 0;
		unsigned long time = npgmtime;
		unsigned long lstTime = 0;
		float kP = 0.09;//0.085;
		float kD = 5.5;
		while (SensorValue[anglerPoti] < (target-10) && (npgmtime-curTime) < 1500)
		{
			time = npgmtime;
			error = (target - SensorValue[anglerPoti]);
			float pVal = error * kP;
			float dVal = (((float)(error-lstError)/(float)(time-lstTime)) * kD);
			float power = pVal + dVal;
			setAngler(power);
			writeDEbugStreamLine("%d error:%d, pVal:%f, dVal:%f, pow: %f", npgmtime, error, pVal, dVal, power);

			lstError = error;
			lstTime = time;
			sleep(10);
		}
		setAngler(6);
		writeDEbugStreamLine("	%d Done move to %d. Sen: %d, Err: %d, Took: %d", npgmtime, target, SensorValue[anglerPoti], target-SensorValue[anglerPoti], (npgmtime-curTime));
		sleep(400);
		writeDEbugStreamLine("	%d After rest. Targ %d. Sen: %d, Err: %d, Took: %d", npgmtime, target, SensorValue[anglerPoti], target-SensorValue[anglerPoti], (npgmtime-curTime));
	}
}

task main()
{
	bool bttn, lstBttn;
	while (true)
	{
		bttn = vexRT[Btn5D];

		if (bttn && !lstBttn)
		{
			moveAngler(1500);
		}
		else if (abs(vexRt[Ch2]) > 20)
		{
			if (vexRT[Ch2] > 0 && SensorValue[anglerPoti] < ANGLER_POTI_TOP) motor[angler] = vexRT[Ch2];
			else if (vexRT[Ch2] < 0 && SensorValue[anglerPoti] > ANGLER_POTI_BOTTOM) motor[angler] = vexRT[Ch2];
		}
		else
		{
			if(SensorValue[anglerPoti] < 800) motor[angler] = -5;
			else if (SensorValue[anglerPoti] > 800 && SensorValue[anglerPoti] < 1000) motor[angler] = -10;
			else if (SensorValue[anglerPoti] > 1000 && SensorValue[anglerPoti] < 1500) motor[angler] = 0;
			else if (SensorValue[anglerPoti] > 1500 && SensorValue[anglerPoti] < 2250) motor[angler] = 8;
			else if (SensorValue[anglerPoti] > 2250 && SensorValue[anglerPoti] < 2600) motor[angler] = 10;
			else if (SensorValue[anglerPoti] > 2600 &&	SensorValue[anglerPoti] < 3500) motor[angler] = 5;
		}

		lstBttn = bttn;
		sleep(10);
	}



}
