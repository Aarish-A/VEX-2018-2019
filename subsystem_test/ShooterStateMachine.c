#pragma config(Sensor, in1,    ballDetector,   sensorLineFollower)
#pragma config(Sensor, in8,    anglerPoti,     sensorPotentiometer)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  LED1,           sensorLEDtoVCC)
#pragma config(Motor,  port2,           driveLY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           intake,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           driveL,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooter,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterY,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           driveR,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
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

#define LIM_TO_VAL(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

/* Shooter Controls */
#define BTN_SHOOT Btn5U

/* Intake Controls */
#define BTN_INTAKE_UP Btn6U
#define BTN_INTAKE_DOWN Btn6D

/* Angler Controls */
#define JOY_ANGLER Ch2
#define BTN_ANGLER_PICKUP Btn5D

/* Macros */
	//Double Shot
#define BTN_SHOOT_FRONT_PF Btn7L
#define BTN_SHOOT_BACK_PF Btn7R
#define BTN_SHOOT_BACK Btn7D

/* Shooter Defines */
#define RESET_OFFSET 45 //35
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_HOLD 11
#define SHOOTER_SHOOT_POS ((gShooterShotCount*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_NEXT_SHOOT_POS (((1+gShooterShotCount)*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_RELOAD_POS ((SHOOTER_SHOOT_POS) + 165)//185)

#define BD_UNPLUGGED (SensorValue[ballDetector] >= 244 && SensorValue[ballDetector] <= 252)

#define BALL_DETECTED (SensorValue[ballDetector] < 1400)// && (!BD_UNPLUGGED))

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
	tHog();
	if (state != gShooterState)
	{
		gShooterStateLst = gShooterState;
		gShooterState = state;

		gShooterStateTime = nPgmTime;
		gShooterStateSen = SensorValue[shooterEnc];
		writeDebugStream("%d Shooter State Set %d ", nPgmTime, gShooterState);
		switch(gShooterState)
		{
			case shooterIdle: writeDebugStream("shooterIdle"); break;
			case shooterBreak: writeDebugStream("shooterBreak"); break;
			case shooterReload: writeDebugStream("shooterReload"); break;
			case shooterHold: writeDebugStream("shooterHold"); break;
			case shooterShoot: writeDebugStream("shooterShoot"); break;
			case shooterReset: writeDebugStream("shooterReset"); break;
			default: writeDebugStream("UNKNOWN STATE"); break;
		}
		writeDebugStreamLine(", Count:%d, Sen:%d, T:%d", gShooterShotCount, gShooterStateSen, gShooterStateTime);
	}
	tRelease();
}

