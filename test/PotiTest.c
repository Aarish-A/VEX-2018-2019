#pragma config(Sensor, in2,    sensor,         sensorPotentiometer)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{
	while (true)
	{
		//clearLCDLine(0);
		//displayLCDNumber(0, 0, SensorValue[sensor]);
		//datalogDataGroupStart();
		datalogAddValueWithTimeStamp(0, SensorValue[sensor]);
		//datalogDataGroupEnd();
		sleep(10);
	}
}
