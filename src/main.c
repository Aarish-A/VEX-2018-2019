#pragma config(Sensor, in1,    anglerPoti,     sensorPotentiometer)
#pragma config(Sensor, in2,    decapperPoti,   sensorPotentiometer)
#pragma config(Sensor, in8,    ballDetector,   sensorLineFollower)
#pragma config(Sensor, dgtl1,  trackL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  trackR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  trackB,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  shooterEnc,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  LED1,           sensorLEDtoVCC)
#pragma config(Motor,  port1,           decapper,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           driveLY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           intake,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           driveL,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           shooter,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           shooterY,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           driveR,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           angler,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int gBatteryLevel;
int gBackupBatteryLevel;

int driveLogs = 1;
int intakeLogs = 1;

int anglerLogs = 1;
int shooterLogs = 1;
int macroLogs = 1;

#define LOG(machineIn) if(machineIn##Logs) writeDebugStreamLine

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

/* Includes */
// Year-independent libraries (headers)
#include "utilities.h"
#include "cycle.h"
#include "motors.h"
#include "custom_turning.h"
#include "safety.h"

// Year-independent libraries (source)
//#include "utilities.c"
#include "cycle.c"
#include "motors.c"
#include "custom_turning.c"
#include "safety.c"

// Other includes
#include "Vex_Competition_Includes_Custom.c"
#include "controls.h"

// Important globals
bool gAnglerShooterTaskRunning = false;

//Iniitialize safeties
sSafety driveSafety, shooterSafety, anglerSafety; //No intake safety
void initSafeties()
{
	initSafety(driveSafety, "driveSafety", velSensor, trackL);
	initSafety(shooterSafety, "shooterSafety", velSensor, shooterEnc);
	initSafety(anglerSafety, "anglerSafety", velSensor, anglerPoti);
}

/* Shooter Positions */
int gShootTuneMode = false;

int gShooterShotCount = 0;
#define RESET_OFFSET 45 //40
#define SHOOTER_GEAR_RATIO 1.0
#define SHOOTER_RELOAD_HOLD 11
#define SHOOTER_SHOOT_POS ((gShooterShotCount*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_NEXT_SHOOT_POS (((1+gShooterShotCount)*360.0*SHOOTER_GEAR_RATIO) + RESET_OFFSET)
#define SHOOTER_RELOAD_POS ((SHOOTER_SHOOT_POS) + 185)//165)

/* Drive Controls */
typedef enum _tTurnDir
{
	turnNone,
	turnCW,
	turnCCW
} tTurnDir;

#define DRIVE_THROTTLE_DZ 15
#define DRIVE_TURN_DZ 10

int gDriveThrottleRaw, gDriveTurnRaw,;

int gDrivePower, gDriveLength;

bool gDriveFlip = false;

int gDriveBreakPow = 11;
int gDriveBreakWait = 500;

void setDrive(word left, word right)
{
	setMotor(driveL, left);
	setMotor(driveLY, left);
	setMotor(driveR, right);
	setMotor(driveRY, right);
}

typedef enum _tDriveState
{
	driveIdle,
	driveManual,
	driveMoveTime,
	driveBreak
} tDriveState;

tDriveState gDriveState = driveIdle;
tDriveState gDriveStateLst = gDriveState;
unsigned long gDriveStateTime;

tTurnDir gDriveTurnDir = turnNone;

void setDriveState (tDriveState state, int drivePower = -1, int driveLength = -1)
{
	tHog();
	if (state != gDriveState)
	{
		gDriveStateLst = gDriveState;
		gDriveState = state;

		//gDriveTurnDir = turnNone;

		gDriveStateTime = nPgmTime;

		gDrivePower = drivePower;
		gDriveLength = driveLength;

		writeDebugStream("%d Drive State Set %d ", nPgmTime, gDriveState);
		switch(gDriveState)
		{
		case driveIdle: writeDebugStream("driveIdle"); break;
		case driveManual: writeDebugStream("driveManual"); break;
		case driveMoveTime: writeDebugStream("driveMoveTime"); break;
		case driveBreak: writeDebugStream("driveBreak"); break;

		default: writeDebugStream("UNKNOWN STATE"); break;
		}
		writeDebugStreamLine(", turnDir: %d, pow: %d, length:%d, T:%d", gDriveTurnDir, gDrivePower, gDriveLength, gDriveStateTime);
	}
	tRelease();
}