task shooterStateSet()
{
	writeDebugStreamLine("%d Start Shooter State Machine Task", nPgmTime);
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
				tHog();
				setShooter(-90);
				unsigned long timerStart = nPgmTime;
				tRelease();
				while( (nPgmTime-timerStart) < 90 ) sleep(10);
				setShooter(0);
				//playTone(300, 50);

				setShooterState(shooterReload);
				break;
			case shooterReload:
				setShooter(127);
				int target = SHOOTER_RELOAD_POS;

				while(SensorValue[shooterEnc] < target)
				{
					sleep(10);
				}

				writeDebugStreamLine("%d Reloaded to %d. Targ: %d", nPgmTime, SensorValue[shooterEnc], target);

				setShooterState(shooterHold);
				break;
			case shooterHold:
				setShooter(SHOOTER_RELOAD_HOLD);
				break;
			case shooterShoot:
				unsigned long shotStartTime = nPgmTime;
				gShooterShotCount++;
				target = SHOOTER_SHOOT_POS;
				setShooter(127);
				writeDebugStreamLine("%d Start shot %d: Time: %d Pos:%d ", nPgmTime, gShooterShotCount, nPgmTime-shotStartTime, SensorValue[shooterEnc]);

				bool shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

				while(!shotTargReached && (SensorValue[shooterEnc] < (target-70) || SensorValue[shooterEnc] > (target-8) || BALL_DETECTED))
				{
					tHog();
					shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

					unsigned long timeElpsd = nPgmTime-shotStartTime;
					//writeDebugStreamLine("	%d Shooter moving to targ. Count%d. Targ:%d. Cur:%d", nPgmTime, gShooterShotCount, target, SensorValue[shooterEnc]);
					tRelease();
					sleep(10);
				}
				if (!shotTargReached)
				{
					tHog();
					writeDebugStreamLine("%d Ball gone: Val:%d, Time: %d Pos:%d, Targ:%d ", nPgmTime, SensorValue[ballDetector], nPgmTime-shotStartTime, SensorValue[shooterEnc], target);
					gShooterShotCount--;
					//playTone(300, 50);

					setShooterState(shooterBreak);
					tRelease();
				}
				else
				{
					writeDebugStreamLine("%d Start break: Time: %d Pos:%d ", nPgmTime, nPgmTime-shotStartTime, SensorValue[shooterEnc]);
					setShooter(-25);
					unsigned long startBreakTime = nPgmTime;

					bool ballThere = true;
					while ((nPgmTime-startBreakTime) < 80)
					{
						if (ballThere && !BALL_DETECTED)
						{
							ballThere = false;
							writeDebugStreamLine("	%d Ball Hit. Angler at %d holding at %d", nPgmTime, SensorValue[anglerPoti], motor[angler]);
						}
						sleep(10);
					}
					setShooter(0);

					writeDebugStreamLine("%d Shoot Trigger False - shot end", nPgmTime);

					setShooterState(shooterReload);
				}
				break;

			case shooterReset:
				setShooter(-18);
				float pos = -1;
				float lstPos = -1;
				float vel = -1;
				unsigned long time = nPgmTime;
				unsigned long lstTime = 0;
				sleep(250);
				do
				{
					pos = SensorValue[shooterEnc];
					time = nPgmTime;

					vel = (float)(pos-lstPos)/(float)(time-lstTime);
					writeDebugStreamLine("%d Vel: %f", nPgmTime, vel);
					lstPos = pos;
					lstTime = time;
					sleep(200);
				} while(vel < -0.001);
				setShooter(0);
				sleep(150);
				writeDebugStreamLine("%d Reset Shooter from %d", nPgmTime, SensorValue[shooterEnc]);
				playTone(300, 50);
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
	//tHog();
	unsigned long stateElpsdTime = (nPgmTime-gShooterStateTime);
	int senChange = SensorValue[shooterEnc]-gShooterStateSen;
		writeDebugStreamLine("	%d Shooter State (%d) Safety %d. TO: %d SenChange: %d", nPgmTime, gShooterState, stateElpsdTime, senChange);
		stopTask(shooterStateSet);
		startTask(shooterStateSet);
		setShooterState(state);
		//tRelease();
}

void killShooter()
{
	if (!gShooterKilled)
	{
			//tHog();
			unsigned long stateElpsdTime = (nPgmTime-gShooterStateTime);
			int senChange = SensorValue[shooterEnc]-gShooterStateSen;
			gShooterKilled = true;
			stopTask(shooterStateSet);
			setShooter(0);
			writeDebugStreamLine("	%d KILL SHOOTER SAFETY state %d Safety TO: %d SenChange: %d, Sen:%d", nPgmTime, gShooterState, stateElpsdTime, senChange, SensorValue[shooterEnc]);
			//writeDebugStreamLine("	%d KILL SHOOTER TASK - Shooter Enc Not Plugged In. State: %d", nPgmTime, gShooterState);
			//tRelease();
	}
}

void shooterSafety()
{
	tHog();
	if (!gShooterKilled)
	{
		unsigned long stateElpsdTime = (nPgmTime-gShooterStateTime);
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
			if (stateElpsdTime > 950) killShooter();//shooterSafetySet(shooterBreak);
			else if (stateElpsdTime > 150 && senChange < 10) killShooter();
		}
	}
	tRelease();
}

