# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.c"
#pragma config(Sensor, in1, autoPoti, sensorPotentiometer)
#pragma config(Sensor, in2, mobilePoti, sensorPotentiometer)
#pragma config(Sensor, in3, liftPoti, sensorPotentiometer)
#pragma config(Sensor, in4, armPoti, sensorPotentiometer)
#pragma config(Sensor, in5, limLift, sensorAnalog)
#pragma config(Sensor, in6, lsBarL, sensorReflection)
#pragma config(Sensor, in7, lsBarR, sensorReflection)
#pragma config(Sensor, in8, lsMobile, sensorReflection)
#pragma config(Sensor, dgtl1, trackL, sensorQuadEncoder)
#pragma config(Sensor, dgtl3, trackR, sensorQuadEncoder)
#pragma config(Sensor, dgtl5, trackB, sensorQuadEncoder)
#pragma config(Sensor, dgtl7, sonarL, sensorSONAR_mm)
#pragma config(Sensor, dgtl9, limArm, sensorTouch)
#pragma config(Sensor, dgtl10, jmpSkills, sensorDigitalIn)
#pragma config(Sensor, dgtl11, sonarR, sensorSONAR_mm)
#pragma config(Motor, port2, liftR, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor, port3, driveL1, tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor, port4, driveL2, tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor, port5, arm, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor, port6, mobile, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor, port7, driveR2, tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor, port8, driveR1, tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor, port9, liftL, tmotorVex393HighSpeed_MC29, openLoop, reversed)







# 1 "state.h" 1
# 13 "state.h"
typedef enum _tVelDir
{
 velEither = -1,
 velUp = 0,
 velDown = 1
}tVelDir;
typedef enum _tVelType
{
 velSensor,
 velLocalY,
 velAngle
} tVelType;
# 32 "main.c" 2
# 1 "func_to_state.h" 1
# 33 "main.c" 2


# 1 "task.h" 1
typedef struct s_taskElement
{
 word next;
 word prev;
 word child;
 word parent;
} taskElement;

taskElement tEls[kNumbOfTasks + 1];



unsigned char _hogLevel = 0;

void tInit();
void tStart(word id, bool detached = false);
void tStop(word id);
void tStopAll(word id, bool notMe = false);
void tUnreg(word id);
void tHog();
void tRelease();
# 36 "main.c" 2
# 1 "motors.h" 1

typedef struct _sMotor
{
 word power;
 word curPower;
 float powerScale;



} sMotor;


void setupMotors();
void updateMotors();
void updateMotor(tMotor mot);


sMotor gMotor[kNumbOfTotalMotors];
# 37 "main.c" 2
# 1 "sensors.h" 1




typedef enum _tSensorClass
{
 snclsNone,
 snclsInput,
 snclsOutput,
 snclsOther
} tSensorClass;

typedef enum _tSensorMode
{
 snmdNormal,
 snmdDgtIn,
 snmdInverted
} tSensorMode;


typedef struct _sSensorDataPoint
{
 int value;
 unsigned long timestamp;
} sSensorDataPoint;

typedef struct _sSensor
{
 int value;
 int lstValue;
 int rawValue;
 tSensorClass cls;
 tSensors port;
 float velocity;
 float lstVelocity;
 sSensorDataPoint dataPointArr[30];
 ubyte arrHead;
 ubyte arrTail;
 ubyte dataCount;

 tSensorMode mode;
 bool potiCheckVel;
 int dgtMin;
 int dgtMax;
# 53 "sensors.h"
 ubyte filterAcc;
 ubyte jumpCount;

} sSensor;


void setupSensors();
void updateSensorOutput(tSensors sen);
void updateSensorOutputs();
void updateSensorInput(tSensors sen);
void updateSensorInputs();
tSensorClass checkSenClass(tSensors sen);
bool correctBtnIn(tSensors sen);
void setupDgtIn(tSensors sen, int min, int max);
void setupInvertedSen(tSensors sen);
void resetQuadratureEncoder(tSensors sen);
void velocityCheck(tSensors sen);
void startSensor(tSensors sen);
void startSensors();


sSensor gSensor[kNumbOfTotalSensors];
# 38 "main.c" 2
# 1 "joysticks.h" 1

typedef struct _sJoy
{
 short cur;
 short lst;
 short deadzone;
 bool enabled;
 TVexJoysticks mirror;
} sJoy;


void setupJoysticks();
void updateJoysticks();
void updateJoystick(TVexJoysticks joy);
void enableJoystick(TVexJoysticks joy);
void mirrorJoystick(TVexJoysticks joy, TVexJoysticks mirror);


sJoy gJoy[kNumbOfVexRFIndices];
# 39 "main.c" 2
# 1 "cycle.h" 1

typedef struct _sCycleData {
 unsigned long period;
 unsigned long startTime;
 unsigned long count;
 unsigned long time;
 string name;
} sCycleData;


void initCycle(sCycleData& data, unsigned long period, const string name);
void endCycle(sCycleData& data);
# 40 "main.c" 2
# 1 "utilities.h" 1
# 31 "utilities.h"
float fmod(float x, float y);
float degToRad(float degrees);
float radToDeg(float radians);
float nearAngle(float angle, float reference);
float simplifyAngle(float angle);
void stopAllButCurrentTasks();
void startTaskID(word id);
void stopTaskID(word id);
# 41 "main.c" 2
# 1 "auto.h" 1
# 28 "auto.h"
typedef enum _turnDir
{
 cw,
 ccw,
 ch
} tTurnDir;

typedef enum _facingDir
{
 facingBack = -1,
 facingNone = 0,
 facingFront = 1
} tFacingDir;


typedef struct _pos
{
 int leftStart;
 int rightStart;
 int backStart;

 float aStart;

 float a;
 float y;
 float x;

 int leftLst;
 int rightLst;
 int backLst;
} sPos;

typedef struct _vel
{
 float a;
 float y;
 float x;
 float localY;

 unsigned long lstChecked;
 float lstPosA;
 float lstPosY;
 float lstPosX;
} sVel;

typedef struct _vector
{
 float y;
 float x;
} sVector;

typedef struct _polar
{
 float magnitude;
 float angle;
} sPolar;

typedef struct _line
{
 sVector p1;
 sVector p2;

 float m;
 float b;
} sLine;


void trackPosition(int left, int right, int back, sPos& position);
void resetPosition(sPos& position);
void resetVelocity(sVel& velocity, sPos& position);
void trackVelocity(sPos position, sVel& velocity);
task trackPositionTask();
void resetPositionFull(sPos& position, float x, float y, float a);


void vectorToPolar(sVector& vector, sPolar& polar);
void polarToVector(sPolar& polar, sVector& vector);


void makeLine(sLine& line);
float getAngleOfLine(sLine line);
float getLengthOfLine(sLine line);


void offsetPos(float& x, float& y, float offset);
byte facingAngle(float targAngle, float offset);
byte facingCoord(float targX, float targY, float offset = (PI/4));
task autoMotorSensorUpdateTask();
void applyHarshStop();


unsigned long gAutoTime = 0;
sPos gPosition;
sVel gVelocity;
# 42 "main.c" 2


# 1 "task.c" 1
void tInit()
{
 for (word i = 0; i <= kNumbOfTasks; ++i)
 {
  tEls[i].child = -1;
  tEls[i].prev = -1;
  tEls[i].parent = -1;
  tEls[i].next = -1;
 }
}

void tStart(word id, bool detached)
{
 if (!detached)
 {
  tHog();
  tEls[id].parent = nCurrentTask;
  tEls[id].next = tEls[nCurrentTask].child;
  if (tEls[nCurrentTask].child != -1) tEls[tEls[nCurrentTask].child].prev = id;
  tEls[nCurrentTask].child = id;
  tRelease();
 }
 startTaskID(id);
}

void tStop(word id)
{
 tHog();
 tUnreg(id);
 stopTaskID(id);
 tRelease();
}

void tStopAll(word id, bool notMe)
{
 bool stopCur = false;
 tHog();
 bool updated[kNumbOfTasks];
 memset(updated, 0, kNumbOfTasks);
 while (tEls[id].child != -1 && !updated[tEls[id].child])
 {
  updated[tEls[id].child] = true;
  if (tEls[id].child == nCurrentTask)
  {
   stopCur = !notMe;
   tUnreg(tEls[id].child);
  }
  else tStop(tEls[id].child);
 }
 tStop(id);
 if (stopCur)
 {
  tRelease();
  stopTaskID(nCurrentTask);
 }
 tRelease();
}

void tUnreg(word id)
{
 tHog();
 if (tEls[id].next != -1)
  tEls[tEls[id].next].prev = tEls[id].prev;
 if (tEls[id].prev != -1)
  tEls[tEls[id].prev].next = tEls[id].next;
 if (tEls[id].parent != -1 && tEls[tEls[id].parent].child == id)
  tEls[tEls[id].parent].child = tEls[id].next;

 bool updated[kNumbOfTasks];
 memset(updated, 0, kNumbOfTasks);
 word ce = tEls[id].child, le = -1;
 while (ce != -1 && !updated[ce])
 {
  updated[ce] = true;
  tEls[ce].parent = tEls[id].parent;
  le = ce;
  ce = tEls[ce].next;
 }
 if (le != -1 && tEls[id].parent != -1)
 {
  tEls[le].next = tEls[tEls[id].parent].child;
  tEls[tEls[tEls[id].parent].child].prev = le;
  tEls[tEls[id].parent].child = tEls[id].child;
 }

 tEls[id].child = -1;
 tEls[id].prev = -1;
 tEls[id].parent = -1;
 tEls[id].next = -1;
 tRelease();
}

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
# 45 "main.c" 2
# 1 "motors.c" 1

void setupMotors()
{

 for (ubyte i = 0; i < kNumbOfTotalMotors; ++i) {
  gMotor[i].power = gMotor[i].curPower = motor[i] = 0;
  gMotor[i].powerScale = -1;



 }




}

void updateMotors()
{





 for (ubyte i = 0; i < kNumbOfTotalMotors; ++i)
  updateMotor(i);





}

void updateMotor(tMotor mot)
{
 motor[mot] = gMotor[mot].curPower = gMotor[mot].powerScale >= 0 ? round(gMotor[mot].power * gMotor[mot].powerScale) : gMotor[mot].power;




}
# 46 "main.c" 2
# 1 "sensors.c" 1

bool correctBtnIn(tSensors sen)
{
 int value = SensorValue[sen];
 return value >= gSensor[sen].dgtMin && value <= gSensor[sen].dgtMax;
}

void updateSensorOutput(tSensors sen)
{
 SensorValue[sen] = gSensor[sen].lstValue = gSensor[sen].value;
}

void updateSensorOutputs()
{
 for (ubyte i = 0; i < kNumbOfTotalSensors; ++i)
 {
  if (gSensor[i].cls == snclsOutput)
   updateSensorOutput(i);
 }
}

