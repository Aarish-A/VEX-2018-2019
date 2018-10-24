#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Motor,  port8,           angler,        tmotorVex393HighSpeed_MC29, openLoop, reversed)

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

#define ANGLER_POTI_BOTTOM 125
#define ANGLER_POTI_TOP 3600
#define ANGLER_POS 1900

#define ANGLER_IS_TOP_MID (SensorValue[anglerPoti] > 1370 && SensorValue[anglerPoti] < 2370)
#define TOP_MID_POWER 9

#define ANGLER_IS_HORIZONTAL (SensorValue[anglerPoti] > 970 && SensorValue[anglerPoti] < 1370)
#define HORIZONTAL_POWER 10

#define ANGLER_IS_BOTTOM_MID (SensorValue[anglerPoti] > 670 && SensorValue[anglerPoti] < 970)
#define BOTTOM_MID_POWER 9

void setAngler(word val)
{
	Motor[angler] = LIM_TO_VAL(val, 127);
}

void moveAngler(int target)
{
	if (SensorValue[anglerPoti] < target)
	{
		writeDebugStreamLine("%d Start move from %d to %d", npgmtime, SensorValue[anglerPoti], target);
		unsigned long curTime = npgmtime;
		float kP = 0.063;
		while (SensorValue[anglerPoti] < target)
		{
			int error = (target - SensorValue[anglerPoti]);
			int power = error * kP;
			if (ANGLER_IS_HORIZONTAL && power < HORIZONTAL_POWER) power = HORIZONTAL_POWER;
			else if (ANGLER_IS_BOTTOM_MID && power < BOTTOM_MID_POWER) power = BOTTOM_MID_POWER;
			else if (ANGLER_IS_TOP_MID && power < TOP_MID_POWER) power = TOP_MID_POWER;
			setAngler(power);
			writeDebugStreamLine("%d Loc:%d, Err:%d, Pow:%d", npgmtime, SensorValue[anglerPoti], error, power);
			sleep(10);
		}
		setAngler(0);
		writeDEbugStreamLine("	%d Done move to %d. Sen: %d, Err: %d, Took: %d", npgmtime, target, SensorValue[anglerPoti], target-SensorValue[anglerPoti], (npgmtime-curTime));
		sleep(400);
		writeDEbugStreamLine("	%d After rest. Targ %d. Sen: %d, Err: %d, Took: %d", npgmtime, target, SensorValue[anglerPoti], target-SensorValue[anglerPoti], (npgmtime-curTime));
	}
}

void moveAnglerSpeed(int target)
{
	if (SensorValue[anglerPoti] < target)
	{
		float targVel = 0.15;

		writeDebugStreamLine("%d Start move from %d to %d", npgmtime, SensorValue[anglerPoti], target);
		unsigned long curTime = npgmtime;
		float kP = 15.0;

		int sen = 0;
		int lstSen = sen;
		int time = npgmtime;
		int lstTime = time;
		float vel = 0;
		while (SensorValue[anglerPoti] < target)
		{
			sen = SensorValue[anglerPoti];
			time = npgmtime;
			if ((time-lstTime) > 0)
				vel = (float)(sen-lstSen) / (float)(time-lstTime);
			else
				writeDEbugStreamLine("	%d Vel Time Calc Error. T:%d lstT: %d", npgmtime, time, lstTime);

			float speedBoost = (targVel - vel) * kP;
			int power = 11 + speedBoost;
			setAngler(power);
			writeDebugStreamLine("%d Loc:%d, Pow:%d, vel: %f, speedBoost: %f", npgmtime, SensorValue[anglerPoti], power, vel, speedBoost);

			lstSen = sen;
			lstTime = time;
			sleep(30);
		}
		setAngler(0);
		writeDEbugStreamLine("	%d Done move to %d. Sen: %d, Err: %d, Took: %d", npgmtime, target, SensorValue[anglerPoti], target-SensorValue[anglerPoti], (npgmtime-curTime));
		sleep(400);
		writeDEbugStreamLine("	%d After rest. Targ %d. Sen: %d, Err: %d, Took: %d", npgmtime, target, SensorValue[anglerPoti], target-SensorValue[anglerPoti], (npgmtime-curTime));
	}
}

task main()
{
	clearDebugStream();
	bool bttn, lstBttn;
	int sen = 0;
	int lstSen = sen;
	int time = npgmtime;
	int lstTime = time;
	float vel = 0;
	while (true)
	{
		bttn = vexRT[Btn5D];

		sen = SensorValue[anglerPoti];
		time = npgmtime;
		if ((time-lstTime) != 0)
		{
			vel = (float)(sen-lstSen) / (float)(time-lstTime);
			//datalogDataGroupStart();
			//datalogAddValue(1, (vel*1000));
			//datalogDataGroupEnd();
			//writeDebugStreamLine("%f", vel);
		}
		if (bttn && !lstBttn)
		{
			//moveAnglerSpeed(SensorValue[anglerPoti]+300);
			//setAngler(11);
			////while(SensorValue[anglerPoti] < 3300);
			////setAngler(0);

			int startVal = SensorValue[anglerPoti];
			writeDEbugStreamLine("%d Start move Targ: %d", npgmtime, SensorValue[anglerPoti], (startVal-400));
			setAngler(-20);
			while (SensorValue[anglerPoti] > (startVal - 400)) sleep(10);
			writeDEbugStreamLine("%d After move val: %d. Targ: %d", npgmtime, SensorValue[anglerPoti], (startVal-400));
			setAngler(7);
			sleep(500);
			writeDEbugStreamLine("%d After break val: %d. Targ: %d", npgmtime, SensorValue[anglerPoti], (startVal-400));
		}
		else if (abs(vexRt[Ch2]) > 20)
		{
			if (vexRT[Ch2] > 0 && SensorValue[anglerPoti] < ANGLER_POTI_TOP) motor[angler] = vexRT[Ch2];
			else if (vexRT[Ch2] < 0 && SensorValue[anglerPoti] > ANGLER_POTI_BOTTOM) motor[angler] = vexRT[Ch2];
		}
		else
		{
			if (SensorValue[anglerPoti] > 2600 &&	SensorValue[anglerPoti] < 3500) setAngler(7);
			//else if (SensorValue[anglerPoti] > 1080 && SensorValue[anglerPoti] < 1500) motor[angler] = 7;//8;
			//else if (SensorValue[anglerPoti] > 1000 && SensorValue[anglerPoti] < 1080) motor[angler] = 7;//9;
			else if (SensorValue[anglerPoti] < 400) setAngler(-6);
			else setAngler(0);
			//if(SensorValue[anglerPoti] < 800) motor[angler] = -5;
			//else if (SensorValue[anglerPoti] > 800 && SensorValue[anglerPoti] < 1000) motor[angler] = -10;
			//else if (SensorValue[anglerPoti] > 1000 && SensorValue[anglerPoti] < 1500) motor[angler] = 0;
			//else if (SensorValue[anglerPoti] > 1500 && SensorValue[anglerPoti] < 2250) motor[angler] = 8;
			//else if (SensorValue[anglerPoti] > 2250 && SensorValue[anglerPoti] < 2600) motor[angler] = 10;
			//else if (SensorValue[anglerPoti] > 2600 &&	SensorValue[anglerPoti] < 3500) motor[angler] = 5;
		}

		lstSen = sen;
		lstTime = time;

		lstBttn = bttn;
		sleep(20);
	}



}