/* Angler Controls */
#define ANGLER_BOTTOM_POS  330
#define ANGLER_HORIZONTAL_POS 1165
#define ANGLER_TOP_POS 3866

#define ANGLER_AXEL_POS (ANGLER_TOP_POS-1850)

#define ANGLER_PICKUP_POS 940

//Positions shooting from back
#define ANGLER_BACK_TOP_FLAG 1320//1415
#define ANGLER_BACK_MID_FLAG 1100//1265//1165
#define ANGLER_BACK_BOTTOM_FLAG 955

//Positions shooting from back of front platform tile
#define ANGLER_FRONT_TOP_FLAG 1700//1415
#define ANGLER_FRONT_MID_FLAG 1280//1265//1165
#define ANGLER_FRONT_BOTTOM_FLAG 955

int gAnglerPower = 0;
void setAngler(word val)
{
	gAnglerPower = LIM_TO_VAL(val, 127);
	motor[angler] = LIM_TO_VAL(val, 127);
}

typedef enum _tAnglerState
{
	anglerIdle,
	anglerMove,
	anglerHold,
	anglerManual
} tAnglerState;

tAnglerState gAnglerState = anglerIdle;
tAnglerState gAnglerStateLst = gAnglerState;
unsigned long gAnglerStateTime = nPgmTime;
int gAnglerStateSen = SensorValue[anglerPoti];

int gAnglerTarget = ANGLER_BOTTOM_POS;
int gAnglerGoodCount = 0;
int gAnglerAcceptableRange = 25;

void setAnglerState (tAnglerState state, int acceptableRange = 25)
{
	tHog();
	if (state != gAnglerState)
	{
		gAnglerStateLst = gAnglerState;
		gAnglerState = state;

		gAnglerAcceptableRange = abs(acceptableRange);

		gAnglerStateTime = nPgmTime;
		gAnglerStateSen = SensorValue[anglerPoti];
		writeDebugStream("%d Angler State Set %d ", nPgmTime, gAnglerState);
		switch(gAnglerState)
		{
			case anglerIdle: writeDebugStream("anglerIdle"); break;
			case anglerMove: writeDebugStream("anglerMove"); break;
			case anglerHold: writeDebugStream("anglerHold"); break;
			case anglerManual: writeDebugStream("anglerManual"); break;

			default: writeDebugStream("UNKNOWN STATE"); break;
		}
		writeDebugStreamLine(". Targ:%d, Sen:%d, Time:%d, GoodCount:%d", gAnglerTarget, gAnglerStateSen, gAnglerStateTime, gAnglerGoodCount);
	}
	tRelease();
}

