#pragma config(Sensor, in1,    ballDetector,   sensorLineFollower)
#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Motor,  port2,           intake,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           shooterA,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterRevSplit, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)

#define BTN_SHOOT Btn5U
#define BTN_INTAKE_UP Btn6U
#define BTN_INTAKE_DOWN Btn6D

#define JOY_ANGLER Ch2

//#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_VAL (360.0*SHOOTER_GEAR_RATIO)
#define SHOOTER_RELOAD_HOLD 11
#define SHOOTER_RELOAD_POS 145

int gShooterPow;
int shooterShotCount;
task angleChange()
{
	int target = 1500;
float percentFull = 0.85;
float percentDecel = 0.95;
int decelSpeed = 68;
	bool BtnAngleUp, BtnAngleUpLst;
	while(SensorValue[anglerPoti]<(percentFull*target))
	{
		motor[angler] = 127;
		sleep(10);
	}
	while(SensorValue[anglerPoti]<(percentDecel*target))
	{
		int error = target-SensorValue[anglerPoti];
		motor[angler] = error*0.3;
		sleep(10);
	}
	while(SensorValue[anglerPoti]<target)
	{
		motor[angler] = -20;
		sleep(10);
	}

	sleep(100);
	motor[angler] = 15;
	writeDebugStreamLine("Intake Podi Value: %d",Senso	rValue[anglerPoti]);
	sleep(300);
	return;
}
task angleChangeB()
{
	int target = 1000;
float percentFull = 0.85;
float percentDecel = 0.95;
int decelSpeed = 68;
	bool BtnAngleUp, BtnAngleUpLst;
	while(SensorValue[anglerPoti]<(percentFull*target))
	{
		motor[angler] = 127
		sleep(10);
	}
	while(SensorValue[anglerPoti]<(percentDecel*target))
	{
		int error = target-SensorValue[anglerPoti];
		motor[angler] = error*0.3;
		sleep(10);
	}
	while(SensorValue[anglerPoti]<target)
	{
		motor[angler] = -20;
		sleep(10);
	}

	sleep(100);
	motor[angler] = 15;
	writeDebugStreamLine("Intake Podi Value: %d",SensorValue[anglerPoti]);
	sleep(300);
	return;
}
void setShooter(int shootSpeed)
{
	motor[shooterA]=motor[shooterRevSplit]=shootSpeed;
}
void reloadShooter()
{
	setShooter(127);
	int target = shooterShotCount * SHOOTER_RELOAD_VAL;
	unsigned long reloadStartTime = npgmtime;
	while (SensorValue[shooterEnc] < (target + SHOOTER_RELOAD_POS))
	{
		sleep(10);
		//writeDebugStreamLine("Enc: %d", SensorValue[shooterEnc]);
	}
	writeDebugStreamLine("Reload time = %d", npgmtime-reloadStartTime);
	setShooter(SHOOTER_RELOAD_HOLD);
}
task waittoAngle()
{
	sleep(300);
	startTask(angleChange);
}

task shooterTask()
{
	//startTask(printEnc);
	clearDebugStream();
	int nBatteryLevel = nImmediateBatteryLevel;
	writeDebugStream("%d Battery: %d", nPgmTime, nBatteryLevel);
	shooterShotCount = 0;
	SensorValue[shooterEnc]=0;

	writeDebugStreamLine("Start");
	const float breakKp = -25.0;
	float shooterBreakOffset = 6;

	bool shotTargReached = false;

	bool cancelledPrint = false;
	while (true)
	{

		if (vexRt[BTN_SHOOT])//RISING(BTN_SHOOT) )
		{


			int target = shooterShotCount * SHOOTER_RELOAD_VAL;
			if (SensorValue[shooterEnc] < (target+SHOOTER_RELOAD_POS-10)) //Should only get triggered when shooterShotCount == 0
			{
				reloadShooter();
				while (!vexRT[BTN_SHOOT]) sleep(10);
			}

			if(SensorValue[ballDetector] > 1000)
			{
				if (!cancelledPrint)
				{
					writeDebugStreamLine("%d No Ball - Shot Cancelled");
					cancelledPrint = true;
				}
			}
			else if(SensorValue[ballDetector] >= 250 & SensorValue[ballDetector] <= 252)
			{
				if (!cancelledPrint)
				{
					writeDebugStreamLine("%d Ball Detector Unplugged - Shot Cancelled");
					cancelledPrint = true;
				}
			}
			else
			{
				cancelledPrint = false;
				unsigned long shotStartTime = npgmtime;
				writeDebugStreamLine("%d Shot Start", npgmtime);
				shooterShotCount++;
				target = shooterShotCount * SHOOTER_RELOAD_VAL;
				setShooter(127);
				startTask(waittoAngle);
				shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );
				while (!shotTargReached && (SensorValue[shooterEnc] < (target-80) || SensorValue[shooterEnc] > (target-15) || SensorValue[ballDetector] < 1000))
				{
					shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

					unsigned long timeElpsd = npgmtime-shotStartTime;

					sleep(10);
				}
				if (!shotTargReached)
				{
					writeDebugStreamLine("%d Ball gone: Val:%d, Time: %d Pos:%d, Targ:%d ", npgmtime, SensorValue[ballDetector], npgmtime-shotStartTime, SensorValue[shooterEnc], target);
					setShooter(-90);
					sleep(100);
					setShooter(0);
					shooterShotCount--;
					PlayTone(300, 50);
				}
				else
				{
					writeDebugStreamLine("%d Shot done accelerating: Time: %d Pos:%d ", npgmtime, npgmtime-shotStartTime, SensorValue[shooterEnc]);
					setShooter(-22);
					unsigned long startBreakTime = npgmtime;
					while ((npgmtime-startBreakTime) < 80)
					{
						writeDebugStreamLine("%d Break", npgmtime);
						if (SensorValue[ballDetector] > 2000)
						{
							writeDebugStreamLine("	%d Ball hit: Time: %d", npgmtime, npgmtime-shotStartTime);
						}
						sleep(10);
					}
					//sleep(80);
					writeDebugStreamLine("Break done at %d, Enc: %d", nPgmTime, SensorValue[shooterEnc]);
					setShooter(0);
					writeDebugStreamLine("Hold done at %d, Tgt: %d, Enc: %d, Err: %d", nPgmTime, target, SensorValue[shooterEnc], target - SensorValue[shooterEnc]);
					//writeDebugStreamLine("%dStart break %d, %d", npgmtime, shooterShotCount, SensorValue[shooterEnc]);
					//sleep(50);
					reloadShooter();
					writeDebugStreamLine("%d Total Shot Time:%d", npgmtime, npgmtime-shotStartTime);
				}
			}
		}

		sleep(10);
	}
	//stopTask(printEnc);
}
task main()
{
startTask(angleChangeB);
startTask(shooterTask);
while(true){sleep(10);}
stopTask(shooterTask);

}
