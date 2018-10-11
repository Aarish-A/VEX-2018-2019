#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)

task main()
{
	clearDebugStream();
int target = 1500;
float percentFull = 0.85;
float percentDecel = 0.95;
int decelSpeed = 68;
	bool BtnAngleUp, BtnAngleUpLst;
	while(SensorValue[anglerPoti]<(percentFull*target))
	{
		motor[angler] = 127
	}
	while(SensorValue[anglerPoti]<(percentDecel*target))
	{
		int error = target-SensorValue[anglerPoti];
		motor[angler] = error*0.3;
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