task anglerStateSet()
{
	unsigned long time = nPgmTime;
	unsigned long timeLst = 0;
	unsigned long deltaTime = 0;
	unsigned long sen = 0;
	unsigned long senLst = 0;
	unsigned long deltaSen = 0;

	float kP = 0.095;//0.09;
	float kI = 0.048;//0.016;
	float iVal, pVal;

	while (true)
	{
		switch (gAnglerState)
		{
			case anglerIdle:
			{
				setAngler(0);
				break;
			}
			case anglerMove:
			{
				gAnglerGoodCount = 0;
				if (SensorValue[anglerPoti] < gAnglerTarget)
				{
					float kP = 0.3;
					int targ = gAnglerTarget-200;
					while(SensorValue[anglerPoti] < targ)
					{
						tHog();
						int error = gAnglerTarget - SensorValue[anglerPoti];
						float power = error * kP;
						if (abs(SensorValue[anglerPoti]-ANGLER_HORIZONTAL_POS) < 250) power+=6;
						setAngler(power);
						tRelease();
						sleep(10);
					}
					setAngler(-15);
					unsigned long startBreakTime = nPgmTime;
					float vel = 10;
					do
					{
						sen = SensorValue[anglerPoti];
						time = nPgmTime;

						vel = (float)(sen-senLst)/(float)(time-timeLst);

						writeDebugStreamLine("%d Break. Pos:%d, Vel:%f", nPgmTime, sen, vel);
						senLst = sen;
						timeLst = time;
						sleep(10);
					} while ((vel > 2.7 && (nPgmTime-startBreakTime) < 100) || (nPgmTime-startBreakTime) < 20);
					setAngler(0);
				}
				setAnglerState(anglerHold);
				break;
			}
			case anglerHold:
			{
				tHog();
				time = nPgmTime;
				sen = SensorValue[anglerPoti];

				int error = gAnglerTarget - SensorValue[anglerPoti];
				float pVal = error * kP;

				unsigned long deltaTime = time-timeLst;
				deltaSen = sen - senLst;
				if (deltaTime <= 0 || abs(deltaSen) > 2 || abs(error) < 10)// || gAnglerStateLst == anglerManual)
				{
					iVal = 0;
				}
				else iVal += ( (float)error / (float)(deltaTime) ) * kI;

				if (abs(deltaSen) < 5 && abs(error) < gAnglerAcceptableRange)// && abs(gAnglerGoodCount) < 25)
				{
					gAnglerGoodCount++;
					SensorValue[LED1] = 300;
					//writeDebugStreamLine("	%d Angler Stopped at %d. Inc gAnglerGoodCount to %d. Err:%d", nPgmTime, sen, gAnglerGoodCount, (gAnglerTarget-sen));
				}
				else
				{
					gAnglerGoodCount = 0;
					SensorValue[LED1] = 0;
				}

				int power = pVal + iVal;// + 5;
				if (SensorValue[anglerPoti] < gAnglerTarget && abs(SensorValue[anglerPoti]-ANGLER_HORIZONTAL_POS) < 250) power+=6;

				setAngler(power);

				//if(gAnglerStateLst == anglerMove)
				unsigned long tElpsd = (nPgmTime-gAnglerStateTime);

				//if((tElpsd < 100) || (tElpsd > 700 && tElpsd < 850))
				//	writeDebugStreamLine("%d Sen:%d, Err: %d, pVal:%f, iVal:%f, pow: %f, dT:%d, dS:%d", nPgmTime, SensorValue[anglerPoti], error, pVal, iVal, power, deltaTime, deltaSen);

				if (gAnglerGoodCount == 5) writeDebugStreamLine("		%d Done hold to %d in %d ms. vel:%f. Sen:%d", nPgmTime, gAnglerTarget, (nPgmTime-gAnglerStateTime), (deltaSen/deltaTime), SensorValue[anglerPoti]);
				senLst = sen;
				timeLst = time;

				datalogDataGroupStart();
				//datalogAddValue(0, SensorValue[anglerPoti]);
				//datalogAddValue(1, (pVal*10.0));
				//datalogAddValue(2, (iVal*10.0));
				//datalogAddValue(3, gAnglerPower);
				datalogAddValue(4, SensorValue[shooterEnc]);
				datalogAddValue(5, SensorValue[ballDetector]);
				datalogDataGroupEnd();

				tRelease();
				break;
			}
			case anglerManual:
			{
				if (abs(vexRT[Ch2]) > 10)
				{
					if (vexRT[Ch2] > 0 && SensorValue[anglerPoti] < (ANGLER_TOP_POS - 100))
					{
						setAngler(vexRT[Ch2]);
						//if (SensorValue[anglerPoti] > ANGLER_AXEL_POS && SensorValue[shooterEnc] > (SHOOTER_RELOAD_POS-10)) goto AnglerHoldTrigger;
					}
					else if (vexRT[Ch2] < 0 && SensorValue[anglerPoti] > (ANGLER_BOTTOM_POS + 100)) setAngler(vexRT[Ch2]);
					else goto AnglerHoldTrigger;
				}
				else
				{
					AnglerHoldTrigger:
					setAngler(0);
					sleep(100);
					gAnglerTarget = SensorValue[anglerPoti];
					setAnglerState(anglerIdle);
					//setAnglerState(anglerHold);
				}
				break;
			}
		}

		//tHog();
		//datalogDataGroupStart();
		//datalogAddValue(0, SensorValue[anglerPoti]);
		//datalogAddValue(1, (pVal*10.0));
		//datalogAddValue(2, (iVal*10.0));
		//datalogAddValue(3, gAnglerPower);
		//datalogAddValue(4, SensorValue[shooterEnc]);
		//datalogAddValue(5, SensorValue[ballDetector]);
		//datalogDataGroupEnd();
		//tRelease();
		sleep(10);
	}
}