void updateSensorInput(tSensors sen)
{
 tHog();
 gSensor[sen].lstValue = gSensor[sen].value;
 int raw = gSensor[sen].rawValue = SensorValue[sen];

 switch (gSensor[sen].mode)
 {
 case snmdDgtIn:
  gSensor[sen].value = correctBtnIn(sen);
  break;
 case snmdInverted:
  gSensor[sen].value = !raw;
  break;
 default:
  gSensor[sen].value = raw;
  break;
 }


 if (SensorType[sen] == sensorPotentiometer || SensorType[sen] == sensorQuadEncoder)
 {
  sSensor& s = gSensor[sen];
  unsigned long t = nPgmTime;
  if (t > s.dataPointArr[s.arrHead].timestamp)
  {
   if (s.dataCount <= 30)
   {
    s.dataCount++;
   }

   s.arrHead = ( (s.arrHead+1) % 30 );
   s.dataPointArr[s.arrHead].value = s.value;
   s.dataPointArr[s.arrHead].timestamp = t;

   if (s.dataCount >= 30)
    s.arrTail = ( (s.arrTail+1) % 30 );
  }
 }
 tRelease();
# 79 "sensors.c"
 if (SensorType[sen] == sensorPotentiometer && ++gSensor[sen].filterAcc < 10 && ( (abs(gSensor[sen].value - gSensor[sen].lstValue) > 400) ) )
 {
  if (gSensor[sen].filterAcc == 1)
  {
   writeDebugStreamLine("%d port %d jumped from %d to %d", nPgmTime, sen - port1 + 1, gSensor[sen].lstValue, gSensor[sen].value);
   gSensor[sen].jumpCount ++;
  }
  gSensor[sen].value = gSensor[sen].lstValue;
 }
 else
  gSensor[sen].filterAcc = 0;

}

void updateSensorInputs()
{





 for (ubyte i = 0; i < kNumbOfTotalSensors; ++i)
 {
  if (gSensor[i].cls == snclsInput)
   updateSensorInput(i);
 }





}

tSensorClass checkSenClass(tSensors sen)
{
 if (SensorType[sen] == sensorNone)
  return snclsNone;
 else if (SensorType[sen] == sensorDigitalOut || SensorType[sen] == sensorLEDtoVCC)
  return snclsOutput;
 else
  return snclsInput;
}

void setupDgtIn(tSensors sen, int min, int max)
{
 gSensor[sen].mode = snmdDgtIn;
 gSensor[sen].dgtMin = min;
 gSensor[sen].dgtMax = max;
}

void setupInvertedSen(tSensors sen)
{
 gSensor[sen].mode = snmdInverted;
}

void resetQuadratureEncoder(tSensors sen)
{
 gSensor[sen].value = 0;
 SensorValue[sen] = 0;
}

void velocityCheck(tSensors sen)
{
 tHog();
 unsigned long t = nPgmTime;
 sSensor& s = gSensor[sen];

 if (SensorType[sen] == sensorPotentiometer || SensorType[sen] == sensorQuadEncoder)
 {
  if(s.dataCount > 5)
  {
   bool outOfBounds = false;
   if (s.arrHead >= 30)
   {
    writeDebugStreamLine("%d SENSOR PORT%d arrHead OUT OF BOUNDS:%d", nPgmTime, sen - port1 + 1, s.arrHead);
    outOfBounds = true;
   }
   if (s.arrTail >= 30)
   {
    writeDebugStreamLine("%d SENSOR PORT%d arrTail OUT OF BOUNDS:%d", nPgmTime, sen - port1 + 1, s.arrTail);
    outOfBounds = true;
   }
   if (!outOfBounds)
   {
    unsigned long tDif = (s.dataPointArr[s.arrHead].timestamp - s.dataPointArr[s.arrTail].timestamp);
    if( tDif <= 0 )
    {
     writeDebugStreamLine("%d SENSOR PORT%d VEL TIMESTAMP ERROR - head:%d t=%d, tail:%d t=%d", nPgmTime, sen - port1 + 1, s.arrHead, s.dataPointArr[s.arrHead].timestamp, s.arrTail, s.dataPointArr[s.arrTail].timestamp);
    }
    else
    {

     ubyte lstVelHead = ((s.arrHead)==0? (30 - 1):(s.arrHead-1));
     s.lstVelocity = (float)(s.dataPointArr[lstVelHead].value - s.dataPointArr[s.arrTail].value) / (float)(s.dataPointArr[lstVelHead].timestamp - s.dataPointArr[s.arrTail].timestamp);
     if (abs(s.lstVelocity) < 0.0035)
      s.lstVelocity = 0;

     s.velocity = (float)(s.dataPointArr[s.arrHead].value - s.dataPointArr[s.arrTail].value) / (float)(tDif)
     if (abs(s.velocity) < 0.0035)
      s.velocity = 0;
    }
   }
  }
 }

 tRelease();
}

void startSensor(tSensors sen)
{
 if (gSensor[sen].cls == snclsInput)
  gSensor[sen].value = gSensor[sen].lstValue = SensorValue[sen];
}

void startSensors()
{
 for (ubyte i = 0; i < kNumbOfTotalSensors; ++i)
  startSensor(i);
}

void setupSensors()
{

 for (ubyte i = 0; i < kNumbOfTotalSensors; ++i)
 {
  gSensor[i].cls = checkSenClass(i);
  gSensor[i].port = (tSensors)i;
  gSensor[i].mode = snmdNormal;

  gSensor[i].lstVelocity = 0;
  gSensor[i].velocity = 0;


  for (ubyte j = 0; j < 30; ++j)
  {
   gSensor[i].dataPointArr[j].timestamp = 0;
   gSensor[i].dataPointArr[j].value = 0;
  }
  gSensor[i].arrHead = 0;
  gSensor[i].arrTail = 0;
  gSensor[i].dataCount = 0;
# 228 "sensors.c"
  gSensor[i].filterAcc = 0;
  gSensor[i].jumpCount = 0;
  gSensor[i].potiCheckVel = false;


  startSensor(i);
 }




}
# 47 "main.c" 2
# 1 "joysticks.c" 1
void setupJoysticks()
{
 for (TVexJoysticks i = (TVexJoysticks)0; i < kNumbOfVexRFIndices; ++i)
 {
  gJoy[i].cur = gJoy[i].lst = 0;
  gJoy[i].deadzone = -1;
  gJoy[i].enabled = false;
  gJoy[i].mirror = -1;
 }
}

void updateJoysticks()
{
 for (TVexJoysticks i = (TVexJoysticks)0; i < kNumbOfVexRFIndices; ++i)
  if (gJoy[i].enabled)
   updateJoystick(i);
}

void updateJoystick(TVexJoysticks joy)
{
 gJoy[joy].lst = gJoy[joy].cur;
 short val = vexRT[joy];
 gJoy[joy].cur = abs(val) > gJoy[joy].deadzone ? val : 0;
 if (gJoy[joy].mirror != -1)
  gJoy[gJoy[joy].mirror].cur |= gJoy[joy].cur;
}

void enableJoystick(TVexJoysticks joy)
{
 gJoy[joy].enabled = true;
}

void mirrorJoystick(TVexJoysticks joy, TVexJoysticks mirror)
{
 enableJoystick(mirror);
 gJoy[mirror].mirror = joy;
}
# 48 "main.c" 2
# 1 "cycle.c" 1

void initCycle(sCycleData& data, unsigned long period, const string name)
{
 data.period = period;
 data.startTime = nPgmTime;
 data.count = 0;
 data.time = 0;
 data.name = name;
}

void endCycle(sCycleData& data)
{
 ++data.count;


 unsigned long now = nPgmTime;
 data.time = now - data.startTime;
 if (data.time <= data.period)
  sleep(data.period - data.time);
 data.startTime = nPgmTime;
}
# 49 "main.c" 2
# 1 "utilities.c" 1
float fmod(float x, float y)
{
 int q = floor(x / y);
 return x - (float)q * y;
}

float degToRad(float degrees)
{
 return degrees * PI / 180;
}

float radToDeg(float radians)
{
 return radians * 180 / PI;
}

float nearAngle(float angle, float reference)
{
 return round((reference - angle) / (2 * PI)) * (2 * PI) + angle;
}

float simplifyAngle(float angle)
{
 float baseAng = fmod(gPosition.a,PI*2);
 return (baseAng > PI)? (baseAng - PI*2) : baseAng;
}

void stopAllButCurrentTasks()
{

 tHog();
 if (1 != nCurrentTask) stopTask(1);
 if (2 != nCurrentTask) stopTask(2);
 if (3 != nCurrentTask) stopTask(3);
 if (4 != nCurrentTask) stopTask(4);
 if (5 != nCurrentTask) stopTask(5);
 if (6 != nCurrentTask) stopTask(6);
 if (7 != nCurrentTask) stopTask(7);
 if (8 != nCurrentTask) stopTask(8);
 if (9 != nCurrentTask) stopTask(9);
 if (10 != nCurrentTask) stopTask(10);
 if (11 != nCurrentTask) stopTask(11);
 if (12 != nCurrentTask) stopTask(12);
 if (13 != nCurrentTask) stopTask(13);
 if (14 != nCurrentTask) stopTask(14);
 if (15 != nCurrentTask) stopTask(15);
 if (16 != nCurrentTask) stopTask(16);
 if (17 != nCurrentTask) stopTask(17);
 if (18 != nCurrentTask) stopTask(18);
 if (19 != nCurrentTask) stopTask(19);
 if (20 != nCurrentTask) stopTask(20);
 tRelease();
}


void startTaskID(word id)
{
 if (id == nCurrentTask)
  _hogLevel = 0;
 switch (id)
 {
  case 0:
   writeDebugStreamLine("WARNING, MAIN TASK BEING STARTED AGAIN, ABORTING");
   break;
  case 1: startTask(1); break;
  case 2: startTask(2); break;
  case 3: startTask(3); break;
  case 4: startTask(4); break;
  case 5: startTask(5); break;
  case 6: startTask(6); break;
  case 7: startTask(7); break;
  case 8: startTask(8); break;
  case 9: startTask(9); break;
  case 10: startTask(10); break;
  case 11: startTask(11); break;
  case 12: startTask(12); break;
  case 13: startTask(13); break;
  case 14: startTask(14); break;
  case 15: startTask(15); break;
  case 16: startTask(16); break;
  case 17: startTask(17); break;
  case 18: startTask(18); break;
  case 19: startTask(19); break;
  case 20: startTask(20); break;
 }
}



void stopTaskID(word id)
{
 if (id == nCurrentTask)
  _hogLevel = 0;
 switch (id)
 {
  case 0:
   writeDebugStreamLine("WARNING, MAIN TASK BEING STOPPED, ABORTING");
   break;
  case 1: stopTask(1); break;
  case 2: stopTask(2); break;
  case 3: stopTask(3); break;
  case 4: stopTask(4); break;
  case 5: stopTask(5); break;
  case 6: stopTask(6); break;
  case 7: stopTask(7); break;
  case 8: stopTask(8); break;
  case 9: stopTask(9); break;
  case 10: stopTask(10); break;
  case 11: stopTask(11); break;
  case 12: stopTask(12); break;
  case 13: stopTask(13); break;
  case 14: stopTask(14); break;
  case 15: stopTask(15); break;
  case 16: stopTask(16); break;
  case 17: stopTask(17); break;
  case 18: stopTask(18); break;
  case 19: stopTask(19); break;
  case 20: stopTask(20); break;
 }
}
# 50 "main.c" 2


# 1 "Vex_Competition_Includes_Custom.c" 1
# 35 "Vex_Competition_Includes_Custom.c"
void allMotorsOff();
void allTasksStop();
void disabled();
void startup();
task autonomous();
task usercontrol();

bool bStopTasksBetweenModes = false;
word gMainTask = 255;

