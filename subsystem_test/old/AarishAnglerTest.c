#pragma config(Sensor, in1,    anglerPoti,     sensorPotentiometer)
#pragma config(Motor,  port2,           angler,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define ANGLER_POTI_BOTTOM 125
#define ANGLER_POTI_TOP 3600
#define ANGLER_POS 1900


task main()
{
	while(true)
	{
		if (abs(vexRt[Ch2]) > 20)
		{
			if (vexRT[Ch2] > 0 && SensorValue[anglerPoti] < ANGLER_POTI_TOP) motor[angler] = vexRT[Ch2];
			else if (vexRT[Ch2] < 0 && SensorValue[anglerPoti] > ANGLER_POTI_BOTTOM) motor[angler] = vexRT[Ch2];
		}
		else if(vexRT[Btn5D])
		{
			unsigned long startTime = nPgmTime;
			int target = ANGLER_POS;

			if (SensorValue[anglerPoti] < target)
			{
				motor[angler] = 127;
				int potiOffset = 275;
				float kP = 0.5;
				int error = target - SensorValue[anglerPoti] - potiOffset;

				while(error > 150)
				{
					if (SensorValue[anglerPoti] > ANGLER_POTI_TOP) break;
					error = target - SensorValue[anglerPoti] - potiOffset;
					sleep(10);
				}
				while (error > 0)
				{
					if (SensorValue[anglerPoti] > ANGLER_POTI_TOP) break;
					motor[angler] = error * kP;
					error = target - SensorValue[anglerPoti] - potiOffset;
					sleep(10);
				}
				motor[angler] = -20;
				sleep(75);
			}
			else if (SensorValue[anglerPoti] > target)
			{
				motor[angler] = -127;
				int potiOffset = -275;
				float kP = 0.5;
				int error = target - SensorValue[anglerPoti] - potiOffset;

				while(error < -150)
				{
					if (SensorValue[anglerPoti] < ANGLER_POTI_BOTTOM) break;
					error = target - SensorValue[anglerPoti] - potiOffset;
					sleep(10);
				}
				while (error < 0)
				{
					if (SensorValue[anglerPoti] < ANGLER_POTI_BOTTOM) break;
					motor[angler] = error * kP;
					error = target - SensorValue[anglerPoti] - potiOffset;
					sleep(10);
				}
				motor[angler] = 20;
				sleep(100);
			}
			motor[angler] = 5;
			sleep(200);
			unsigned long timeDiff = nPgmTime - startTime;
			writeDebugStreamLine("Target: %d, Poti: %d, Err: %d, Time: %d.%d seconds", target, SensorValue[anglerPoti], target - SensorValue[anglerPoti], timeDiff / 1000, timeDiff % 1000);
		}
		else
		{
			if(SensorValue[anglerPoti] < 800) motor[angler] = -5;
			else if (SensorValue[anglerPoti] > 800 && SensorValue[anglerPoti] < 1000) motor[angler] = -10;
			else if (SensorValue[anglerPoti] > 1000 && SensorValue[anglerPoti] < 1500) motor[angler] = 0;
			else if (SensorValue[anglerPoti] > 1500 && SensorValue[anglerPoti] < 2250) motor[angler] = 8;
			else if (SensorValue[anglerPoti] > 2250 && SensorValue[anglerPoti] < 2600) motor[angler] = 10;
			else if (SensorValue[anglerPoti] > 2600 &&	SensorValue[anglerPoti] < 3500) motor[angler] = 5;
			//if (SensorValue[anglerPoti] < 500) motor[angler] = 12;
			//else motor[angler] = -12;
		}

		//writeDebugStreamLine("Angler Poti is at %d", SensorValue[anglerPoti]);

		sleep(10);
	}
}