task driveStateSet()
{
	sCycleData driveCycle;
	initCycle(driveCycle, 10, "driveCycle");

	int throttle, turn;
	word left, right;

	while (true)
	{
		switch(gDriveState)
		{
		case driveIdle:
			{
				setDrive(0, 0);
				break;
			}
		case driveManual:
			{
				if (abs(gDriveThrottleRaw) > DRIVE_THROTTLE_DZ) throttle = gDriveThrottleRaw;
				else throttle = 0;
				if (gDriveFlip) throttle *= -1;

				if (abs(gDriveTurnRaw) > DRIVE_TURN_DZ)
				{
					turn = gDriveTurnRaw;//lookupTurn(gDriveTurnRaw);
					gDriveTurnDir = (turn > 0)? turnCW : turnCCW;
				}
				else turn = 0;

				if ((nPgmTime-gDriveStateTime) < 50) gDriveTurnDir = turnNone; //For first 250ms of being in manual, turnDir reset to turnNone

				left = throttle + turn;
				right = throttle - turn;

				setDrive(left, right);

				if (!(abs(gDriveTurnRaw) > DRIVE_TURN_DZ) && !(abs(gDriveThrottleRaw) > DRIVE_THROTTLE_DZ)) setDriveState(driveBreak);

				break;
			}
		case driveMoveTime:
			{
				unsigned long timeCur = nPgmTime;
				unsigned long length = timeCur + gDriveLength;
				setDrive(gDrivePower, gDrivePower);
				while (nPgmTime < length) sleep(10);
				setDrive(0,0);

				setDriveState(driveIdle);
				break;
			}
		case driveBreak:
			{
				gDriveBreakPow = abs(gDriveBreakPow);
				gDriveBreakWait = abs(gDriveBreakWait);
				//LOG(drive)("%d In breaking. DriveBreakPow:%d. TurnDir:%d", nPgmTime, gDrivebreakPow, gDriveTurnDir);

				if (gDriveTurnDir == turnCW) setDrive(-gDriveBreakPow, gDriveBreakPow);
				else if (gDriveTurnDir == turnCCW) setDrive(gDriveBreakPow, -gDriveBreakPow);

				if ((nPgmTime-gDriveStateTime) > gDriveBreakWait || gDriveTurnDir == turnNone) setDriveState(driveIdle);
				break;
			}
		}
		endCycle(driveCycle);
	}
}


/* Intake Controls */
void setIntake(word power)
{
	setMotor(intake, power);
}

typedef enum _tIntakeState
{
	intakeIdle,
	intakeUp,
	intakeDown

} tIntakeState;

tIntakeState gIntakeState = intakeIdle;
tIntakeState gIntakeStateLst = gIntakeState;
unsigned long gIntakeStateTime;

void setIntakeState (tIntakeState state)
{
	tHog();
	if (state != gIntakeState)
	{
		gIntakeStateLst = gIntakeState;
		gIntakeState = state;

		gIntakeStateTime = nPgmTime;

		writeDebugStream("%d Intake State Set %d ", nPgmTime, gIntakeState);
		switch(gIntakeState)
		{
		case intakeIdle: writeDebugStream("intakeIdle"); break;
		case intakeUp: writeDebugStream("intakeUp"); break;
		case intakeDown: writeDebugStream("intakeDown"); break;

		default: writeDebugStream("UNKNOWN STATE"); break;
		}
		writeDebugStreamLine(", AnglerSen:%d, T:%d", SensorValue[anglerPoti], gIntakeStateTime);
	}
	tRelease();
}


task intakeStateSet()
{
	sCycleData intakeCycle;
	initCycle(intakeCycle, 10, "intakeCycle");

	while (true)
	{
		switch (gIntakeState)
		{
		case intakeIdle:
			{
				setIntake(-15);
				break;
			}
		case intakeUp:
			{
				setIntake(127);
				break;
			}
		case intakeDown:
			{
				setIntake(-127);
				break;
			}
		}
		//endCycle(intakeCycle);
		sleep(10);
	}
}

bool intakeUpBtn = false;
bool intakeUpBtnLst = false;
bool intakeDownBtn = false;
bool intakeDownBtnLst = false;
void intakeControls()
{
	intakeUpBtn = (bool)vexRT[BTN_INTAKE_UP];
	intakeDownBtn = (bool)vexRT[BTN_INTAKE_DOWN];

	if (intakeUpBtn && !intakeUpBtnLst)
	{
		if (gIntakeState != intakeIdle) setIntakeState(intakeIdle);
		else setIntakeState(intakeUp);
	}
	else if (intakeDownBtn && !intakeDownBtnLst)
	{
		if (gIntakeState != intakeIdle) setIntakeState(intakeIdle);
		else setIntakeState(intakeDown);
	}

	intakeUpBtnLst = intakeUpBtn;
	intakeDownBtnLst = intakeDownBtn;
}

//int gAnglerTarget = 1000;
/* Angler Controls */
#define ANGLER_BOTTOM_POS  330
#define ANGLER_HORIZONTAL_POS 1165
#define ANGLER_TOP_POS 3866

#define ANGLER_TOP_LIM_POS (ANGLER_TOP_POS-1000)

#define ANGLER_AXEL_POS (ANGLER_TOP_POS-2000)

#define ANGLER_GROUND_PICKUP_POS 900
#define ANGLER_LOW_PF_PICKUP_POS 1470
#define ANGLER_CAP_PICKUP_POS 1530

#define ANGLER_CAP_FLIP_POS 820

int gAnglerBackTopFlag = 1390; //1490
int gAnglerBackMidFlag = 1175; //1270

int gAnglerFrontPFTopFlag = 1770;//1730;
int gAnglerFrontPFMidFlag = 1360;//1310;

