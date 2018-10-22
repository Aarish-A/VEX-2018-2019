unsigned char _hogLevel = 0;
void tHog()
{
	if (!_hogLevel++)
		hogCPU();
}

void tRelease()
{
	if (_hogLevel && !--_hogLevel)
		releaseCPU();
}

#include "../src/sensors.h"
#include "../src/sensors.c"

task main()
{
	setupSensors();
	while (true) sleep(10);
}
