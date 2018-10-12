#include "../src/task.h"
//#include "motors.h"
#include "sensors.h"
//#include "joysticks.h"
#include "utilities.h"
#include "auto.h"
#include "cycle.h"

// Year-independent libraries (source)
#include "task.c"
//#include "motors.c"
#include "sensors.c"
//#include "joysticks.c"
#include "utilities.c"
#include "cycle.c"

task main()
{
	setupSensors();
	tInit();

	int sen, lstSen;
	unsigned long time, lstTime, tElpsdTime;
	unsigned long startBreakTime = npgmtime;
	float vel = 0;
	tElpsdTime = 0;

	sCycleData cycle;
	initCycle(cycle, 10, "test");
	for (int i = 0; i < 1000 && tElpsdTime < 700; i+=2)
	{
			time = npgmtime;
			sen = i;

			int dif = (sen-lstSen);
			unsigned long elpsdTime = time - lstTime;
			writeDebugStreamLine("%d time: %d, dif:%d, cur:%d - lst:%d v:%f", npgmtime, elpsdTime, dif, sen, lstSen, dif/elpsdTime);

			if (elpsdTime > 0)
				vel = (float)(dif)/(elpsdTime);
			else
				vel =  0;

			int power = vel * -7;

			lstSen = sen;
			writeDebugStreamLine("%d Break, vel: %f, pow: %d, tEl:%d", npgmtime, vel, power, tElpsdTime);
			lstTime = time;

			endCycle(cycle);

			tElpsdTime = npgmtime - startBreakTime;
	}
}

task updateVals()
{
	//writeDebugStreamLine("%d Start update", npgmtime);
	sCycleData cycle;
	initCycle(cycle, 9, "UpdateVals");
	while (true)
	{
		tHog();
		updateSensorInputs();
		updateSensorOutputs();
		tRelease();
		endCycle(cycle);
	}
}
