#pragma config(Motor,  port5,           shooterA,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterRevSplit, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Sensor, in1,    ballDetector,   sensorLineFollower)

int shotCount=0;
bool gExitShot = false;
#define RELOAD_POS 145
#define SHOOTER_HOLDING 11
void setShooter(int shootSpeed)
{
	motor[shooterA]=motor[shooterRevSplit]=shootSpeed;
}
void reloadShooter()
{
	setShooter(127);
	int target = shotCount * 360;
	while(SensorValue[shooterEnc]<(target+RELOAD_POS))
	{
		sleep(10);
	}
	setShooter(SHOOTER_HOLDING);
}
task main()
{
	bool toggleBtn, toggleBtnLst;

	int shotTarget;
	SensorValue[shooterEnc]=0;
	while(true)
	{
		toggleBtn = vexRT[Btn6U];
		if(toggleBtn && !toggleBtnLst)
		{
			if(SensorValue[shooterEnc]<(shotCount*360+RELOAD_POS))
			{
				reloadShooter();
			}
			while(SensorValue[ballDetector]>1000){sleep(10);}
			shotCount++;
			shotTarget = (shotCount*360)-10;
			setShooter(127);
			while(SensorValue[shooterEnc]<shotTarget & gExitShot == false)
			{
				if(SensorValue[ballDetector]>1000)
				{
					shotCount--;
					setShooter(-90);
					int CurDeg = SensorValue[shooterEnc];
					while(SensorValue[shooterEnc]<(CurDeg-65)){sleep(10);}
					gExitShot = true;
				}
				sleep(10);
			}
			setShooter(SHOOTER_HOLDING);
		}
		toggleBtnLst = toggleBtn;
	}
}
