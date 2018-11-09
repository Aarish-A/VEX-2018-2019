#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)

task main()
{
	clearDebugStream();
	int target = 1500;
	int fullSpeed = 75;
	float percentFull = 0.75;
	float percentDecel = 0.9;
	float kp = 0.04;
	int gError = target-SensorValue[anglerPoti];
	while(SensorValue[anglerPoti]<(percentFull*target))
	{
		motor[angler] = fullSpeed;
	}
	while(SensorValue[anglerPoti]<(percentDecel*target))
	{
		int error = target-SensorValue[anglerPoti];
		motor[angler] = error*kp;
	}
	while(SensorValue[anglerPoti]<target)
	{
		motor[angler] = -20;
	}

	sleep(100);
	motor[angler] = 15;
	sleep(300);
	writeDebugStreamLine("Intake Podi Value: %d",SensorValue[anglerPoti]);
	while(true)
	{
		sleep(10);
	}
}