int gAnglerMidPFTopFlag = 1570;//1730;
int gAnglerMidPFMidFlag = 1250;//1310;

//Positions shooting from back of front platform tile
int gAnglerBackPFTopFlag = 1510;//1410;
int gAnglerBackPFMidFlag = 1220;//1160;

int gAnglerPower = 0;
void setAngler(word val)
{
	setMotor(angler, val);
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

	sCycleData anglerCycle;
	initCycle(anglerCycle, 10, "anglerCycle");

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

						LOG(angler)("%d Angler Break. Pos:%d, Vel:%f", nPgmTime, sen, vel);
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
				if (gAnglerTarget > ANGLER_AXEL_POS && SensorValue[shooterEnc] > (SHOOTER_RELOAD_POS-10)) gAnglerTarget = ANGLER_AXEL_POS;
				time = nPgmTime;
				sen = SensorValue[anglerPoti];

				int error = gAnglerTarget - SensorValue[anglerPoti];
				pVal = error * kP;

				unsigned long deltaTime = time-timeLst;
				deltaSen = sen - senLst;
				if (deltaTime <= 0 || abs(deltaSen) > 2 || abs(error) < 10)// || gAnglerStateLst == anglerManual)
				{
					iVal = 0;
				}
				else iVal += ( (float)error / (float)(deltaTime) ) * kI;

				if (abs(deltaSen) < 4 && abs(error) < gAnglerAcceptableRange)// && abs(gAnglerGoodCount) < 25)
				{
					gAnglerGoodCount++;
					SensorValue[LED1] = 300;
					//LOG(angler)("  %d Angler Stopped at %d. Inc gAnglerGoodCount to %d. Err:%d", nPgmTime, sen, gAnglerGoodCount, (gAnglerTarget-sen));
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
				//  LOG(angler)("%d Sen:%d, Err: %d, pVal:%f, iVal:%f, pow: %f, dT:%d, dS:%d", nPgmTime, SensorValue[anglerPoti], error, pVal, iVal, power, deltaTime, deltaSen);

				//if (gAnglerGoodCount == 5) LOG(angler)("   %d Done angler hold to %d in %d ms. vel:%f. Sen:%d", nPgmTime, gAnglerTarget, (nPgmTime-gAnglerStateTime), (deltaSen/deltaTime), SensorValue[anglerPoti]);
				senLst = sen;
				timeLst = time;

				//datalogDataGroupStart();
				//datalogAddValue(0, SensorValue[anglerPoti]);
				//datalogAddValue(1, (pVal*10.0));
				//datalogAddValue(2, (iVal*10.0));
				//datalogAddValue(3, gAnglerPower);
				//datalogAddValue(4, SensorValue[shooterEnc]);
				//datalogAddValue(5, SensorValue[ballDetector]);
				//datalogDataGroupEnd();

				tRelease();
				break;
			}
		case anglerManual:
			{
				if (abs(vexRT[JOY_ANGLER]) > 10)
				{
					if (vexRT[JOY_ANGLER] > 0 && SensorValue[anglerPoti] < (ANGLER_TOP_LIM_POS - 100))
					{
						if (SensorValue[anglerPoti] > ANGLER_AXEL_POS && SensorValue[shooterEnc] > (SHOOTER_RELOAD_POS-10)) goto AnglerHoldTrigger;
						setAngler(vexRT[JOY_ANGLER]);
					}
					else if (vexRT[JOY_ANGLER] < 0 && SensorValue[anglerPoti] > (ANGLER_BOTTOM_POS + 100)) setAngler(vexRT[JOY_ANGLER]);
					else goto AnglerHoldTrigger;
				}
				else
				{
				AnglerHoldTrigger:
					setAngler(0);
					sleep(200);
					gAnglerTarget = SensorValue[anglerPoti];
					//setAnglerState(anglerIdle);
					setAnglerState(anglerHold);
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

		//endCycle(anglerCycle);
		sleep(10);
	}
}

/* Shooter Controls */
#define BD_UNPLUGGED (SensorValue[ballDetector] >= 244 && SensorValue[ballDetector] <= 252)
#define BALL_DETECTED (SensorValue[ballDetector] < 2000)// && (!BD_UNPLUGGED))

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
	setMotor(shooter, val);
	setMotor(shooterY, val);
}
tShooterState gShooterState = shooterIdle;
tShooterState gShooterStateLst = gShooterState;
tShooterState gShooterStateLstLst = gShooterStateLst;
unsigned long gShooterStateTime; //Stores the time the shooter state is changed
int gShooterStateSen;
bool gShooterKilled = false;

//bool gShooterTimedOut = false;

void setShooterState (tShooterState state)
{
	tHog();
	if (state != gShooterState)
	{
		gShooterStateLstLst = gShooterStateLst;
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
		writeDebugStreamLine(", Count:%d, Sen:%d, Targ:%d, NextTarg:%d, T:%d", gShooterShotCount, gShooterStateSen, SHOOTER_SHOOT_POS, SHOOTER_NEXT_SHOOT_POS, gShooterStateTime);
	}
	tRelease();
}

