#pragma config(Sensor, in1,    ballDetector,   sensorLineFollower)
#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Motor,  port2,           driveLY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           intake,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           driveL,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooter,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterY,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           driveR,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

/* Shooter Defines */
#define BTN_SHOOT Btn5U
#define BTN_INTAKE_UP Btn6U
#define BTN_INTAKE_DOWN Btn6D

#define JOY_ANGLER Ch2
#define BTN_ANGLER_TEST Btn5D

#define JOY_THROTTLE Ch3
#define JOY_TURN Ch4

#define RESET_OFFSET 45 //35
#define SHOOTER_GEAR_RATIO 1.0
//#define SHOOTER_RELOAD_VAL ((gShooterShotCount*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_RELOAD_HOLD 11
#define SHOOTER_SHOOT_POS ((gShooterShotCount*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_NEXT_SHOOT_POS (((1+gShooterShotCount)*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_RELOAD_POS ((SHOOTER_SHOOT_POS) + 145)

#define BD_UNPLUGGED (SensorValue[ballDetector] >= 244 && SensorValue[ballDetector] <= 252)

#define BALL_DETECTED (SensorValue[ballDetector] < 1000 && (!BD_UNPLUGGED))

/* Angler Defines */
#define ANGLER_POTI_BOTTOM 125
#define ANGLER_POTI_TOP 3600


unsigned long curTimeS, timeOutS, accelTimeS;
int moveAmntS;

#define WHILE_SHOOTER_MOVING(moveAmt, accelTime, TO, condition) \
curTimeS = npgmtime; \
timeOutS = TO + curTimeS; \
accelTimeS = curTimeS + accelTime; \
moveAmntS = SensorValue[shooterEnc] + moveAmt; \
while (moving(moveAmntS, accelTimeS, timeOutS, shooterEnc) && (condition))


bool moving (int moveAmnt, unsigned long accelTime, unsigned long timeOut, tSensors sen)
{
	if ((npgmTime < accelTime || SensorValue[sen] > moveAmnt) && npgmTime < timeOut) return true;
	else if (npgmTime >= timeOut)
	{
		writeDebugStreamLine("%d TimedOut past %d", npgmtime, timeOut);
		return false;
	}
	else
	{
		writeDebugStreamLine("%d TimedOut: Hasn't moved %d w/in %d ms. At%d", npgmtime, moveAmnt, accelTime, SensorValue[sen]);
		//if (sen == gSensor[shooterEnc]) killShooter();
		return false;
	}
}

typedef enum _tShooterState
{
	shooterIdle,
	shooterBreak,
	shooterReload,
	shooterHold,
	shooterShoot,
	shooterReset
} tShooterState;

void setShooter(word val)
{
	motor[shooter] = motor[shooterY] = LIM_TO_VAL(val, 127);
}
tShooterState gShooterState = shooterIdle;
tShooterState gShooterStateLst;
unsigned long gShooterStateTime; //Stores the time the shooter state is changed
int gShooterStateSen;
bool gShooterKilled = false;

int gShooterShotCount = 0;
//bool gShooterTimedOut = false;

void setShooterState (tShooterState state)
{
	if (state != gShooterState)
	{
		gShooterStateLst = gShooterState;
		gShooterState = state;

		gShooterStateTime = npgmtime;
		gShooterStateSen = SensorValue[shooterEnc];
		writeDebugStreamLine("%d Shooter State Set %d, Count:%d, Sen:%d, T:%d", nPgmTime, gShooterState, gShooterShotCount, gShooterStateSen, gShooterStateTime);
	}
}