task main()
{

 clearLCDLine(0);
 clearLCDLine(1);
 displayLCDPos(0, 0);
 displayNextLCDString("Startup");
 wait1Msec(2000);

 startup();



 while (true)
 {
  while (bIfiRobotDisabled) { disabled(); wait1Msec(25); }

  if (bIfiAutonomousMode)
  {
   startTask(autonomous);


   while (bIfiAutonomousMode && !bIfiRobotDisabled)
   {
    if (!bVEXNETActive)
    {
     if (nVexRCReceiveState == vrNoXmiters)
      allMotorsOff();
    }
    wait1Msec(25);
   }
   allMotorsOff();
   if(bStopTasksBetweenModes)
   {
    allTasksStop();
   }
  }

  else
  {
   startTask(usercontrol);



   while (!bIfiAutonomousMode && !bIfiRobotDisabled)
   {
    if (nVexRCReceiveState == vrNoXmiters)
     allMotorsOff();
    wait1Msec(25);
   }
   allMotorsOff();
   if(bStopTasksBetweenModes)
   {
    allTasksStop();
   }
  }
 }
}

void allMotorsOff()
{
 motor[port1] = 0;
 motor[port2] = 0;
 motor[port3] = 0;
 motor[port4] = 0;
 motor[port5] = 0;
 motor[port6] = 0;
 motor[port7] = 0;
 motor[port8] = 0;




}

void allTasksStop()
{
 tStop(1);
 tStop(2);
 tStop(3);
 tStop(4);
# 143 "Vex_Competition_Includes_Custom.c"
}
# 53 "main.c" 2
# 1 "controls.h" 1
# 54 "main.c" 2


void setDrive(word left, word right)
{

 gMotor[driveL1].power = gMotor[driveL2].power = (abs(left) > (127) ? (127) * sgn(left) : (left));
 gMotor[driveR1].power = gMotor[driveR2].power = (abs(right) > (127) ? (127) * sgn(right) : (right));
}

const int driveStateCount = 3; typedef enum _tStatesdrive { driveIdle, driveBreak, driveManual, drivestate3, drivestate4 }tStatesdrive; tStatesdrive driveState = driveIdle; float driveVelSafetyThresh = -1; tVelDir driveVelSafetyDir = -1; unsigned long driveTimeout; float driveVel; int drivePower; int driveVelSafetyCount = 0; unsigned long driveStateStartTime = 0; unsigned long driveStateCycCount = 0; bool driveLogs = 0; void driveStateChange(int stateIn, long timeout = -1, float velSafetyThresh = -1, tVelDir velDir = -1, float arg1In = -1, int arg2In = -1) { if (driveState != stateIn) { unsigned long curTime = npgmtime; if (timeout <= 0) { driveTimeout = 0; } else { driveTimeout = ( timeout + curTime ); } driveVelSafetyCount = 0; driveStateStartTime = curTime; driveStateCycCount = 0; driveVelSafetyThresh = velSafetyThresh; driveVelSafetyDir = velDir; driveState = stateIn; driveVel = arg1In; drivePower = arg2In; writeDebugStreamLine ("%d" "drive" "State:%d, TO:%d velS:%f, %d, %d", npgmTime, driveState, drivetimeout, driveVelSafetyThresh, driveVel, drivePower); } } void driveVelSafetyCheck (tVelType velType = velSensor) { if (driveVelSafetyThresh != -1 && driveVelSafetyThresh != 0) { if (driveVelSafetyDir == velEither || driveVelSafetyDir == velUp) driveVelSafetyThresh = abs(driveVelSafetyThresh); else if (driveVelSafetyDir == velDown) driveVelSafetyThresh = -1 * abs(driveVelSafetyThresh); tHog(); float out = 0; bool goodVel = false; switch (velType) { case velSensor: { velocityCheck(trackL); out = gSensor[trackL].velocity; goodVel = true; break; } case velLocalY: { out = ( gVelocity.x * (float) sin(gPosition.a) )+ (gVelocity.y * (float) cos(gPosition.a)); if(driveLogs) writeDebugStreamLine("%d:""drive""velSafety out locY= %f", npgmtime, out); goodVel = true; break; } case velAngle: { out = gVelocity.a; goodVel = true; break; } } unsigned long curTime = npgmTime; if (goodVel && curTime-driveStateStartTime > 75) { if (driveVelSafetyDir == velEither) { if ( abs(out) < abs(driveVelSafetyThresh) ) { driveVelSafetyCount ++; if(driveLogs) writeDebugStreamLine("%d:""drive""velSafety trip(either)%f", npgmtime, out); } } else { if ( (sgn(driveVelSafetyThresh) == 1)? (out < driveVelSafetyThresh) : (out > driveVelSafetyThresh) ) { driveVelSafetyCount ++; if(driveLogs) writeDebugStreamLine("%d:""drive""velSafety trip(dir)%f", npgmtime, out); } } } } } void driveSafetyCheck(int timedOutState = driveIdle, float driveVel = -1, int drivePower = -1) { bool timedOut = false; bool velSafety = false; if (!( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) )) timedOut = true; if (driveVelSafetyCount >= 10) velSafety = true; if (velSafety || timedOut) { writeDebugStreamLine("%d" "drive" "safety: Timedout? %d at %d VelSafety? %d", npgmTime, timedout, driveTimeout, velSafety); driveStateChange(timedOutState, driveVel, drivePower); } }

# 1 "auto.c" 1

void trackPosition(int left, int right, int back, sPos& position)
{
 tHog();
 float L = (left - position.leftLst) * (2.783 * PI / 360.0);
 float R = (right - position.rightLst) * (2.783 * PI / 360.0);
 float S = (back - position.backLst) * (2.783 * PI / 360.0);

 float LTurn = (left - position.leftStart) * (2.783 * PI / 360.0);
 float RTurn = (right - position.rightStart) * (2.783 * PI / 360.0);


 position.leftLst = left;
 position.rightLst = right;
 position.backLst = back;

 float h;
 float i;
 float h2;
 float a = (L - R) / (4.766 + 4.766);
 tRelease();

 if (a)
 {
  float r = R / a;
  i = a / 2.0;
  float sinI = sin(i);
  h = ((r + 4.766) * sinI) * 2.0;

  float r2 = S / a;
  h2 = ((r2 + 7) * sinI) * 2.0;
 }
 else
 {
  h = R;
  i = 0;

  h2 = S;
 }
 float p = i + position.a;
 float cosP = cos(p);
 float sinP = sin(p);


 position.y += h * cosP;
 position.x += h * sinP;

 position.y += h2 * -sinP;
 position.x += h2 * cosP;

 position.a = position.aStart + ( ((float)(LTurn - RTurn)) / ((float)(4.766 + 4.766)) );
}

void resetPosition(sPos& position)
{
 position.leftLst = position.rightLst = position.backLst = 0;
 position.y = position.x = position.a = 0;
}

void resetVelocity(sVel& velocity, sPos& position)
{
 velocity.a = velocity.y = velocity.x = 0;

 velocity.lstPosA = position.a;
 velocity.lstPosY = position.y;
 velocity.lstPosX = position.x;

 velocity.lstChecked = nPgmTime;
}

void trackVelocity(sPos position, sVel& velocity)
{
 unsigned long curTime = nPgmTime;
 long passed = curTime - velocity.lstChecked;
 if (passed > 40)
 {
  float posA = position.a;
  float posY = position.y;
  float posX = position.x;
  velocity.a = ((posA - velocity.lstPosA) * 1000.0) / passed;
  velocity.y = ((posY - velocity.lstPosY) * 1000.0) / passed;
  velocity.x = ((posX - velocity.lstPosX) * 1000.0) / passed;

  velocity.localY = (velocity.x * (float) sin(position.a) )+ (velocity.y * (float) cos(position.a);

  velocity.lstPosA = posA;
  velocity.lstPosY = posY;
  velocity.lstPosX = posX;
  velocity.lstChecked = curTime;
 }
}

task trackPositionTask()
{
 while (true)
 {
  updateSensorInput(trackL);
  updateSensorInput(trackR);
  updateSensorInput(trackB);
  trackPosition(gSensor[trackL].value, gSensor[trackR].value, gSensor[trackB].value, gPosition);
  trackVelocity(gPosition, gVelocity);
  sleep(1);
 }
}

void resetPositionFull(sPos& position, float x, float y, float a)
{
 tStop(trackPositionTask);
 writeDebugStreamLine("Resetting position %f %f %f | %f %f %f", position.y, position.x, radToDeg(fmod(gPosition.a, PI * 2)), y, x, radToDeg(fmod(a, PI * 2)));
 resetPosition(position);

 resetQuadratureEncoder(trackL);
 resetQuadratureEncoder(trackR);
 resetQuadratureEncoder(trackB);

 position.leftStart = gSensor[trackL].value;
 position.rightStart = gSensor[trackR].value;
 position.backStart = gSensor[trackB].value;

 position.y = y;
 position.x = x;
 position.aStart = a;

 tStart(trackPositionTask);
}


void vectorToPolar(sVector& vector, sPolar& polar)
{
 if (vector.x || vector.y)
 {
  polar.magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
  polar.angle = atan2(vector.y, vector.x);
 }
 else
  polar.magnitude = polar.angle = 0;
}

void polarToVector(sPolar& polar, sVector& vector)
{
 if (polar.magnitude)
 {
  vector.x = polar.magnitude * cos(polar.angle);
  vector.y = polar.magnitude * sin(polar.angle);
 }
 else
  vector.x = vector.y = 0;
}


void makeLine(sLine& line)
{
 line.m = (line.p2.y - line.p1.y) / (line.p2.x - line.p1.x);
 line.b = line.p2.y - (line.m * line.p2.x);
}

float getAngleOfLine(sLine line)
{
 return atan2(line.p2.x - line.p1.x, line.p2.y - line.p1.y);
}

float getLengthOfLine(sLine line)
{
 float x = line.p2.x - line.p1.x;
 float y = line.p2.y - line.p1.y;
 return sqrt(x * x + y * y);
}


void offsetPos(float& x, float& y, float offset)
{
 x = offset * sin(gPosition.a);
 y = offset * cos(gPosition.a);

 x += gPosition.x;
 y += gPosition.y;
}

byte facingCoord(float targX, float targY, float offset)
{
 float curX = gPosition.x;
 float curY = gPosition.y;
 float curA = simplifyAngle(gPosition.a);
 float gAngleToFront = aTan2((targX-curX),(targY-curY));
 float gAngleToBack = aTan2((curX-targX),(curY-targY));

 writeDebugStreamLine("(%f,%f)RobotPos%f, PosTo%f", gPosition.x, gPosition.y, gPosition.a, gAngleToFront);

 if ( abs( (curA-gAngleToFront) ) < offset )
 {
  writeDebugStreamLine("Front facing target (%f, %f)", targX, targY);
  return facingFront;
 }
 else if ( abs( (curA-gAngleToBack) ) < offset )
 {
  writeDebugStreamLine("Back facing target (%f, %f)", targX, targY);
  return facingBack;
 }
 else
 {
  writeDebugStreamLine("MOVEMENT ERROR - WRONG DIR. RobotPos%f, PosTo%f", gPosition.a, gAngleToFront);
  return facingNone;
 }
}

task autoMotorSensorUpdateTask()
{
 sCycleData cycle;
 initCycle(cycle, 10, "auto motor/sensor");
 while (true)
 {
  updateMotors();
  updateSensorInputs();
  updateSensorOutputs();

  if (-1 != -1)
  {
   tHog();
   datalogDataGroupStart();
   datalogAddValue(-1 + 0, nImmediateBatteryLevel);
   datalogAddValue(-1 + 2, BackupBatteryLevel);
   datalogDataGroupEnd();
   tRelease();
  }

  endCycle(cycle);
 }
}

void applyHarshStop()
{
 sVector vel;
 vel.x = gVelocity.x;
 vel.y = gVelocity.y;
 sPolar polarVel;
 vectorToPolar(vel, polarVel);
 polarVel.angle += gPosition.a;
 polarToVector(polarVel, vel);
 float yPow = vel.y, aPow = gVelocity.a;

 writeDebugStreamLine("Vel y | a: %f | %f", yPow, aPow);

 yPow *= -0.7;
 aPow *= -6.3;

 word left = yPow + aPow;
 word right = yPow - aPow;

 left = sgn(left) * ((fabs(left)) > (7) ? (fabs(left)) : (7));
 right = sgn(right) * ((fabs(right)) > (7) ? (fabs(right)) : (7));

 left = (abs(left) > (30) ? (30) * sgn(left) : (left));
 right = (abs(right) > (30) ? (30) * sgn(right) : (right));

 writeDebugStreamLine("Applying harsh stop: %d %d", left, right);
 setDrive(left, right);
 updateMotors();

 unsigned long startTime = npgmtime;
 while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((npgmtime-startTime) < 150) )
  sleep(10);

 setDrive(0, 0);
 updateMotors();
}
# 66 "main.c" 2
# 1 "drive_algs.h" 1