task shooterStateSet()
{
	writeDebugStreamLine("%d Start Shooter State Machine Task", nPgmTime);
	gShooterKilled = false;

	float shooterBreakOffset = 8;

	//SensorValue[shooterEnc] = 0;
	SensorValue[shooterEnc] = 0;

	sCycleData shooterCycle;
	initCycle(shooterCycle, 10, "shooterCycle");
	while (true)
	{
		switch (gShooterState)
		{
		case shooterIdle:
			{
				setShooter(0);
				break;
			}
		case shooterBreak:
			{
				tHog();
				setShooter(-90);
				unsigned long timerStart = nPgmTime;
				tRelease();
				while( (nPgmTime-timerStart) < 90 ) sleep(10);
				setShooter(0);
				//playTone(300, 50);

				setShooterState(shooterReload);
				break;
			}
		case shooterReload:
			{
				if (SensorValue[anglerPoti] < ANGLER_AXEL_POS)
				{
					setShooter(127);
					int target = SHOOTER_RELOAD_POS;

					while(SensorValue[shooterEnc] < target)
					{
						sleep(10);
					}

					LOG(shooter)("%d Reloaded to %d. Targ: %d", nPgmTime, SensorValue[shooterEnc], target);

					setShooterState(shooterHold);
				}
				else setShooterState(shooterIdle);
				break;
			}
		case shooterHold:
			{
				setShooter(SHOOTER_RELOAD_HOLD);
				break;
			}
		case shooterShoot:
			{
				unsigned long shotStartTime = nPgmTime;
				gShooterShotCount++;
				int target = SHOOTER_SHOOT_POS;
				setShooter(127);
				LOG(shooter)("%d Start shot %d: Time: %d Pos:%d ", nPgmTime, gShooterShotCount, nPgmTime-shotStartTime, SensorValue[shooterEnc]);

				bool shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

				while(!shotTargReached && (SensorValue[shooterEnc] < (target-80) || SensorValue[shooterEnc] > (target-35) || BALL_DETECTED))
				{
					tHog();
					shotTargReached = ( SensorValue[shooterEnc] > (target-shooterBreakOffset) );

					unsigned long timeElpsd = nPgmTime-shotStartTime;
					//LOG(shooter)("  %d Shooter moving to targ. Count%d. Targ:%d. Cur:%d", nPgmTime, gShooterShotCount, target, SensorValue[shooterEnc]);
					tRelease();
					sleep(10);
				}
				if (!shotTargReached)
				{
					tHog();
					LOG(shooter)("%d Ball gone: Val:%d, Time: %d Pos:%d, Targ:%d ", nPgmTime, SensorValue[ballDetector], nPgmTime-shotStartTime, SensorValue[shooterEnc], target);
					gShooterShotCount--;
					//playTone(300, 50);

					setShooterState(shooterBreak);
					tRelease();
				}
				else
				{
					LOG(shooter)("%d Start break: Time: %d Pos:%d ", nPgmTime, nPgmTime-shotStartTime, SensorValue[shooterEnc]);
					setShooter(-25);
					unsigned long startBreakTime = nPgmTime;

					bool ballThere = true;
					while ((nPgmTime-startBreakTime) < 80)
					{
						if (ballThere && !BALL_DETECTED)
						{
							ballThere = false;
							LOG(shooter)("  > %d BALL HIT. Angler at %d holding at %d. Err:%d", nPgmTime, SensorValue[anglerPoti], gMotor[angler].powerCur, (gAnglerTarget-SensorValue[anglerPoti]));
						}
						sleep(10);
					}
					setShooter(0);

					LOG(shooter)("%d Shoot Trigger False - shot end", nPgmTime);

					setShooterState(shooterReload);
				}
				break;
			}
		case shooterReset:
			{
				//if (gShooterShotCount != 0) //Let it come to a rest if it enters the reset outside of startup
				//{
				LOG(shooter)("%d Shooter rest for 500ms in reset. ShotCount:%d. Pos:%d", nPgmTime, gShooterShotCount, SensorValue[shooterEnc]);
				setShooter(0);
				sleep(500);
				//}
				LOG(shooter)("%d Start reset at -12 in reverse", nPgmTime);
				setShooter(-12);
				float pos = SensorValue[shooterEnc];
				float lstPos = 10;
				float vel = -1;
				unsigned long time = nPgmTime;
				unsigned long lstTime = 0;
				sleep(300);
				do
				{
					pos = SensorValue[shooterEnc];
					time = nPgmTime;

					vel = (float)(pos-lstPos);///(float)(time-lstTime);
					LOG(shooter)("%d Pos:%d, PosLst:%d, Vel: %f", nPgmTime, pos, lstPos, vel);
					lstPos = pos;
					lstTime = time;
					sleep(300);
				} while(vel < -1);
				//setShooter(0);
				//sleep(100);
				LOG(shooter)("%d Reset Shooter from %d. Vel:%d", nPgmTime, SensorValue[shooterEnc], vel);
				playTone(300, 50);
				SensorValue[shooterEnc] = 0;
				gShooterShotCount = 0;
				sleep(50);

				//if (SensorValue[anglerPoti] < ANGLER_AXEL_POS) setShooterState(shooterReload);
				//setShooterState(shooterIdle);
				setShooterState(shooterReload);
				break;
			}
		}
		//endCycle(shooterCycle);
		sleep(10);
	}
}