task shooterStateSet()
{
	writeDebugStreamLine("%d Start Shooter State Machine Task", npgmtime);
	float shooterBreakOffset = 8;

	//SensorValue[shooterEnc] = 0;
	SensorValue[shooterEnc] = 0;

	//sCycleData cycle;
	//initCycle(cycle, 10, "shooter");
	while (true)
	{
		switch (gShooterState)
		{
			case shooterIdle:
				setShooter(0);
				break;
			case shooterBreak:
				setShooter(-90);
				unsigned long timerStart = npgmtime;
				while( (npgmTime-timerStart) < 80 ) sleep(10);
				setShooter(0);
				PlayTone(300, 50);

				setShooterState(shooterReload);
				//setShooterState(shooterIdle);
				break;
			case shooterReload:
				setShooter(127);
				int target = SHOOTER_RELOAD_POS;

				//WHILE_SHOOTER_MOVING(10, 100, 400, SensorValue[shooterEnc] < (target + SHOOTER_RELOAD_POS))
				while(SensorValue[shooterEnc] < target)
				{
					sleep(10);
				}

				writeDebugStreamLine("%d Reloaded to %d", npgmtime, SensorValue[shooterEnc]);

				setShooterState(shooterHold);
				break;
			case shooterHold:
				setShooter(SHOOTER_RELOAD_HOLD);
				break;
			case shooterShoot:
				unsigned long shotStartTime = npgmtime;
				gShooterShotCount++;
				target = SHOOTER_SHOOT_POS;
				setShooter(127);
				//writeDebugStreamLine("Fired shot #%d at %d, Tgt: %d, Enc: %d, Err: %d", gShooterShotCount, nPgmTime, SensorValue[shooterEnc], target, target - SensorValue[shooterEnc]);
				writeDebugStreamLine("%d Start shot %d: Time: %d Pos:%d ", npgmtime, gShooterShotCount, npgmtime-shotStartTime, SensorValue[shooterEnc]);

				bool shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

				//WHILE_SHOOTER_MOVING(10, 150, 700, !shotTargReached && (SensorValue[shooterEnc] < (target-85) || SensorValue[shooterEnc] > (target-15) || BALL_DETECTED))
				while(!shotTargReached && (SensorValue[shooterEnc] < (target-65) || SensorValue[shooterEnc] > (target-8) || BALL_DETECTED))
				{
					//writeDEbugStreamLine("Ball? %d", BALL_DETECTED);
					shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

					unsigned long timeElpsd = npgmtime-shotStartTime;

					sleep(10);
				}
				if (!shotTargReached)
				{
					writeDebugStreamLine("%d Ball gone: Val:%d, Time: %d Pos:%d, Targ:%d ", npgmtime, SensorValue[ballDetector], npgmtime-shotStartTime, SensorValue[shooterEnc], target);
					//setShooter(-90);
					//sleep(80);
					//setShooter(0);
					//gShooterShotCount--;
					//while (SensorValue[shooterEnc] > (target + SHOOTER_RELOAD_POS)) sleep(10);
					//setShooter(SHOOTER_RELOAD_HOLD);
					gShooterShotCount--;
					PlayTone(300, 50);

					setShooterState(shooterBreak);
				}
				else
				{
					writeDebugStreamLine("%d Start break: Time: %d Pos:%d ", npgmtime, npgmtime-shotStartTime, SensorValue[shooterEnc]);
					setShooter(-25);
					unsigned long startBreakTime = npgmtime;
					while ((npgmtime-startBreakTime) < 80)
					{
						//writeDebugStreamLine("%d Break", npgmtime);
						if (SensorValue[ballDetector] > 2000)
						{
							//writeDebugStreamLine("	%d Ball hit: Time: %d", npgmtime, npgmtime-shotStartTime);
						}
						sleep(10);
					}
					//sleep(80);
					//writeDebugStreamLine("Break done at %d, Enc: %d", nPgmTime, SensorValue[shooterEnc]);
					setShooter(0);

					writeDebugStreamLine("%d Shoot Trigger False - shot end", npgmtime);

					setShooterState(shooterReload);
				}
				break;

			case shooterReset:
				setShooter(-18);
				float pos = -1;
				float lstPos = -1;
				float vel = -1;
				unsigned long time = npgmtime;
				unsigned long lstTime = 0;
				sleep(250);
				do
				{
					pos = SensorValue[shooterEnc];
					time = npgmtime;

					vel = (float)(pos-lstPos)/(float)(time-lstTime);
					writeDebugStreamLine("%d Vel: %f", npgmtime, vel);
					lstPos = pos;
					lstTime = time;
					sleep(200);
				} while(vel < -0.001);// || lstVel < -0.001);
				setShooter(0);
				sleep(150);
				writeDEbugStreamLine("%d Reset Shooter from %d", npgmtime, SensorValue[shooterEnc]);
				PlayTone(300, 50);
				SensorValue[shooterEnc] = 0;
				gShooterShotCount = 0;
				sleep(50);

				setShooterState(shooterReload);
				break;
		}
		//endCycle(cycle);
		sleep(10);
	}
}

void shooterSafetySet(tShooterState state)
{
	//hogCPU();
	unsigned long stateElpsdTime = (npgmtime-gShooterStateTime);
	int senChange = SensorValue[shooterEnc]-gShooterStateSen;
		writeDebugStreamLine("	%d Shooter State Safety %d. TO: %d SenChange: %d", npgmtime, stateElpsdTime, gShooterState, senChange);
		stopTask(shooterStateSet);
		startTask(shooterStateSet);
		setShooterState(state);
		//releaseCPU();
}