typedef enum _stopType
{
 stopNone = 0b00000000,
 stopSoft = 0b00000001,
 stopHarsh = 0b00000010
} tStopType;

typedef enum _mttMode
{
 mttSimple,
 mttProportional,
 mttCascading
} tMttMode;


sVector gTargetLast;


void followLine(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType);
void moveToTargetSimple(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop);
void moveToTarget(float x, float y, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);
void moveToTargetDis(float a, float d, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);


void turnToFace(float x, float y, tFacingDir facingDir = facingFront, tStopType stopType);
void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true);
void turnToTargetNewAlg(float x, float y, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true, float offset = 0);
void sweepTurnToTarget(float x, float y, float a, float r, tTurnDir turnDir, byte power, bool slow = true);

typedef enum _tFuncStatesdrive { drivefollowLine = driveStateCount, drivemoveToTargetSimple, drivemoveToTarget, drivemoveToTargetDis, driveturnToFace, driveturnToAngleNewAlg, driveturnToTargetNewAlg, drivesweepTurnToTarget }tFuncStatesdrive; const int followLineLoc = driveStateCount + 0; const int moveToTargetSimpleLoc = driveStateCount + 1; const int moveToTargetLoc = driveStateCount + 2; const int moveToTargetDisLoc = driveStateCount + 3; const int turnToFaceLoc = driveStateCount + 4; const int turnToAngleNewAlgLoc = driveStateCount + 5; const int turnToTargetNewAlgLoc = driveStateCount + 6; const int sweepTurnToTargetLoc = driveStateCount + 7;

const int followLineArgCount = 6; float followLineArg1 = -1; float followLineArg2 = -1; byte followLineArg3 = -1; tMttMode followLineArg4 = -1; bool followLineArg5 = -1; tStopType followLineArg6 = -1;
const int moveToTargetSimpleArgCount = 6; float moveToTargetSimpleArg1 = -1; float moveToTargetSimpleArg2 = -1; byte moveToTargetSimpleArg3 = -1; tMttMode moveToTargetSimpleArg4 = -1; bool moveToTargetSimpleArg5 = -1; bool moveToTargetSimpleArg6 = -1;
const int moveToTargetArgCount = 12; float moveToTargetArg1 = -1; float moveToTargetArg2 = -1; float moveToTargetArg3 = -1; float moveToTargetArg4 = -1; byte moveToTargetArg5 = -1; byte moveToTargetArg6 = -1; float moveToTargetArg7 = -1; float moveToTargetArg8 = -1; byte moveToTargetArg9 = -1; float moveToTargetArg10 = -1; tStopType moveToTargetArg11 = -1; tMttMode moveToTargetArg12 = -1;
const int moveToTargetDisArgCount = 12; float moveToTargetDisArg1 = -1; float moveToTargetDisArg2 = -1; float moveToTargetDisArg3 = -1; float moveToTargetDisArg4 = -1; byte moveToTargetDisArg5 = -1; byte moveToTargetDisArg6 = -1; float moveToTargetDisArg7 = -1; float moveToTargetDisArg8 = -1; byte moveToTargetDisArg9 = -1; float moveToTargetDisArg10 = -1; tStopType moveToTargetDisArg11 = -1; tMttMode moveToTargetDisArg12 = -1;
const int turnToFaceArgCount = 4; float turnToFaceArg1 = -1; float turnToFaceArg2 = -1; tFacingDir turnToFaceArg3 = -1; tStopType turnToFaceArg4 = -1;
const int turnToAngleNewAlgArgCount = 7; float turnToAngleNewAlgArg1 = -1; tTurnDir turnToAngleNewAlgArg2 = -1; float turnToAngleNewAlgArg3 = -1; byte turnToAngleNewAlgArg4 = -1; float turnToAngleNewAlgArg5 = -1; bool turnToAngleNewAlgArg6 = -1; bool turnToAngleNewAlgArg7 = -1;
const int turnToTargetNewAlgArgCount = 9; float turnToTargetNewAlgArg1 = -1; float turnToTargetNewAlgArg2 = -1; tTurnDir turnToTargetNewAlgArg3 = -1; float turnToTargetNewAlgArg4 = -1; byte turnToTargetNewAlgArg5 = -1; float turnToTargetNewAlgArg6 = -1; bool turnToTargetNewAlgArg7 = -1; bool turnToTargetNewAlgArg8 = -1; float turnToTargetNewAlgArg9 = -1;
const int sweepTurnToTargetArgCount = 7; float sweepTurnToTargetArg1 = -1; float sweepTurnToTargetArg2 = -1; float sweepTurnToTargetArg3 = -1; float sweepTurnToTargetArg4 = -1; tTurnDir sweepTurnToTargetArg5 = -1; byte sweepTurnToTargetArg6 = -1; bool sweepTurnToTargetArg7 = -1;
# 67 "main.c" 2
# 1 "drive_algs.c" 1

void followLine(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType)
{
 byte facingDir = facingCoord(x,y,(pi/4));

 if (facingDir)
 {
  sVector currentLocalVector;
  if ((gPosition.x - x) == 0)
   correction = 0;


  word throttle, turn, left, right;
  byte dir;


  const float propKP = 6.0;
  float softExit = 3;


  sVector offsetGlobalVector;
  float offset = 5.0;
  sLine followLine;
  if (correction)
  {
   followLine.p1.x = gPosition.x;
   followLine.p1.y = gPosition.y;
   followLine.p2.x = x;
   followLine.p2.y = y;
   makeLine(followLine);
   writeDebugStreamLine("%d Constructing line to follow: y=%fx+%f", npgmtime, followLine.m, followLine.b);
  }

  do
  {
   driveVelSafetyCheck(velLocalY); driveStateCycCount++;
# 48 "drive_algs.c"
   currentLocalVector.x = gPosition.x - x;
   currentLocalVector.y = gPosition.y - y;

   switch (mode)
   {
    case mttSimple:
    {
     if (abs(currentLocalVector.y) > 3)
      throttle = abs(power) * facingDir;
     else
      throttle = 7 * facingDir;
     break;
    }
    case mttProportional:
    {
     throttle = (abs((currentLocalVector.y * propKP)) > (127) ? (127) * sgn((currentLocalVector.y * propKP)) : ((currentLocalVector.y * propKP)));
     if (abs(currentLocalVector.y) < 5)
      throttle = (abs(throttle) > (15) ? (15) * sgn(throttle) : (throttle));
     break;
    }
   }
   throttle = abs(throttle) * facingDir;
   if(driveLogs) writeDebugStreamLine("\t Facing%d, Drive throttle mode:%d, Pwr%f", facingDir, mode, throttle);

   if (correction)
   {
    const float turnBase = 1.42;

    offsetPos(offsetGlobalVector.x, offsetGlobalVector.y, offset);
    if (abs(currentLocalVector.y) <= (offset+2))
    {
     offset = abs(currentLocalVector.y) - softExit;
     if(driveLogs) writeDebugStreamLine("\t\t Offset Reset - %f", offset);
    }

    float targX = ( ((float)offsetGlobalVector.y - followLine.b) / followLine.m );
    float errorX = fabs(offsetGlobalVector.x - targX);


    if (fabs(errorX) <= 1)
     turn = 0;
    else
     turn = (abs(((float)5.5 * (exp(0.2 * errorX)))) > (127) ? (127) * sgn(((float)5.5 * (exp(0.2 * errorX)))) : (((float)5.5 * (exp(0.2 * errorX)))));

    turn *= facingDir;

    if (abs(errorX) > 6 && abs(throttle) > 65)
     throttle /= 2;

    byte dir = sgn(currentLocalVector.x) * sgn(currentLocalVector.y) * facingDir;
    switch(dir)
    {
     case (-1):
     {

      left = throttle;
      right = throttle + turn;
      break;
     }
     case(1):
     {

      right = throttle;
      left = throttle + turn;
      break;
     }
     case(0):
     {

      right = left = throttle;
      break;
     }
    }

   if(driveLogs) writeDebugStreamLine("%d LocalPos:(%f,%f), targ:%f - projx:%f = error%f, vel:%f, %d, %d, t:%f l:%d r:%d, trttle:%d, trn:%d",npgmtime, currentLocalVector.x, currentLocalVector.y, targX, offsetGlobalVector.x, errorX, gVelocity.localY, facingDir, dir, errorX, left, right, throttle, turn);
   }
   else
   {
    left = right = throttle;
   }

   left = (abs(left) > (127) ? (127) * sgn(left) : (left));
   right = (abs(right) > (127) ? (127) * sgn(right) : (right));

   setDrive(left,right);

   sleep(10);
  } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && (( abs(currentLocalVector.y) > ((stopType & stopSoft)? softExit : 0.8) )) );

  if(driveLogs) writeDebugStreamLine("%d Done LineFollow(%f, %f)", npgmtime, gPosition.x, gPosition.y);

  if (stopType & stopSoft)
  {
   currentLocalVector.x = gPosition.x - x;
   currentLocalVector.y = gPosition.y - y;

   if(driveLogs) writeDebugStreamLine("%d Starting LineFollow stopSoft(%f,%f), vel:%f", npgmtime, gPosition.x, gPosition.y, gVelocity.localY);

   while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && (abs(currentLocalVector.y) > 0.6 && abs(gVelocity.localY) > 0.3) )
   {
    throttle = facingDir * -6;
    setDrive(throttle, throttle);
    if(driveLogs) writeDebugStreamLine("%d LocalPos:(%f,%f), %d, %d, pow:%d",npgmtime, currentLocalVector.x, currentLocalVector.y, facingDir, dir, throttle);

    sleep(10);
   }
  }

  if(driveLogs) writeDebugStreamLine("%d Done LineFollow stopSoft(%f, %f)", npgmtime, gPosition.x, gPosition.y);

  if (stopHarsh & stopType)
   applyHarshStop();
  else
   setDrive(0,0);

  if(driveLogs) writeDebugStreamLine("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
 }
}

