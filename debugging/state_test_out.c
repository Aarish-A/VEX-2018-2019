# 1 "state_test.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "state_test.c"
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
# 31 "state_test.c" 2
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
# 32 "state_test.c" 2
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
# 33 "state_test.c" 2
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
# 34 "state_test.c" 2
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
# 35 "state_test.c" 2
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
# 36 "state_test.c" 2
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
# 37 "state_test.c" 2


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
# 40 "state_test.c" 2
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
# 41 "state_test.c" 2
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
# 42 "state_test.c" 2
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
# 43 "state_test.c" 2
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
# 44 "state_test.c" 2
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
# 45 "state_test.c" 2

# 1 "state_test.h" 1
# 19 "state_test.h"
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
# 47 "state_test.c" 2


const int driveStateCount = 3; typedef enum _tStatesdrive { driveIdle, driveBreak, driveManual }tStatesdrive; tStatesdrive driveState = driveIdle; bool driveBlocked = false; float driveVelSafetyThresh = -1; tVelDir driveVelSafetyDir = -1; unsigned long driveTimeout; float driveVel; int drivePower; int driveVelSafetyCount = 0; unsigned long driveStateStartTime = 0; unsigned long driveStateCycCount = 0; bool driveLogs = 0; void driveStateChange(int stateIn, bool await = 0, long timeout = -1, float velSafetyThresh = -1, tVelDir velDir = -1, float arg1In = -1, int arg2In = -1) { if (driveState != stateIn) { driveState = stateIn; driveBlocked = await; writeDebugStreamLine("Await?%d, machineBlocked?%d", await, driveBlocked); unsigned long curTime = npgmtime; if (timeout <= 0) { driveTimeout = 0; } else { driveTimeout = ( timeout + curTime ); } driveVelSafetyCount = 0; driveStateStartTime = curTime; driveStateCycCount = 0; driveVelSafetyThresh = velSafetyThresh; driveVelSafetyDir = velDir; driveVel = arg1In; drivePower = arg2In; writeDebugStreamLine ("%d" "drive" "State:%d, TO:%d velS:%f, %d, %d", npgmTime, driveState, drivetimeout, driveVelSafetyThresh, driveVel, drivePower); writeDebugStreamLine("Before await - Await?%d, machineBlocked?%d", await, driveBlocked); if (await) while (driveBlocked) { sleep(10); }; } } void driveVelSafetyCheck (tVelType velType = velSensor) { if (driveVelSafetyThresh != -1 && driveVelSafetyThresh != 0) { if (driveVelSafetyDir == velEither || driveVelSafetyDir == velUp) driveVelSafetyThresh = abs(driveVelSafetyThresh); else if (driveVelSafetyDir == velDown) driveVelSafetyThresh = -1 * abs(driveVelSafetyThresh); tHog(); float out = 0; bool goodVel = false; switch (velType) { case velSensor: velocityCheck(trackL); out = gSensor[trackL].velocity; goodVel = true; break; case velLocalY: out = gVelocity.localY; break; case velAngle: out = gVelocity.a; goodVel = true; break; } unsigned long curTime = npgmTime; if (goodVel && curTime-driveStateStartTime > 75) { if (driveVelSafetyDir == velEither) { if ( abs(out) < abs(driveVelSafetyThresh) ) { driveVelSafetyCount ++; if(driveLogs) writeDebugStreamLine("%d:""drive""velSafety trip(either)%f", npgmtime, out); } } else { if ( (sgn(driveVelSafetyThresh) == 1)? (out < driveVelSafetyThresh) : (out > driveVelSafetyThresh) ) { driveVelSafetyCount ++; if(driveLogs) writeDebugStreamLine("%d:""drive""velSafety trip(dir)%f", npgmtime, out); } } } } } void driveSafetyCheck(int timedOutState = driveIdle, float driveVel = -1, int drivePower = -1) { bool timedOut = false; bool velSafety = false; if (!( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) )) timedOut = true; if (driveVelSafetyCount >= 10) velSafety = true; if (velSafety || timedOut) { writeDebugStreamLine("%d" "drive" "safety: Timedout? %d at %d VelSafety? %d", npgmTime, timedout, driveTimeout, velSafety); driveStateChange(timedOutState, false, driveVel, drivePower); } }

void foo (int a, int b, int c, int d); const int fooArgCount = 4; int fooArg1 = -1; int fooArg2 = -1; int fooArg3 = -1; int fooArg4 = -1;
int bar (int a, int b, int c, int d); int barRet; const int barArgCount = 4; int barArg1 = -1; int barArg2 = -1; int barArg3 = -1; int barArg4 = -1;
typedef enum _tFuncStatesdrive { drivefoo = driveStateCount, drivebar, driveb, drivec, drived }tFuncStatesdrive; const int fooLoc = driveStateCount + 0; const int barLoc = driveStateCount + 1; const int bLoc = driveStateCount + 2; const int cLoc = driveStateCount + 3; const int dLoc = driveStateCount + 4;

void foo(int a, int b, int c, int d)
{
 if(driveLogs) writeDebugStreamLine("%d Start foo", npgmtime);
 driveBlocked = true; while( ( (driveTimeout <= 0)? 1 : (npgmTime < driveTimeout) ) && driveVelSafetyCount < 10 && (npgmtime < a) )
 {
  sleep(10);
 }
 if(driveLogs) writeDebugStreamLine("%d time passed - exit foo", nPgmTime);
}
int bar(int a, int b, int c, int d)
{
 if(driveLogs) writeDebugStreamLine("%d Start bar", npgmtime);
 return a;
}

task driveSet()
{
 driveLogs = true;
 while (true)
 {
  switch(driveState)
  {
   case driveIdle:
    driveBlocked = false;
    break;
   case driveBreak:
    driveBlocked = false;
    break;
   case driveManual:
    driveBlocked = false;
    break;

   case (fooLoc): { int curState = driveState; driveBlocked = true; foo(fooArg1, fooArg2, fooArg3, fooArg4); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveIdle); break; };
   case (barLoc): { int curState = driveState; driveBlocked = true; barRet = bar(barArg1, barArg2, barArg3, barArg4); driveSafetyCheck(driveIdle); if (driveState == curState) driveStateChange(driveIdle, 0); break; };
  }
  sleep(10);
 }
}

void driveCode()
{
 writeDebugStreamLine("Log drive %d", driveLogs);
 fooArg1 = 3000; fooArg2 = 2; fooArg3 = 3; fooArg4 = 4;
 driveStateChange(drivefoo, 1, 7000);

 barArg1 = 9; barArg2 = 2; barArg3 = 3; barArg4 = 4;
 driveStateChange(drivebar, 1, 2000);
 if(driveLogs) writeDebugStreamLine("%d a = %d", npgmtime, barRet);
}

task main()
{
 startTask(driveSet);
 driveCode();
 while(true)
 {
  sleep(10);
 }

 stopTask(driveSet);
}