void shooterSafetySet(tShooterState state)
{
	//tHog();
	unsigned long stateElpsdTime = (nPgmTime-gShooterStateTime);
	int senChange = SensorValue[shooterEnc]-gShooterStateSen;
	writeDebugStreamLine(">>	  %d Shooter State %d Safety. TO: %d SenChange: %d", nPgmTime, gShooterState, stateElpsdTime, senChange);
	stopTask(shooterStateSet);
	startTask(shooterStateSet);
	setShooterState(state);
	//tRelease();
}

void killShooter()
{
	//if (!gShooterKilled)
	//{
		//Kill shooter
		unsigned long stateElpsdTime = (nPgmTime-gShooterStateTime);
		int senChange = SensorValue[shooterEnc]-gShooterStateSen;
		gShooterKilled = true;
		stopTask(shooterStateSet);
		setShooter(0);
		writeDebugStreamLine(">>>	 %d KILL SHOOTER SAFETY state %d Safety TO: %d SenChange: %d, Sen:%d", nPgmTime, gShooterState, stateElpsdTime, senChange, SensorValue[shooterEnc]);

		//Wait for shooter to come to stop
		int sen, senLst, senDelta;
		do
		{
			sen = SensorValue[shooterEnc];
			senDelta = sen - senLst;
			writeDebugStream("%d Waiting for shooter to come to stop. Pos:%d. LstPos:%d, DeltaPos:%d", nPgmTime, sen, senLst, senDelta);
			senLst = sen;
			sleep(300);
		}while(abs(senDelta) > 1);
		sleep(1000);

		//Reset shooter
		startTask(shooterStateSet);
		setShooterState(shooterReset);

	//}
}