void turnToFace(float x, float y, tFacingDir facingDir, tStopType stopType)
{
 float curX = gPosition.x;
 float curY = gPosition.y;
 float gAngleToFront = aTan2((x-curX),(y-curY));
 float gAngleToBack = aTan2((curX-x),(curY-y));

 if(driveLogs) writeDebugStreamLine("%d Begin turnToFace(%f,%f)", npgmtime, x, y);

 tTurnDir turnDir;
 if (facingDir == facingFront)
  if (fmod(gPosition.a - gAngleToFront, PI * 2) < PI) turnDir = ccw; else turnDir = cw;
 else if (facingDir == facingBack)
  if (fmod(gPosition.a - gAngleToBack, PI * 2) < PI) turnDir = ccw; else turnDir = cw;
 else
  if(driveLogs) writeDebugStreamLine("%d Exit turnToFace. facingDir must be facingFront or facingBack", npgmtime);

 if (turnDir == cw)
  setDrive(90, -90);
 else
  setDrive(90, 90);

 while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((facingCoord(x, y, 0.2) != facingDir)) )
 {
  sleep(10);
 }

 if (stopType & stopHarsh)
  applyHarshStop();
 else
  setDrive(0,0);
}

void moveToTargetSimple(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop)
{
 byte facingDir = facingCoord(x,y,(pi/4))
 if (facingDir)
 {
  power = abs(power) * facingDir;


  word throttle, turn, left, right;
  byte dir;

  sVector currentLocalVector;
  sPolar currentLocalPolar;

  sVector turnGlobalVector;
  sVector turnLocalVector;
  sPolar turnLocalPolar;


  const float propKP = 6.0;


  float offset = 4.0;
  const float turnBase = 1.42;
  const float turnKP = -5.0;

  do
  {
   driveVelSafetyCheck(velLocalY); driveStateCycCount++;

   currentLocalVector.x = gPosition.x - x;
   currentLocalVector.y = gPosition.y - y;

   switch (mode)
   {
    case mttSimple:
    {
     if(driveLogs) writeDebugStreamLine("\t simple drive throttle");
     if (abs(currentLocalVector.y) > 3)
      throttle = abs(power) * facingDir;
     else
      throttle = 7 * facingDir;
     break;
    }
    case mttProportional:
    {
     throttle = (abs((currentLocalVector.y * propKP)) > (127) ? (127) * sgn((currentLocalVector.y * propKP)) : ((currentLocalVector.y * propKP)));
     if(driveLogs) writeDebugStreamLine("\t prop drive throttle pwr%f", throttle);
     if (abs(currentLocalVector.y) < 5)
      throttle = (abs(throttle) > (15) ? (15) * sgn(throttle) : (throttle));
     break;
    }
   }

   throttle = abs(throttle) * facingDir;




   if (correction)
   {

    if (abs(currentLocalVector.y) <= offset)
     offset = abs(currentLocalVector.y) - 0.2;

    offsetPos(turnGlobalVector.x, turnGlobalVector.y, offset);

    turnLocalVector.x = turnGlobalVector.x - x;
    turnLocalVector.y = turnGlobalVector.y - y;

    if (abs(turnLocalVector.x) > 8 && abs(throttle) > 62)
     throttle /= 2;

    if (fabs(turnLocalVector.x) > 2 && fabs(turnLocalVector.x) < 7)
     turn = (abs((fabs(turnLocalVector.x) * 4)) > (127) ? (127) * sgn((fabs(turnLocalVector.x) * 4)) : ((fabs(turnLocalVector.x) * 4))) * facingDir;
    else
     turn = (abs(pow(turnBase, fabs(turnLocalVector.x))) > (127) ? (127) * sgn(pow(turnBase, fabs(turnLocalVector.x))) : (pow(turnBase, fabs(turnLocalVector.x)))) * facingDir;


    dir = sgn(turnLocalVector.x) * sgn(turnLocalVector.y) * facingDir;





    switch(dir)
    {
     case (-1):
     {
      left = throttle;
      right = throttle + turn;
      break;
     }
     case(1):
     {
      right = throttle;
      left = throttle + turn;
      break;
     }
     case(0):
     {
      right = left = throttle;
      break;
     }
    }
   }
   else
   {
    left = right = throttle;
   }

   left = (abs(left) > (127) ? (127) * sgn(left) : (left));
   right = (abs(right) > (127) ? (127) * sgn(right) : (right));

   setDrive(left,right);

   if(driveLogs) writeDebugStreamLine("loc coord:(%f,%f), %d, %d, t:%f l:%d r:%d, throttle:%d, turn:%d", currentLocalVector.x, currentLocalVector.y, facingDir, dir, turnLocalVector.x, left, right, throttle, turn);

   sleep(10);
  } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((abs(currentLocalVector.y) > 0.8)) );
  if(driveLogs) writeDebugStreamLine("%d (%f, %f)", npgmtime, gPosition.x, gPosition.y);

  if (harshStop)
   applyHarshStop();

  if(driveLogs) writeDebugStreamLine("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
 }
 else
 {
  if(driveLogs) writeDebugStreamLine("%d Move Simple Exit - Not Facing", npgmtime);
 }
}