void killShooter()
{
	if (!gShooterKilled)
	{
			//hogCPU();
			gShooterKilled = true;
			stopTask(shooterStateSet);
			setShooter(0);
			writeDebugStreamLine("	%d KILL SHOOTER TASK - Shooter Enc Not Plugged In. State: %d", npgmtime, gShooterState);
			//releaseCPU();
	}
}

void shooterSafety()
{
	if (!gShooterKilled)
	{
		hogCPU();
		unsigned long stateElpsdTime = (npgmtime-gShooterStateTime);
		int senChange = SensorValue[shooterEnc]-gShooterStateSen;

		if (gShooterState == shooterReset)
		{
			if (stateElpsdTime > 4500) shooterSafetySet(shooterIdle);
			else if (stateElpsdTime > 100 && senChange > 10) killShooter();
		}
		else if (gShooterState == shooterReload)
		{
			if (stateElpsdTime > 500) shooterSafetySet(shooterBreak);
			else if (stateElpsdTime > 150 && senChange < 10) killShooter();
		}
		else if (gShooterState == shooterShoot)
		{
			if (stateElpsdTime > 850) shooterSafetySet(shooterBreak);
			else if (stateElpsdTime > 150 && senChange < 10) killShooter();
		}
		releaseCPU();
	}
}

task driverControl
{
	bool shootBtn = false;
	bool shootBtnLst = false;

	int lstShotCount = 0;
	unsigned long lstShotTimer = 0;
	while (true)
	{
		shootBtn = vexRT[BTN_SHOOT];

		if (shootBtn && !shootBtnLst)
		{
			if (SensorValue[shooterEnc] < SHOOTER_RELOAD_POS) setShooterState(shooterReload);
			else if (gShooterShotCount != 0 && SensorValue[shooterEnc] >= (SHOOTER_NEXT_SHOOT_POS)) shooterSafetySet(shooterReset);
			else if (BALL_DETECTED)
			{
				setShooterState(shooterShoot);
				lstShotTimer = npgmtime;
				writeDebugStreamLine("%d FIRST SHOT TRIGGERED. # %d. Angler: %d", npgmtime, gShooterShotCount, SensorValue[anglerPoti]);
				//if ((npgmtime-secondShotTimer) > 900) secondShotTimer = npgmtime;
				//else secondShotTimer = 0;
			}
		}

		if (!shootBtn) lstShotTimer = 0;
		else if (shootBtn && gShooterState == shooterHold && (lstShotTimer != 0 && (nPgmTime-lstShotTimer) < 900))
		{
			setShooterState(shooterShoot);
			lstShotTimer = 0;
			writeDebugStreamLine("%d SECOND SHOT TRIGGERED. # %d", npgmtime, gShooterShotCount);
		}

		//Angler Controls
		if (abs(vexRt[Ch2]) > 20)
		{
			if (vexRT[Ch2] > 0 && SensorValue[anglerPoti] < ANGLER_POTI_TOP) motor[angler] = vexRT[Ch2];
			else if (vexRT[Ch2] < 0 && SensorValue[anglerPoti] > ANGLER_POTI_BOTTOM) motor[angler] = vexRT[Ch2];
		}
		else
		{
			if(SensorValue[anglerPoti] < 800) motor[angler] = -5;
			else if (SensorValue[anglerPoti] > 800 && SensorValue[anglerPoti] < 1000) motor[angler] = -10;
			else if (SensorValue[anglerPoti] > 1000 && SensorValue[anglerPoti] < 1500) motor[angler] = 0;
			else if (SensorValue[anglerPoti] > 1500 && SensorValue[anglerPoti] < 2250) motor[angler] = 8;
			else if (SensorValue[anglerPoti] > 2250 && SensorValue[anglerPoti] < 2600) motor[angler] = 10;
			else if (SensorValue[anglerPoti] > 2600 &&	SensorValue[anglerPoti] < 3500) motor[angler] = 5;
		}

		shootBtnLst = shootBtn;
		sleep(10);
	}
}

task main()
{
	clearDebugStream();

	int nBatteryLevel = nImmediateBatteryLevel;
	writeDebugStreamLine("%d Battery: %d", nPgmTime, nBatteryLevel);

	SensorValue[shooterEnc] = 0;
	startTask(shooterStateSet);
	startTask(driverControl);
	setShooterState(shooterReset);
	while (true)
	{
		shooterSafety();
		sleep(10);
	}
	stopTask(shooterStateSet);
	stopTask(driverControl);
}
