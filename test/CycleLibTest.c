#include "../src/cycle.h"
#include "cycle.c"

task main()
{
	sCycleData cycleA;
	initCycle(cycleA, 10, "cycleAName");
	while(true)
	{
		sleep(1000);
		endCycle(cycleA);
	}

}