void moveToTarget(float x, float y, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
 if(driveLogs) writeDebugStreamLine("Moving to %f %f from %f %f at %d", y, x, ys, xs, power);

 gTargetLast.y = y;
 gTargetLast.x = x;


 sLine followLine;


 followLine.p1.y = ys;
 followLine.p1.x = xs;


 followLine.p2.y = y;
 followLine.p2.x = x;

 float lineLength = getLengthOfLine(followLine);
 if(driveLogs) writeDebugStreamLine("Line length: %.2f", lineLength);
 float lineAngle = getAngleOfLine(followLine);
 float pidAngle = nearAngle(lineAngle - (power < 0 ? PI : 0), gPosition.a);
 if(driveLogs) writeDebugStreamLine("Line | Pid angle: %f | %f", radToDeg(lineAngle), radToDeg(pidAngle));


 sVector currentPosVector;
 sPolar currentPosPolar;

 sCycleData cycle;
 initCycle(cycle, 10, "moveToTarget");

 float vel;
 float _sin = sin(lineAngle);
 float _cos = cos(lineAngle);

 word last = startPower;
 float correction = 0;

 if (mode == mttSimple)
  setDrive(power, power);

 word finalPower = power;

 unsigned long timeStart = nPgmTime;
 do
 {
  driveVelSafetyCheck(velLocalY); driveStateCycCount++;

  currentPosVector.x = gPosition.x - x;
  currentPosVector.y = gPosition.y - y;
  vectorToPolar(currentPosVector, currentPosPolar);
  currentPosPolar.angle += lineAngle;
  polarToVector(currentPosPolar, currentPosVector);

  if (maxErrX)
  {
   float errA = gPosition.a - pidAngle;
   float errX = currentPosVector.x + currentPosVector.y * sin(errA) / cos(errA);
   float correctA = atan2(x - gPosition.x, y - gPosition.y);
   if (power < 0)
    correctA += PI;
   correction = fabs(errX) > maxErrX ? 8.0 * (nearAngle(correctA, gPosition.a) - gPosition.a) * sgn(power) : 0;
  }

  if (mode != mttSimple)
  {
   switch (mode)
   {
   case mttProportional:
    finalPower = round(-127.0 / 40.0 * currentPosVector.y) * sgn(power);
    break;
   case mttCascading:
   const float kB = 2.8;
   const float kP = 2.0;
# 425 "drive_algs.c"
    float vTarget = 45 * (1 - exp(0.07 * (currentPosVector.y + dropEarly)));
    finalPower = round(kB * vTarget + kP * (vTarget - vel)) * sgn(power);
    break;
   }
   finalPower = (abs(finalPower) > (abs(power)) ? (abs(power)) * sgn(finalPower) : (finalPower));
   if (finalPower * sgn(power) < 30)
    finalPower = 30 * sgn(power);
   word delta = finalPower - last;
   delta = (abs(delta) > (5) ? (5) * sgn(delta) : (delta));
   finalPower = last += delta;
  }

  switch (sgn(correction))
  {
  case 0:
   setDrive(finalPower, finalPower);
   break;
  case 1:
   setDrive(finalPower, finalPower * exp(-correction));
   break;
  case -1:
   setDrive(finalPower * exp(correction), finalPower);
   break;
  }

  vel = _sin * gVelocity.x + _cos * gVelocity.y;

  endCycle(cycle);
 } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((currentPosVector.y < -dropEarly - (((vel * ((stopType & stopSoft) ? 0.175 : 0.098))) > (decelEarly) ? ((vel * ((stopType & stopSoft) ? 0.175 : 0.098))) : (decelEarly)))) );

 if(driveLogs) writeDebugStreamLine("%f %f", currentPosVector.y, vel);

 setDrive(decelPower, decelPower);

 do
 {
  currentPosVector.x = gPosition.x - x;
  currentPosVector.y = gPosition.y - y;
  vectorToPolar(currentPosVector, currentPosPolar);
  currentPosPolar.angle += lineAngle;
  polarToVector(currentPosPolar, currentPosVector);

  vel = _sin * gVelocity.x + _cos * gVelocity.y;

  endCycle(cycle);
 } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((currentPosVector.y < -dropEarly - (vel * ((stopType & stopSoft) ? 0.175 : 0.098)))) );

 if (stopType & stopSoft)
 {
  setDrive(-6 * sgn(power), -6 * sgn(power));
  do
  {
   currentPosVector.x = gPosition.x - x;
   currentPosVector.y = gPosition.y - y;
   vectorToPolar(currentPosVector, currentPosPolar);
   currentPosPolar.angle += lineAngle;
   polarToVector(currentPosPolar, currentPosVector);

   vel = _sin * gVelocity.x + _cos * gVelocity.y;

   endCycle(cycle);
  } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((vel > 7 && currentPosVector.y < 0)) );
 }

 if (stopType & stopHarsh)
  applyHarshStop();
 else
  setDrive(0, 0);

 if(driveLogs) writeDebugStreamLine("Moved to %f %f from %f %f | %f %f %f", y, x, ys, xs, gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void moveToTargetDis(float a, float d, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
 moveToTarget(xs + d * sin(a), ys + d * cos(a), xs, ys, power, startPower, maxErrX, decelEarly, decelPower, dropEarly, stopType, mode);
}

void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo, bool harshStop)
{
 if(driveLogs) writeDebugStreamLine("Turning to %f", radToDeg(a));


 if (turnDir == ch)
  if (fmod(a - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

 float endFull;

 unsigned long timeStart = nPgmTime;

 switch (turnDir)
 {
 case cw:
  a = gPosition.a + fmod(a - gPosition.a, PI * 2);
  endFull = gPosition.a * (1 - fullRatio) + a * fullRatio;
  setDrive(127, -127);
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a < endFull)) )
  {
   driveVelSafetyCheck()
   if (-1 != -1)
   {
    driveVelSafetyCheck(velAngle); driveStateCycCount++;
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  setDrive(coastPower, -coastPower);
  timeStart = nPgmTime;
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a < a - degToRad(stopOffsetDeg))) )
  {
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  if(driveLogs) writeDebugStreamLine("Turn done: %d", gPosition.a);
  if (harshStop)
  {
   setDrive(-20, 20);
   sleep(150);
   if(driveLogs) writeDebugStreamLine("Break done: %d", gPosition.a);
  }
  setDrive(0, 0);
  break;
 case ccw:
  a = gPosition.a - fmod(gPosition.a - a, PI * 2);
  endFull = gPosition.a * (1 - fullRatio) + a * fullRatio;
  setDrive(-127, 127);
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a > endFull)) )
  {
   driveVelSafetyCheck(velAngle); driveStateCycCount++;
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  setDrive(-coastPower, coastPower);
  timeStart = npgmTime;
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a > a + degToRad(stopOffsetDeg))) )
  {
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  if(driveLogs) writeDebugStreamLine("Turn done: %d", gPosition.a);
  if (harshStop)
  {
   setDrive(20, -20);
   sleep(150);
   if(driveLogs) writeDebugStreamLine("Break done: %d", gPosition.a);
  }
  setDrive(0, 0);
  break;
 }
 if(driveLogs) writeDebugStreamLine("Turned to %f | %f %f %f", radToDeg(a), gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void turnToTargetNewAlg(float x, float y, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo, bool harshStop, float offset)
{
 if(driveLogs) writeDebugStreamLine("Turning to %f %f", y, x);

 if (turnDir == ch)
  if (fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

 float endFull, target;


 unsigned long timeStart = nPgmTime;

 switch (turnDir)
 {
 case cw:
  target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
  endFull = gPosition.a * (1 - fullRatio) + target * fullRatio;
  if(driveLogs) writeDebugStreamLine("%f %f", radToDeg(target), radToDeg(endFull));
  setDrive(127, -127);
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a < endFull )) )
  {
   driveVelSafetyCheck(velAngle); driveStateCycCount++;
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  setDrive(coastPower, -coastPower);
  timeStart = npgmTime;
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a < nearAngle(atan2(x - gPosition.x, y - gPosition.y) + offset, target) - degToRad(stopOffsetDeg))) )
  {
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  if(driveLogs) writeDebugStreamLine("Turn done: %d", gPosition.a);
  if (harshStop)
  {
   setDrive(-20, 20);
   sleep(150);
   if(driveLogs) writeDebugStreamLine("Break done: %d", gPosition.a);
  }
  setDrive(0, 0);
  break;
 case ccw:
  target = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y) - offset, PI * 2);
  endFull = gPosition.a * (1 - fullRatio) + (target) * fullRatio;
  if(driveLogs) writeDebugStreamLine("%f %f", radToDeg(target), radToDeg(endFull));
  setDrive(-127, 127);
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a > endFull)) )
  {
   driveVelSafetyCheck(velAngle); driveStateCycCount++;
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  setDrive(-coastPower, coastPower);
  timeStart = nPgmTime;
  while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && ((gPosition.a > nearAngle(atan2(x - gPosition.x, y - gPosition.y) + offset, target) + degToRad(stopOffsetDeg))) )
  {
   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, radToDeg(gPosition.a));
    datalogAddValue(-1 + 1, 127);
    datalogDataGroupEnd();
    tRelease();
   }
   sleep(10);
  }
  if(driveLogs) writeDebugStreamLine("Turn done: %d", gPosition.a);
  if (harshStop)
  {
   setDrive(20, -20);
   sleep(150);
   if(driveLogs) writeDebugStreamLine("Break done: %d", gPosition.a);
  }
  setDrive(0, 0);
  break;
 }
 if(driveLogs) writeDebugStreamLine("Turned to %f %f | %f %f %f", y, x, gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void sweepTurnToTarget(float x, float y, float a, float r, tTurnDir turnDir, byte power, bool slow)
{
 sVector vector;
 sPolar polar;


 if (turnDir == ch)
 {
  vector.x = gPosition.x - x;
  vector.y = gPosition.y - y;
  vectorToPolar(vector, polar);
  polar.angle += a;
  polarToVector(polar, vector);

  turnDir = vector.x > 0 ? cw : ccw;
 }

 float yOrigin, xOrigin;
 float linearV, angularV, angularVLast = 0;
 float localR, localA;

 const float kR = 15.0;
 const float kA = 5.0;
 const float kB = 60.0;
 const float kP = 30.0;
 const float kD = 2000.0;

 sCycleData cycle;
 initCycle(cycle, 40, "sweepTurnToTarget");

 unsigned long timeStart = nPgmTime;
 switch (turnDir)
 {
 case cw:
  vector.y = 0;
  vector.x = r;
  vectorToPolar(vector, polar);
  polar.angle -= a;
  polarToVector(polar, vector);
  yOrigin = y + vector.y;
  xOrigin = x + vector.x;

  localA = atan2(gPosition.x - xOrigin, gPosition.y - yOrigin);

  a = nearAngle(a, power > 0 ? gPosition.a : (gPosition.a + PI));

  if(driveLogs) writeDebugStreamLine("%d Sweep to %f around %f %f", nPgmTime, radToDeg(a), yOrigin, xOrigin);

  do
  {
   driveVelSafetyCheck(velAngle); driveStateCycCount++;
   float aGlobal = gPosition.a;
   if (power < 0)
    aGlobal += PI;
   angularV = gVelocity.a;
   float _y = gPosition.y - yOrigin;
   float _x = gPosition.x - xOrigin;
   localR = sqrt(_y * _y + _x * _x);
   localA = nearAngle(atan2(_x, _y), localA);
   linearV = gVelocity.x * sin(localA + PI / 2) + gVelocity.y * cos(localA + PI / 2);

   float target = ((linearV) > (15) ? (linearV) : (15)) / localR + kR * log(localR / r) + kA * (nearAngle(localA + PI / 2, aGlobal) - aGlobal);
   word turn = round(kB * target + kP * (target - angularV) + kD * (angularVLast - angularV) / 40);
   angularVLast = angularV;

   if (turn < 0)
    turn = 0;
   else if (turn > 150)
    turn = 150;

   if (power > 0)
    setDrive(power, power - turn);
   else
    setDrive(power + turn, power);

   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, localR * 100);
    datalogAddValue(-1 + 1, radToDeg(localA) * 10);
    datalogAddValue(-1 + 2, radToDeg(target) * 10);
    datalogAddValue(-1 + 3, turn * 10);
    datalogAddValue(-1 + 4, linearV * 10);
    datalogAddValue(-1 + 5, radToDeg(angularV) * 10);
    datalogDataGroupEnd();
    tRelease();
   }

   endCycle(cycle);
  } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && (((power > 0 ? gPosition.a : (gPosition.a + PI)) - a < (slow ? -0.1 : -0.15))) );
  break;
 case ccw:
  vector.y = 0;
  vector.x = r;
  vectorToPolar(vector, polar);
  polar.angle += a;
  polarToVector(polar, vector);
  yOrigin = y + vector.y;
  xOrigin = x + vector.x;

  localA = atan2(gPosition.x - xOrigin, gPosition.y - yOrigin);

  a = nearAngle(a, power > 0 ? gPosition.a : (gPosition.a + PI));

  if(driveLogs) writeDebugStreamLine("%d Sweep to %f around %f %f", nPgmTime, radToDeg(a), yOrigin, xOrigin);

  do
  {
   driveVelSafetyCheck(velAngle); driveStateCycCount++;
   float aGlobal = gPosition.a;
   if (power < 0)
    aGlobal += PI;
   angularV = gVelocity.a;
   float _y = gPosition.y - yOrigin;
   float _x = gPosition.x - xOrigin;
   localR = sqrt(_y * _y + _x * _x);
   localA = nearAngle(atan2(_x, _y), localA);
   linearV = gVelocity.x * sin(localA - PI / 2) + gVelocity.y * cos(localA - PI / 2);

   float target = -((linearV) > (15) ? (linearV) : (15)) / localR + kR * log(r / localR) + kA * (nearAngle(localA - PI / 2, aGlobal) - aGlobal);
   word turn = round(kB * target + kP * (target - angularV) + kD * (angularVLast - angularV) / 40);
   angularVLast = angularV;

   if (turn > 0)
    turn = 0;
   else if (turn < -150)
    turn = -150;

   if (power > 0)
    setDrive(power + turn, power);
   else
    setDrive(power, power - turn);

   if (-1 != -1)
   {
    tHog();
    datalogDataGroupStart();
    datalogAddValue(-1 + 0, localR * 100);
    datalogAddValue(-1 + 1, radToDeg(localA) * 10);
    datalogAddValue(-1 + 2, radToDeg(target) * 10);
    datalogAddValue(-1 + 3, turn * 10);
    datalogAddValue(-1 + 4, linearV * 10);
    datalogAddValue(-1 + 5, radToDeg(angularV) * 10);
    datalogDataGroupEnd();
    tRelease();
   }

   endCycle(cycle);
  } while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && (((power > 0 ? gPosition.a : (gPosition.a + PI)) - a > (slow ? 0.1 : 0.15))) );
  break;
 }
 setDrive(0, 0);
 if(driveLogs) writeDebugStreamLine("%d Done sweep turn", nPgmTime);
}
# 68 "main.c" 2