void shooterSafetyCheck()
{
	tHog();
	if (!gShooterKilled)
	{
		unsigned long stateElpsdTime = (nPgmTime-gShooterStateTime);
		int senChange = SensorValue[shooterEnc]-gShooterStateSen;

		if (gShooterState == shooterReset)
		{
			if (stateElpsdTime > 7000) shooterSafetySet(shooterIdle);
			else if (stateElpsdTime > 100 && senChange > 10) killShooter();
		}
		else if (gShooterState == shooterReload)
		{
			if (stateElpsdTime > 600) shooterSafetySet(shooterBreak);
			else if (stateElpsdTime > 150 && senChange < 10) killShooter();
		}
		else if (gShooterState == shooterShoot)
		{
			if (stateElpsdTime > 1200) killShooter();//shooterSafetySet(shooterBreak);
			else if (stateElpsdTime > 150 && senChange < 10 && (gShooterStateLst != shooterBreak && gShooterStateLstLst != shooterBreak)) killShooter();
		}
	}
	tRelease();
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
		LOG(macro)("%d Done waiting for first point. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
	}
	setShooterState(shooterShoot);

	while (gShooterState != shooterReload) sleep(10); //Start moving angler immediately after first shot applies breaking
		LOG(macro)("%d Done waiting for first reload. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
	//Second shot
	gAnglerTarget = posB;
	gAnglerGoodCount = 0;
	setAnglerState(anglerMove, acceptableRange);
	if (waitForSecShot)
	{
		while (gAnglerGoodCount < 5) sleep(10);
		LOG(macro)("%d Done waiting for second point. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
	}
	setShooterState(shooterShoot);

	while (gShooterState != shooterHold) sleep(10);

	//If we haven't made two shots, try the last shot again
	if ((gShooterShotCount-startShotCount) < 2)
	{
		LOG(macro)("  %d Second shot failed (ball jumped). Try again", nPgmTime);
		sleep(50);
		setShooterState(shooterShoot);
		while (gShooterState != shooterHold) sleep(10);
	}

	LOG(macro)("%d Done double point and shoot from back. Angler:%d. Shooter:%d", nPgmTime, SensorValue[anglerPoti], SensorValue[shooterEnc]);
}

//ToDo: Add safeties to anglerShooter
void anglerShooter(int posA, int posB, int acceptableRange, bool waitForFirstShot = true, bool waitForSecShot = true, TVexJoysticks btn, bool reversePos = false)
{
	int kGoodCount = 5;

	//setDriveState(driveIdle); //Kill drive

	bool btnReleased = false;
	acceptableRange = abs(acceptableRange);
	int startShotCount = gShooterShotCount;

	//Handle test mode
	if (gShootTuneMode)
	{
		acceptableRange = 20;
		waitForFirstShot = true;
		waitForSecShot = true;
	}

	//Handle reverse request
	if (reversePos)
	{
		int temp = posA;
		posA = posB;
		posB = temp;
	}

	//First shot
	gAnglerTarget = posA;
	gAnglerGoodCount = 0;
	setAnglerState(anglerMove, acceptableRange);
	if (waitForFirstShot)
	{
		//setSafetyTO(anglerSafety, "angle for shot1", 7000);
		//WHILE(anglerSafety, (gAnglerGoodCount < kGoodCount))
		while(gAnglerGoodCount < kGoodCount)
		{
			if (!vexRT[btn]) btnReleased = true;
			sleep(10);
		}
		LOG(macro)("%d Done waiting for point. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
	}

	//Move shooter once it's in hold (protects from other states overwriting our state-change-request)
	while (gShooterState != shooterHold)
	{
		if (!vexRT[btn]) btnReleased = true;
			sleep(10);
	}
	//Start taking first shot
	setShooterState(shooterShoot);

	while (gShooterState != shooterReload) //Start moving angler immediately after first shot applies breaking
	{
		if (!vexRT[btn]) btnReleased = true;
		sleep(10);
	}

	if ((gShooterShotCount-startShotCount) < 1)
	{
		LOG(macro)("  %d First shot failed (ball jumped). Try again", nPgmTime);
		sleep(50);
		setShooterState(shooterShoot);
		while (gShooterState != shooterReload) //Start moving angler immediately after first shot applies breaking
		{
			if (!vexRT[btn]) btnReleased = true;
			sleep(10);
		}
	}

	if (!btnReleased && vexRT[btn])
	{
		startShotCount = gShooterShotCount;
		//Second shot
		gAnglerTarget = posB;
		gAnglerGoodCount = 0;
		setAnglerState(anglerMove, acceptableRange);
		if (waitForSecShot)
		{
			while (gAnglerGoodCount < kGoodCount)
			{
				if (!vexRT[btn]) btnReleased = true;
				sleep(10);
			}
			LOG(macro)("%d Done waiting for point. Angler:%d", nPgmTime, SensorValue[anglerPoti]);
		}
		setShooterState(shooterShoot);

		while (gShooterState != shooterHold) sleep(10);

		//If we haven't made two shots, try the last shot again
		if ((gShooterShotCount-startShotCount) < 1)
		{
			LOG(macro)("  %d Second shot failed (ball jumped). Try again", nPgmTime);
			sleep(50);
			setShooterState(shooterShoot);
			while (gShooterState != shooterHold) sleep(10);
		}

		LOG(macro)("%d Done double point and shoot. Angler:%d. Shooter:%d", nPgmTime, SensorValue[anglerPoti], SensorValue[shooterEnc]);

		writeDebugStreamLine("%d Angler to pickup ground position", nPgmTime);
		anglerMoveToPos(ANGLER_GROUND_PICKUP_POS, 150);
	}
}

int anglerShooterPosA, anglerShooterPosB, anglerShooterAcceptableRange;
bool anglerShooterWaitForFirstShot, anglerShooterWaitForSecShot, anglerShooterReversePos;
TVexJoysticks anglerShooterBtn;
task anglerShooterTask()
{
	gAnglerShooterTaskRunning = true;
	anglerShooter(anglerShooterPosA, anglerShooterPosB, anglerShooterAcceptableRange, anglerShooterWaitForFirstShot, anglerShooterWaitForSecShot, anglerShooterBtn, anglerShooterReversePos);
	gAnglerShooterTaskRunning = false;
	return;
}
#define ANGLER_SHOOTER_TASK(posA, posB, acceptableRange, waitForFirstShot, waitForSecShot, btn, reversePos) \
anglerShooterPosA = posA; anglerShooterPosB = posB; anglerShooterAcceptableRange = acceptableRange; \
anglerShooterWaitForFirstShot = waitForFirstShot; anglerShooterWaitForSecShot = waitForSecShot; anglerShooterReversePos = reversePos; \
anglerShooterBtn = btn; \
startTask(anglerShooterTask)

//Declarations for ball log
bool gBallThere = false;
bool gBallThereLst = gBallThere;

float gAnglerSen = SensorValue[anglerPoti];
float gAnglerSenLst = gAnglerSen;
unsigned long gBallTime = nPgmTime;
unsigned long gBallTimeLst = gBallTime;
//float anglerVel;

void ballTrackLog()
{
	gBallThere = BALL_DETECTED;
	gAnglerSen = SensorValue[anglerPoti];
	gBallTime = nPgmTime;

	float anglerVel;
	if ((gBallTime-gBallTimeLst) > 0)
		anglerVel = ( (float)(gAnglerSen-gAnglerSenLst) / (float)(gBallTime-gBallTimeLst) );
	else anglerVel = 0;
	if (gBallThere && !gBallThereLst) writeDebugStreamLine("	%d Ball Detected (%d). Shooter:%d Angler:%d (err:%d). Vel:%f", nPgmTime, SensorValue[ballDetector], SensorValue[shooterEnc], SensorValue[anglerPoti], (gAnglerTarget-SensorValue[anglerPoti]), anglerVel);
	else if (!gBallThere && gBallThereLst) writeDebugStreamLine("	%d Ball Off (%d). Shooter:%d Angler:%d (err:%d). Vel:%f", nPgmTime, SensorValue[ballDetector], SensorValue[shooterEnc], SensorValue[anglerPoti], (gAnglerTarget-SensorValue[anglerPoti]), anglerVel);

	gBallThereLst = gBallThere;
	gAnglerSenLst = gAnglerSen;
	gBallTimeLst = gBallTime;
}

task monitorVals()
{
	while(true)
	{
		tHog();
		shooterSafetyCheck();
		ballTrackLog();

		datalogDataGroupStart();
		datalogAddValue(0, SensorValue[anglerPoti]);
		//datalogAddValue(1, (pVal*10.0));
		//datalogAddValue(2, (iVal*10.0));
		datalogAddValue(3, gAnglerPower);
		datalogAddValue(4, SensorValue[shooterEnc]);
		datalogAddValue(5, SensorValue[ballDetector]);
		datalogDataGroupEnd();

		tRelease();

		sleep(10);
	}
}

/* LCD Controls */
#include "LCD.h"
#include "LCD.c"

/* Modes */
void startTasks()
{
	tHog();

	SensorValue[shooterEnc] = 0;
	startTask(driveStateSet);
	startTask(shooterStateSet);
	startTask(intakeStateSet);
	startTask(anglerStateSet);
	startTask(monitorVals);

	startTask(handleLCD);

	setShooterState(shooterReset);

	tRelease();

	sleep(50);
	while(gShooterState != shooterHold && gShooterState != shooterIdle) sleep(10);

}

void stopTasks()
{
	tHog();
	stopTask(driveStateSet);
	stopTask(shooterStateSet);
	stopTask(intakeStateSet);
	stopTask(anglerStateSet);

	stopTask(monitorVals);

	stopTask(handleLCD);
	tRelease();
}

void startup()
{
	clearDebugStream();
	datalogClear();
	setupMotors();
	updateTurnLookup();
	initSafeties();

	gBatteryLevel = nImmediateBatteryLevel;
	gBackupBatteryLevel = BackupBatteryLevel;
	writeDebugStreamLine("%d Startup Battery: %d, BU Battery: %d", nPgmTime, gBatteryLevel, gBackupBatteryLevel);

	SensorValue[shooterEnc] = 0;

	startTasks();
	//Setup Joysticks & Buttons
}

void disabled()
{
	//updateSensorInputs();

	//selectAuto();
	//handleLcd();
}

task autonomous()
{
	writeDebugStreamLine("\n%d Start Autonomous", nPgmTime);
	sCycleData cycle;
	initCycle(cycle, 10, "auto");
	startTasks();
	while(true)
	{
		endCycle(cycle);
	}
	stopTasks();
}
task usercontrol()
{
	writeDebugStreamLine("\n%d Start UserControl", nPgmTime);
	sCycleData cycle;
	initCycle(cycle, 10, "usercontrol");

	//startTasks();


	bool shootBtn = false;
	bool shootBtnLst = false;

	bool shootFrontPFBtn = false;
	bool shootFrontPFBtnLst = false;

	bool shootMidPFBtn = false;
	bool shootMidPFBtnLst = false;

	bool shootBackPFBtn = false;
	bool shootBackPFBtnLst = false;

	bool shootBackBtn = false;
	bool shootBackBtnLst = false;

	bool anglerPickupGroundBtn = false;
	bool anglerPickupGroundBtnLst = false;

	bool anglerPickupCapBtn = false;
	bool anglerPickupCapBtnLst = false;

	bool anglerPickupLowPFBtn = false;
	bool anglerPickupLowPFBtnLst = false;

	//int lstShotCount = 0;
	unsigned long lstShotTimer = 0;

	//setDriveState(driveManual);
	while (true)
	{
		/* Handle Btns */
		shootBtn = (bool)vexRT[BTN_SHOOT];
		shootFrontPFBtn = (bool)vexRT[BTN_SHOOT_FRONT_PF];
		shootMidPFBtn = (bool)vexRT[BTN_SHOOT_MID_PF];
		shootBackPFBtn = (bool)vexRT[BTN_SHOOT_BACK_PF];
		shootBackBtn = (bool)vexRT[BTN_SHOOT_BACK];
		anglerPickupGroundBtn = (bool)vexRT[BTN_ANGLER_GROUND_PICKUP];
		anglerPickupCapBtn = (bool)vexRT[BTN_ANGLER_CAP_PICKUP];
		anglerPickupLowPFBtn = (bool)vexRT[BTN_ANGLER_LOW_PF_PICKUP];

		/* Drive Controls */
		gDriveThrottleRaw = (!gAnglerShooterTaskRunning)? vexRT[JOY_DRIVE_THROTTLE] : vexRT[JOY_ANGLER];
		gDriveTurnRaw = (!gAnglerShooterTaskRunning)? vexRT[JOY_DRIVE_TURN] : (vexRT[JOY_DECAPPER] * 0.5);
		if ( ((abs(gDriveTurnRaw) > DRIVE_TURN_DZ) || (abs(gDriveThrottleRaw) > DRIVE_THROTTLE_DZ)) && gDriveState != driveMoveTime) setDriveState(driveManual);

		/* Intake Controls */
		intakeControls();

		/* Shooter & Angler Controls */
		if (!gAnglerShooterTaskRunning)
		{
			if (shootBtn && !shootBtnLst)
			{
				if (vexRT[BTN_SHIFT])
				{
					gDriveFlip = !gDriveFlip;
				}
				else if (SensorValue[shooterEnc] < SHOOTER_RELOAD_POS)
				{
					setShooterState(shooterReload);
				}
				else if (SensorValue[shooterEnc] >= (SHOOTER_NEXT_SHOOT_POS-50))
				{
					writeDebugStreamLine("%d Shot count wrong. Reset", nPgmTime);
					shooterSafetySet(shooterReset);
				}
				else if (BALL_DETECTED && SensorValue[anglerPoti] > ANGLER_GROUND_PICKUP_POS)
				{
					setShooterState(shooterShoot);
					lstShotTimer = nPgmTime;
					writeDebugStreamLine("%d FIRST SHOT TRIGGERED. # %d. Angler: %d", nPgmTime, gShooterShotCount, SensorValue[anglerPoti]);
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
				writeDebugStreamLine("%d Shoot from front of platform", nPgmTime);
				ANGLER_SHOOTER_TASK(gAnglerFrontPFMidFlag, gAnglerFrontPFTopFlag, 60, false, false, BTN_SHOOT_FRONT_PF, vexRT[BTN_SHIFT]);
			}
			else if ((shootMidPFBtn && !shootMidPFBtnLst) && !(shootBtn && !shootBtnLst))
			{
				writeDebugStreamLine("%d Shoot from mid of platform", nPgmTime);
				ANGLER_SHOOTER_TASK(gAnglerMidPFMidFlag, gAnglerMidPFTopFlag, 60, false, false, BTN_SHOOT_MID_PF, vexRT[BTN_SHIFT]);
			}
			else if ((shootBackPFBtn && !shootBackPFBtnLst) && !(shootBtn && !shootBtnLst))
			{
				writeDebugStreamLine("%d Shoot from back of platform", nPgmTime);
				ANGLER_SHOOTER_TASK(gAnglerBackPFMidFlag, gAnglerBackPFTopFlag, 60, false, false, BTN_SHOOT_BACK_PF, vexRT[BTN_SHIFT]);
			}
			else if ((shootBackBtn && !shootBackBtnLst) && !(shootBtn && !shootBtnLst))
			{
				writeDebugStreamLine("%d Shoot from back of field", nPgmTime);
				ANGLER_SHOOTER_TASK(gAnglerBackMidFlag, gAnglerBackTopFlag, 25, true, true, BTN_SHOOT_BACK, vexRT[BTN_SHIFT]);
			}
			else if (anglerPickupGroundBtn && !anglerPickupGroundBtnLst)
			{
				if (vexRT[BTN_SHIFT])
				{
					writeDebugStreamLine("%d Angler to flip cap position", nPgmTime);
					anglerMoveToPos(ANGLER_CAP_FLIP_POS, 150);
					while(gAnglerGoodCount < 5) sleep(10);
					setIntakeState(intakeDown);
					setDriveState(driveMoveTime, 70, 250);
					while(gDriveState != driveIdle) sleep(10);
					anglerMoveToPos(2800, 100);
					setDriveState(driveMoveTime, 70, 250);
					while(gDriveState != driveIdle) sleep(10);
					setDriveState(driveMoveTime, -11, 400);
				}
				else
				{
					writeDebugStreamLine("%d Angler to pickup ground position", nPgmTime);
					anglerMoveToPos(ANGLER_GROUND_PICKUP_POS, 150);
				}
			}
			else if (anglerPickupCapBtn && !anglerPickupCapBtnLst)
			{
				writeDebugStreamLine("%d Angler to pickup cap position", nPgmTime);
				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 150);
			}
			else if (anglerPickupLowPFBtn && !anglerPickupLowPFBtnLst)
			{
				writeDebugStreamLine("%d Angler to pickup low platform position", nPgmTime);
				anglerMoveToPos(ANGLER_LOW_PF_PICKUP_POS, 150);
			}
			else if (abs(vexRT[JOY_ANGLER]) > 10)
			{
				setAnglerState(anglerManual);
			}
		}

		/* Handle Btns */
		shootBtnLst = shootBtn;
		shootFrontPFBtnLst = shootFrontPFBtn;
		shootMidPFBtnLst = shootMidPFBtn;
		shootBackPFBtnLst = shootBackPFBtn;
		shootBackBtnLst = shootBackBtn;

		anglerPickupGroundBtnLst = anglerPickupGroundBtn;
		anglerPickupCapBtnLst = anglerPickupCapBtn;
		anglerPickupLowPFBtnLst = anglerPickupLowPFBtn;

		endCycle(cycle);
	}

	stopTasks();
}