void anglerMoveToPos(int pos, int acceptableRange)
{
	acceptableRange = abs(acceptableRange);

	gAnglerTarget = pos;
	gAnglerGoodCount = 0;
	setAnglerState(anglerMove, acceptableRange);
}

void doubleShot(int posA, int posB, int acceptableRange, bool waitForFirstShot = true, bool waitForSecShot = true)
{
		acceptableRange = abs(acceptableRange);

      int startShotCount = gShooterShotCount;

      //First shot
      gAnglerTarget = posA;
      gAnglerGoodCount = 0;
      setAnglerState(anglerMove, acceptableRange);
      if (waitForFirstShot)
    	{
	      while (gAnglerGoodCount < 5) sleep(10);
	      writeDebugStreamLine("%d Done waiting for point. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
	    }
      setShooterState(shooterShoot);

      while (gShooterState != shooterReload) sleep(10); //Start moving angler immediately after first shot applies breaking

    	//Second shot
      gAnglerTarget = posB;
      gAnglerGoodCount = 0;
      setAnglerState(anglerMove, acceptableRange);
      if (waitForSecShot)
    	{
	      while (gAnglerGoodCount < 5) sleep(10);
	      writeDebugStreamLine("%d Done waiting for point. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
	    }
	    setShooterState(shooterShoot);

      while (gShooterState != shooterHold) sleep(10);

      //If we haven't made two shots, try the last shot again
      if ((gShooterShotCount-startShotCount) < 2)
      {
        writeDebugStreamLine("  %d Second shot failed (ball jumped). Try again", nPgmTime);
        sleep(50);
        setShooterState(shooterShoot);
        while (gShooterState != shooterHold) sleep(10);
      }

      writeDebugStreamLine("%d Done double point and shoot from back. Angler:%d. Shooter:%d", nPgmTime, SensorValue[anglerPoti], SensorValue[shooterEnc]);

}

/* Driver Control */
task driverControl
{
	bool shootBtn = false;
	bool shootBtnLst = false;

	bool shootFrontPFBtn = false;
	bool shootFrontPFBtnLst = false;

	bool shootBackPFBtn = false;
	bool shootBackPFBtnLst = false;

	bool shootBackBtn = false;
	bool shootBackBtnLst = false;

	bool anglerPickupBtn = false;
	bool anglerPickupBtnLst = false;

	//int lstShotCount = 0;
	unsigned long lstShotTimer = 0;

	while (true)
	{
		shootBtn = (bool)vexRT[BTN_SHOOT];

		shootFrontPfBtn = (bool)vexRT[BTN_SHOOT_FRONT_PF];
		shootBackPfBtn = (bool)vexRT[BTN_SHOOT_BACK_PF];
		shootBackBtn = (bool)vexRT[BTN_SHOOT_BACK];
		anglerPickupBtn = (bool)vexRT[BTN_ANGLER_PICKUP];

		if (shootBtn && !shootBtnLst)
		{
			if (SensorValue[shooterEnc] < SHOOTER_RELOAD_POS) setShooterState(shooterReload);
			else if (gShooterShotCount != 0 && SensorValue[shooterEnc] >= (SHOOTER_NEXT_SHOOT_POS)) shooterSafetySet(shooterReset);
			else if (BALL_DETECTED)
			{
				setShooterState(shooterShoot);
				lstShotTimer = nPgmTime;
				writeDebugStreamLine("%d FIRST SHOT TRIGGERED. # %d. Angler: %d", nPgmTime, gShooterShotCount, SensorValue[anglerPoti]);
				//if ((nPgmTime-secondShotTimer) > 900) secondShotTimer = nPgmTime;
				//else secondShotTimer = 0;
			}
		}

		if (!shootBtn) lstShotTimer = 0;
		else if (shootBtn && gShooterState == shooterHold && (lstShotTimer != 0 && (nPgmTime-lstShotTimer) < 900))
		{
			setShooterState(shooterShoot);
			lstShotTimer = 0;
			writeDebugStreamLine("%d SECOND SHOT TRIGGERED. # %d", nPgmTime, gShooterShotCount);
		}
		if ((shootFrontPFBtn && !shootFrontPFBtnLst) && !(shootBtn && !shootBtnLst))
		{
			doubleShot(ANGLER_FRONT_MID_FLAG, ANGLER_FRONT_TOP_FLAG, 60, false, false);
		}
		else if ((shootBackBtn && !shootBackBtnLst) && !(shootBtn && !shootBtnLst))
		{
			doubleShot(ANGLER_BACK_MID_FLAG, ANGLER_BACK_TOP_FLAG, 25, true, true);
		}
		else if (anglerPickupBtn && !anglerPickupBtnLst)
		{
			anglerMoveToPos(ANGLER_PICKUP_POS, 150);
		}
		else if (abs(vexRT[Ch2]) > 10)
		{
			setAnglerState(anglerManual);
		}

		shootBtnLst = shootBtn;
		shootFrontPFBtnLst = shootFrontPFBtn;
		shootBackPFBtnLst = shootBackPFBtn;
		shootBackBtnLst = shootBackBtn;

		anglerPickupBtnLst = anglerPickupBtn;
		sleep(10);
	}
}

task main()
{
	clearDebugStream();
	datalogClear();

	int nBatteryLevel = nImmediateBatteryLevel;
	writeDebugStreamLine("%d Battery: %d", nPgmTime, nBatteryLevel);

	SensorValue[shooterEnc] = 0;
	startTask(shooterStateSet);
	startTask(driverControl);
	startTask(anglerStateSet);
	setShooterState(shooterReset);

	bool ballThere = false;
	bool ballThereLst = ballThere;

	float anglerSen = SensorValue[anglerPoti];
	float anglerSenLst = anglerSen;
	unsigned long time = nPgmTime;
	unsigned long timeLst = time;
	float anglerVel;
	while (true)
	{
		ballThere = BALL_DETECTED;
		anglerSen = SensorValue[anglerPoti];
		time = nPgmTime;

		shooterSafety();
		float anglerVel;
		if ((time-timeLst) > 0)
			anglerVel = ( (float)(anglerSen-anglerSenLst) / (float)(time-timeLst) );
		else anglerVel = 0;
		if (ballThere && !ballThereLst) writeDebugStreamLine("	%d Ball Detected. Shooter:%d Angler:%d (err:%d). Vel:%f", nPgmTime, SensorValue[shooterEnc], SensorValue[anglerPoti], (gAnglerTarget-SensorValue[anglerPoti]), anglerVel);
		else if (!ballThere && ballThereLst) writeDebugStreamLine("	%d Ball Off. Shooter:%d Angler:%d (err:%d). Vel:%f", nPgmTime, SensorValue[shooterEnc], SensorValue[anglerPoti], (gAnglerTarget-SensorValue[anglerPoti]), anglerVel);

		ballThereLst = ballThere;
		anglerSenLst = anglerSen;
		timeLst = time;
		sleep(10);
	}
	stopTask(shooterStateSet);
	stopTask(driverControl);
	stopTask(anglerStateSet);
}