task driveSet()
{
 bool autoLogs = 1;
 driveLogs = 1;

 sCycleData drive;
 initCycle(drive, 10, "drive");
 while (true)
 {
  switch(driveState)
  {
   case driveIdle:
   {
    setDrive(0,0);

    driveStateCycCount++;
    break;
   }
   case driveBreak:
   {
    float startVelL, startVelR;

    const float velThresh = 0.015;
    velocityCheck(trackL);
    velocityCheck(trackR);
     float left = gSensor[trackL].velocity;
     float right = gSensor[trackR].velocity;
     if (driveStateCycCount == 0)
     {
      startVelL = left;
      startVelR = right;
      if(driveLogs) writeDebugStreamLine("BreakFirstVel %f, %f", startVelL, startVelR);
     }

     if(driveLogs) writeDebugStreamLine("L: %f, r: %f", left, right);
     if (( abs(left) < velThresh || sgn(left)!=sgn(startVelL) ) && ( abs(right) < velThresh || sgn(right)!=sgn(startVelR) ) )
     {
      driveStateChange(driveIdle);
     }
     else
     {
      setDrive((abs(left) > velThresh? ((abs(sgn(left) * -50) > (12) ? (12) * sgn(sgn(left) * -50) : (sgn(left) * -50))) : 0), (abs(right) > velThresh? ((abs(sgn(right) * -50) > (12) ? (12) * sgn(sgn(right) * -50) : (sgn(right) * -50))) : 0) );
     }
     driveStateCycCount++;

    driveSafetyCheck(driveIdle);
    break;
   }
   case driveManual:
   {

    setDrive(((abs((gJoy[Ch3].cur + (gJoy[Ch4].cur - gJoy[Ch4].deadzone * sgn(gJoy[Ch4].cur)))) > (127) ? (127) * sgn((gJoy[Ch3].cur + (gJoy[Ch4].cur - gJoy[Ch4].deadzone * sgn(gJoy[Ch4].cur)))) : ((gJoy[Ch3].cur + (gJoy[Ch4].cur - gJoy[Ch4].deadzone * sgn(gJoy[Ch4].cur)))))), ((abs((gJoy[Ch3].cur - (gJoy[Ch4].cur - gJoy[Ch4].deadzone * sgn(gJoy[Ch4].cur)))) > (127) ? (127) * sgn((gJoy[Ch3].cur - (gJoy[Ch4].cur - gJoy[Ch4].deadzone * sgn(gJoy[Ch4].cur)))) : ((gJoy[Ch3].cur - (gJoy[Ch4].cur - gJoy[Ch4].deadzone * sgn(gJoy[Ch4].cur)))))));
    driveStateCycCount++;
    sVector newVector;
    newVector.x = gPosition.x;
    newVector.y = gPosition.y;
    if(autoLogs) writeDebugStreamLine("%d (%f,%f) a:%f", npgmtime, gPosition.x, gPosition.y, gPosition.a);

    break;
   }

   case (followLineLoc): { int curState = driveState; followLine(followLineArg1, followLineArg2, followLineArg3, followLineArg4, followLineArg5, followLineArg6); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveIdle); break; }
   case (moveToTargetSimpleLoc): { int curState = driveState; moveToTargetSimple(moveToTargetSimpleArg1, moveToTargetSimpleArg2, moveToTargetSimpleArg3, moveToTargetSimpleArg4, moveToTargetSimpleArg5, moveToTargetSimpleArg6); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveIdle); break; }
   case (moveToTargetLoc): { int curState = driveState; moveToTarget(moveToTargetArg1, moveToTargetArg2, moveToTargetArg3, moveToTargetArg4, moveToTargetArg5, moveToTargetArg6, moveToTargetArg7, moveToTargetArg8, moveToTargetArg9, moveToTargetArg10, moveToTargetArg11, moveToTargetArg12); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveIdle); break; }
   case (moveToTargetDisLoc): { int curState = driveState; moveToTargetDis(moveToTargetDisArg1, moveToTargetDisArg2, moveToTargetDisArg3, moveToTargetDisArg4, moveToTargetDisArg5, moveToTargetDisArg6, moveToTargetDisArg7, moveToTargetDisArg8, moveToTargetDisArg9, moveToTargetDisArg10, moveToTargetDisArg11, moveToTargetDisArg12); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveBreak); break; }
   case (turnToAngleNewAlgLoc): { int curState = driveState; turnToAngleNewAlg(turnToAngleNewAlgArg1, turnToAngleNewAlgArg2, turnToAngleNewAlgArg3, turnToAngleNewAlgArg4, turnToAngleNewAlgArg5, turnToAngleNewAlgArg6, turnToAngleNewAlgArg7); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveBreak); break; }
   case (turnToTargetNewAlgLoc): { int curState = driveState; turnToTargetNewAlg(turnToTargetNewAlgArg1, turnToTargetNewAlgArg2, turnToTargetNewAlgArg3, turnToTargetNewAlgArg4, turnToTargetNewAlgArg5, turnToTargetNewAlgArg6, turnToTargetNewAlgArg7, turnToTargetNewAlgArg8, turnToTargetNewAlgArg9); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveBreak); break; }
   case (sweepTurnToTargetLoc): { int curState = driveState; sweepTurnToTarget(sweepTurnToTargetArg1, sweepTurnToTargetArg2, sweepTurnToTargetArg3, sweepTurnToTargetArg4, sweepTurnToTargetArg5, sweepTurnToTargetArg6, sweepTurnToTargetArg7); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveBreak); break; }
  }
  endCycle(drive);
 }
}
void handleDrive()
{
 if ((gJoy[Btn7D].cur && !gJoy[Btn7D].lst))
 {
  if(driveLogs) writeDebugStreamLine("Btn_Drive_Test Pressed");
  followLineArg1 = 0; followLineArg2 = 10; followLineArg3 = 60; followLineArg4 = mttProportional; followLineArg5 = true; followLineArg6 = (stopSoft | stopHarsh);
  driveStateChange(drivefollowLine, 2000, 0.3, velEither);




 }
 else if (!gJoy[Ch3}.cur && !gJoy[Ch4].cur && driveState == driveManual)
 {
  driveStateChange(driveBreak, 150);
 }
 else if(gJoy[Ch3}.cur || gJoy[Ch4].cur)
 {
  driveStateChange(driveManual);
 }
}
# 170 "main.c"
void setLift(word val)
{
gMotor[liftR].power = gMotor[liftL].power = (abs(val) > (127) ? (127) * sgn(val) : (val));
}

const int liftStateCount = 5; typedef enum _tStateslift { liftIdle, liftHold, liftManual, liftMove, liftMoveSimple }tStateslift; tStateslift liftState = liftIdle; float liftVelSafetyThresh = -1; tVelDir liftVelSafetyDir = -1; unsigned long liftTimeout; int liftTarget; int liftPower; int liftVelSafetyCount = 0; unsigned long liftStateStartTime = 0; unsigned long liftStateCycCount = 0; bool liftLogs = 0; void liftStateChange(int stateIn, long timeout = -1, float velSafetyThresh = -1, tVelDir velDir = -1, int arg1In = -1, int arg2In = -1) { if (liftState != stateIn) { unsigned long curTime = npgmtime; if (timeout <= 0) { liftTimeout = 0; } else { liftTimeout = ( timeout + curTime ); } liftVelSafetyCount = 0; liftStateStartTime = curTime; liftStateCycCount = 0; liftVelSafetyThresh = velSafetyThresh; liftVelSafetyDir = velDir; liftState = stateIn; liftTarget = arg1In; liftPower = arg2In; writeDebugStreamLine ("%d" "lift" "State:%d, TO:%d velS:%f, %d, %d", npgmTime, liftState, lifttimeout, liftVelSafetyThresh, liftTarget, liftPower); } } void liftVelSafetyCheck (tVelType velType = velSensor) { if (liftVelSafetyThresh != -1 && liftVelSafetyThresh != 0) { if (liftVelSafetyDir == velEither || liftVelSafetyDir == velUp) liftVelSafetyThresh = abs(liftVelSafetyThresh); else if (liftVelSafetyDir == velDown) liftVelSafetyThresh = -1 * abs(liftVelSafetyThresh); tHog(); float out = 0; bool goodVel = false; switch (velType) { case velSensor: { velocityCheck(liftPoti); out = gSensor[liftPoti].velocity; goodVel = true; break; } case velLocalY: { out = ( gVelocity.x * (float) sin(gPosition.a) )+ (gVelocity.y * (float) cos(gPosition.a)); if(liftLogs) writeDebugStreamLine("%d:""lift""velSafety out locY= %f", npgmtime, out); goodVel = true; break; } case velAngle: { out = gVelocity.a; goodVel = true; break; } } unsigned long curTime = npgmTime; if (goodVel && curTime-liftStateStartTime > 75) { if (liftVelSafetyDir == velEither) { if ( abs(out) < abs(liftVelSafetyThresh) ) { liftVelSafetyCount ++; if(liftLogs) writeDebugStreamLine("%d:""lift""velSafety trip(either)%f", npgmtime, out); } } else { if ( (sgn(liftVelSafetyThresh) == 1)? (out < liftVelSafetyThresh) : (out > liftVelSafetyThresh) ) { liftVelSafetyCount ++; if(liftLogs) writeDebugStreamLine("%d:""lift""velSafety trip(dir)%f", npgmtime, out); } } } } } void liftSafetyCheck(int timedOutState = liftIdle, int liftTarget = -1, int liftPower = -1) { bool timedOut = false; bool velSafety = false; if (!( (liftTimeout <= 0)? 1 : (npgmTime < liftTimeout) )) timedOut = true; if (liftVelSafetyCount >= 10) velSafety = true; if (velSafety || timedOut) { writeDebugStreamLine("%d" "lift" "safety: Timedout? %d at %d VelSafety? %d", npgmTime, timedout, liftTimeout, velSafety); liftStateChange(timedOutState, liftTarget, liftPower); } }
task liftSet()
{
 sCycleData lift;
 initCycle(lift, 10, "lift");
 while (true)
 {
  switch (liftState)
  {
  case 0:
   {
    setLift(0);
    break;
   }
  case 1:
   {
    if (gSensor[liftPoti].value < (800 + 50))
     setLift(!gSensor[limLift].value ? -15 : -90);
    else if (gSensor[liftPoti].value > ((800 + 1930) - 100))
     setLift(gSensor[liftPoti].value >= (800 + 1930) ? 15 : 127);
    else
     setLift(gSensor[liftPoti].value > (800 + 620) ? 12 : 10);
    break;
   }
  case 2:
   {
    short joy = gJoy[CH2].cur;
    setLift(joy);

    velocityCheck(liftPoti);


    if ((sgn(joy) == -1 && gSensor[liftPoti].value < (800 + 50)) || (sgn(joy) == 1 && gSensor[liftPoti].value > ((800 + 1930) - 100)))
     liftStateChange(liftHold);
    break;
   }
  case 3:
   {
    if (liftTarget == -1)
   {
    liftStateChange(liftHold);
   }
   else
   {
    int dir = (liftTarget < gSensor[liftPoti].value)? -1 : 1;

    int firstTarg = liftTarget + (dir == 1? -150 : 150);
    float pB = (dir == 1)? 25 : 30;
    float vKP = 0.006;
    float pKP = 30.0;
    float targVel, power;

    while ( (dir == 1)? (gSensor[liftPoti].value < firstTarg) : (gSensor[liftPoti].value > firstTarg) )
    {
     targVel = vKP * (liftTarget - gSensor[liftPoti].value);
     velocityCheck(liftPoti);
      power = (abs((pB + (pKP * (targVel - gSensor[liftPoti].velocity)))) > (127) ? (127) * sgn((pB + (pKP * (targVel - gSensor[liftPoti].velocity)))) : ((pB + (pKP * (targVel - gSensor[liftPoti].velocity)))));

      if (abs(gSensor[liftPoti].velocity) < 1.0)
      {
       if (dir == 1 && power < 27)
        power = 27;
       else if (dir == -1 && power > -27)
        power = -27;
      }

      if ( sgn(power) != sgn(dir) )
       power = (abs(power) > (5) ? (5) * sgn(power) : (power));

      if(liftLogs) writeDebugStreamLine("Power: %d in dir: %d. vel:%f, velTarg:%f, Loc: %d, Targ: %d", power, dir, gSensor[liftPoti].velocity, targVel, gSensor[liftPoti].value, firstTarg);
      setLift(power);
     sleep(10);
    }

    setLift(0);

    velocityCheck(liftPoti);
     while ( abs(gSensor[liftPoti].velocity) > 0.75 && ( (dir == 1)? (gSensor[liftPoti].value < liftTarget) : (gSensor[liftPoti].value > liftTarget) ) )
     {
      velocityCheck(liftPoti);
      if(liftLogs) writeDebugStreamLine("loc: %d, abs-vel: %f", gSensor[liftPoti].value, abs(gSensor[liftPoti].velocity));
      if(liftLogs) writeDebugStreamLine("Power: 0 in dir: %d. Loc: %d, Targ: %d", gMotor[lift].power , dir, gSensor[liftPoti].value, liftTarget);
      sleep(10);
     }
    liftStateChange(liftHold);
   }
    break;
   }
  }
  endCycle(lift);
 }
}


void handleLift()
{
 if(liftLogs) writeDebugStreamLine("State = %d, Lift loc = %d, Lift Power = %d", liftState, gSensor[liftPoti].value, gMotor[liftR].power);

 short joy = gJoy[CH2].cur;
 if ((gJoy[CH2].cur && !gJoy[CH2].lst) && liftState != liftManual)
  {
   liftStateChange(liftManual);
  }
 else if (!joy && liftState == liftManual)
  {
   liftStateChange(liftHold);
  }
 if ((gJoy[Btn7U].cur && !gJoy[Btn7U].lst) && liftState != liftMove)
 {
  liftStateChange(liftMove, 1200, -1, -1, (800 + 620)+300, -1);

 }

}
# 305 "main.c"
void setArm(word val)
{
gMotor[arm].power = (abs(val) > (127) ? (127) * sgn(val) : (val));
}

