#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Motor,  port8,           angler,        tmotorVex393HighSpeed_MC29, openLoop, reversed)

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

#define ANGLER_POTI_BOTTOM 125
#define ANGLER_POTI_TOP 3600
#define ANGLER_POS 1000

//1200-2000 undershoots by 80
//1650-1750 is 60 undershoot

//Shooting form back positions for angler
#define TOP_FLAG_POS 1340
#define MID_FLAG_POS 1090

void setAngler(word val)
{
	Motor[angler] = LIM_TO_VAL(val, 127);
}

task main()
{
	int target = SensorValue[anglerPoti] - 300; //TOP_FLAG_POS;//MID_FLAG_POS;//=
	float kP = 0.095;//0.09;
	float kI = 0.02;

	//setAngler(127);
	//while (SensorValue[anglerPoti] < (target-300)) sleep(10);
	//setAngler(0);
	writeDebugStreamLine("%d Done first Move at %d", npgmtime, SensorVAlue[anglerPoti]);

	unsigned long time = nPgmTime;
	unsigned long timeLst = 0;
	unsigned long deltaTime = 0;
	unsigned long sen = 0;
	unsigned long senLst = 0;
	unsigned long deltaSen = 0;

	float iVal, pVal;
	while (true)
	{
		time = nPgmTime;
		sen = SensorValue[anglerPoti];

		int error = target - SensorValue[anglerPoti];
		float pVal = error * kP;

		deltaTime = time-timeLst;
		deltaSen = sen - senLst;
		if (deltaTime <= 0 || abs(deltaSen) > 5 || abs(error) < 35) iVal = 0;
		else iVal += ( (float)error / (float)(deltaTime) ) * kI;

		int power = pVal + iVal;

		setAngler(power);
		writeDebugStreamLine("%d Sen:%d, Err: %d, pVal:%f, iVal:%f, pow: %f, dT:%d, dS:%d", npgmtime, SensorValue[anglerPoti], error, pVal, iVal, power, deltaTime, deltaSen);

		senLst = sen;
		timeLst = time;
		sleep(10);
	}

}