const int armStateCount = 5; typedef enum _tStatesarm { armIdle, armHold, armManual, armMove, armMoveSimple }tStatesarm; tStatesarm armState = armIdle; float armVelSafetyThresh = -1; tVelDir armVelSafetyDir = -1; unsigned long armTimeout; int armTarget; int armPower; int armVelSafetyCount = 0; unsigned long armStateStartTime = 0; unsigned long armStateCycCount = 0; bool armLogs = 0; void armStateChange(int stateIn, long timeout = -1, float velSafetyThresh = -1, tVelDir velDir = -1, int arg1In = -1, int arg2In = -1) { if (armState != stateIn) { unsigned long curTime = npgmtime; if (timeout <= 0) { armTimeout = 0; } else { armTimeout = ( timeout + curTime ); } armVelSafetyCount = 0; armStateStartTime = curTime; armStateCycCount = 0; armVelSafetyThresh = velSafetyThresh; armVelSafetyDir = velDir; armState = stateIn; armTarget = arg1In; armPower = arg2In; writeDebugStreamLine ("%d" "arm" "State:%d, TO:%d velS:%f, %d, %d", npgmTime, armState, armtimeout, armVelSafetyThresh, armTarget, armPower); } } void armVelSafetyCheck (tVelType velType = velSensor) { if (armVelSafetyThresh != -1 && armVelSafetyThresh != 0) { if (armVelSafetyDir == velEither || armVelSafetyDir == velUp) armVelSafetyThresh = abs(armVelSafetyThresh); else if (armVelSafetyDir == velDown) armVelSafetyThresh = -1 * abs(armVelSafetyThresh); tHog(); float out = 0; bool goodVel = false; switch (velType) { case velSensor: { velocityCheck(armPoti); out = gSensor[armPoti].velocity; goodVel = true; break; } case velLocalY: { out = ( gVelocity.x * (float) sin(gPosition.a) )+ (gVelocity.y * (float) cos(gPosition.a)); if(armLogs) writeDebugStreamLine("%d:""arm""velSafety out locY= %f", npgmtime, out); goodVel = true; break; } case velAngle: { out = gVelocity.a; goodVel = true; break; } } unsigned long curTime = npgmTime; if (goodVel && curTime-armStateStartTime > 75) { if (armVelSafetyDir == velEither) { if ( abs(out) < abs(armVelSafetyThresh) ) { armVelSafetyCount ++; if(armLogs) writeDebugStreamLine("%d:""arm""velSafety trip(either)%f", npgmtime, out); } } else { if ( (sgn(armVelSafetyThresh) == 1)? (out < armVelSafetyThresh) : (out > armVelSafetyThresh) ) { armVelSafetyCount ++; if(armLogs) writeDebugStreamLine("%d:""arm""velSafety trip(dir)%f", npgmtime, out); } } } } } void armSafetyCheck(int timedOutState = armIdle, int armTarget = -1, int armPower = -1) { bool timedOut = false; bool velSafety = false; if (!( (armTimeout <= 0)? 1 : (npgmTime < armTimeout) )) timedOut = true; if (armVelSafetyCount >= 10) velSafety = true; if (velSafety || timedOut) { writeDebugStreamLine("%d" "arm" "safety: Timedout? %d at %d VelSafety? %d", npgmTime, timedout, armTimeout, velSafety); armStateChange(timedOutState, armTarget, armPower); } }
task armSet()
{
sCycleData arm;
initCycle(arm, 10, "arm");
while(true)
{
 armLogs = 1;

 switch (armState)
 {
 case 0:
  {
   setArm(0);
   break;
  }
 case 1:
  {
   if (gSensor[armPoti].value < ((2700 - 1450) + 50))
    setArm(gSensor[limArm].value ? -15 : -40);
   else
    setArm(7);
   break;
  }
 case 2:
  {
   short joy = -1 * (gJoy[CH1].cur);
   velocityCheck(armPoti);
   if(armLogs) writeDebugStreamLine("power: %d, vel: %f", joy, gSensor[armPoti].velocity);

   setArm(joy);

   if ( (sgn(joy) == -1 && gSensor[armPoti].value < ((2700 - 1450) + 20)) || (sgn(joy) == 1 && gSensor[armPoti].value > ((2700 - 100) - 20)) )
    armStateChange(armHold);

   break;
  }
 case 3:
  {
   if (armTarget == -1)
   {
    armStatechange(armHold);
   }
   else
   {
    int dir = (armTarget < gSensor[armPoti].value)? -1 : 1;

    int firstTarg = armTarget + (dir == 1? -50 : 200);
    float pB = (dir == 1)? 25 : 0;
    float vKP = 0.006;
    float pKP = 30.0;
    float targVel, power;

    while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && (((dir == 1)? (gSensor[armPoti].value < firstTarg) : (gSensor[armPoti].value > firstTarg))) )
    {
    targVel = vKP * (armTarget - gSensor[armPoti].value);
     velocityCheck(armPoti);
      power = (abs((pB + (pKP * (targVel - gSensor[armPoti].velocity)))) > (127) ? (127) * sgn((pB + (pKP * (targVel - gSensor[armPoti].velocity)))) : ((pB + (pKP * (targVel - gSensor[armPoti].velocity)))));

      if (abs(gSensor[armPoti].velocity) < 2.6)
      {
       if (dir == 1 && power < 26)
        power = 26;
       else if (dir == -1 && power > -7)
        power = -7;

      if ( sgn(power) != sgn(dir) )
       power = (abs(power) > ((dir == 1)? 4 : 8) ? ((dir == 1)? 4 : 8) * sgn(power) : (power));

      if(armLogs) writeDebugStreamLine("Power: %d in dir: %d. vel:%f, velTarg:%f, Loc: %d, Targ: %d", power, dir, gSensor[armPoti].velocity, targVel, gSensor[armPoti].value, firstTarg);
      setArm(power);
     }
     sleep(10);
    }

    setArm(0);
    if(armLogs) writeDebugStreamLine("targ: %d", armTarget);
    velocityCheck(armPoti);
     while ( abs(gSensor[armPoti].velocity) > 0.75 && ( (dir == 1)? (gSensor[armPoti].value < armTarget) : (gSensor[armPoti].value > armTarget) ) )
     {
      velocityCheck(armPoti);
      if(armLogs) writeDebugStreamLine("loc: %d, abs-vel: %f", gSensor[armPoti].value, abs(gSensor[armPoti].velocity));
      if(armLogs) writeDebugStreamLine("Power: 0 in dir: %d. Loc: %d, Targ: %d", gMotor[arm].power , dir, gSensor[armPoti].value, armTarget);
      sleep(10);
     }
    armStateChange(armHold);
   }
   break;
  }
 case 4:
  {
   if (ArmTarget == -1)
   {
    armStateChange(armHold);
   }
   else
   {
    int dir = (armTarget < gSensor[armPoti].value)? -1 : 1;

    armPower = abs(armPower) * dir;
    if(armLogs) writeDebugStreamLine("move: set power %d, dir %d, poti %d", armPower, dir, gSensor[armPoti].value);
    setArm(armPower);
    while( ( (armTimeout <= 0)? 1 : (npgmTime < armTimeout) ) && armVelSafetyCount < 10 && ((dir == 1)? (gSensor[armPoti].value < armTarget) : (gSensor[armPoti].value > armTarget)) )
     sleep(10);
   }
   break;
  }
 }
 endCycle(arm);
}
}


void handleArm()
{
short joy = gJoy[CH1].cur;
if ((gJoy[CH1].cur && !gJoy[CH1].lst))
{
 armStateChange(armManual);
}
else if (!joy && armState == armManual)
{
 armStateChange(armHold, -1, -1);
}

if ((gJoy[Btn8D].cur && !gJoy[Btn8D].lst))
{
 if(armLogs) writeDebugStreamLine("Stack button");
 armStateChange(armMove, 1500, 2, velEither, (2700 - 100) - 800, 80);
}
}
# 453 "main.c"
typedef enum _tMobileStates
{
idle,
hold,
moveTop,
moveBottom,
moveUpToMid,
moveDownToMid,
holdMid
}tMobileStates;

tMobileStates gMobileState = idle;

void setMobile(word val)
{
gMotor[mobile].power = val;
}

task setMobileState()
{
sCycleData mobile;
initCycle(mobile, 10, "mobile");
while (true)
{
 switch (gMobileState)
 {
 case 0:
  {
   setMobile(0);
   break;
  }
 case 1:
  {
   setMobile(gSensor[mobilePoti].value < 1550? -5 : 5);
   break;
  }
 case 2:
  {
   setMobile(127);
   if (gSensor[mobilePoti].value > (2430 - 50))
    gMobileState = hold;
   break;
  }
 case 3:
  {
   setMobile(-127);
   if (gSensor[mobilePoti].value < (900 + 50))
    gMobileState = hold;
   break;
  }
 case 4:
  {
   setMobile(127);
   if (gSensor[mobilePoti].value > 1150)
    gMobileState = holdMid;
   break;
  }
 case 5:
  {
   setMobile(-127);
   if (gSensor[mobilePoti].value < 1650)
    gMobileState = holdMid;
   break;
  }
 case 6:
  {
   if (gSensor[mobilePoti].value > 1650 -100)
    setMobile(0);
   else
    setMobile(15);
   break;
  }
 }
 endCycle(mobile);
}
}

void handleMobile()
{
if ((gJoy[Btn6U].cur && !gJoy[Btn6U].lst))
{
 writeDebugStreamLine("Toggle Mobile");
 if (gMobileState != holdMid)
 {
  gMobileState = (gSensor[mobilePoti].value < 1550)? moveTop : moveBottom;
  writeDebugStreamLine("Mobile state: %d", gMobileState);
 }
 else
 {
  gMobileState = moveTop;
 }
}
if ((gJoy[Btn6D].cur && !gJoy[Btn6D].lst))
{
 if (gMobileState != holdMid)
  gMobileState = (gSensor[mobilePoti].value < 1550)? moveUpToMid : moveDownToMid;
 else
  gMobileState = moveBottom;
}
}

void startTasks()
{
 resetPositionFull(gPosition, 0, 0, 0);

 tStart(driveSet);
 tStart(liftSet);
 tStart(armSet);
 tStart(setMobileState);

}

void stopTasks()
{
 tStop(trackPositionTask);

 tStop(driveSet);
 tStop(liftSet);
 tStop(armSet);
 tStop(setMobileState);
}

void startup()
{
clearDebugStream();
setupSensors();
setupMotors();
setupJoysticks();
tInit();


enableJoystick(Ch3);
enableJoystick(Ch4);
enableJoystick(CH2);
enableJoystick(CH1);
enableJoystick(Btn6U);
enableJoystick(Btn6D);
enableJoystick(Btn8D);
enableJoystick(Btn7U);
enableJoystick(Btn7D);

gJoy[Ch3].deadzone = 15;
gJoy[Ch4].deadzone = 15;
gJoy[CH2].deadzone = 25;
gJoy[CH1].deadzone = 25;
}

void disabled()
{
updateSensorInputs();


}

task autonomous()
{
 sCycleData cycle;
 initCycle(cycle, 10, "autonomous");
 startTasks();

 while(true)
 {

  updateMotors();
  updateSensorInputs();
  updateSensorOutputs();
  endCycle(cycle);
 }
 stopTasks();
}

task usercontrol()
{
 sCycleData cycle;
 initCycle(cycle, 10, "usercontrol");

 startTasks();

 while (true)
 {
  updateJoysticks();
  updateMotors();
  updateSensorInputs();
  updateSensorOutputs();

  handleDrive();
  handleLift();
  handleArm();
  handleMobile();



  endCycle(cycle);
 }
 stopTasks();
}
