#define DATALOG_TURN -1
#define DATALOG_SWEEP -1

/* Enumerations */
typedef enum _stopType
{
	stopNone =		0b00000000,
	stopSoft =		0b00000001,
	stopHarsh =		0b00000010
} tStopType;

typedef enum _mttMode
{
	mttSimple,
	mttProportional,
	mttCascading
} tMttMode;

/* Variables */
sVector gTargetLast;

///* Movement Functions */
//void followLine(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType);
//void moveToTargetSimple(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop);
//void moveToTarget(float x, float y, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);
//void moveToTargetDis(float a, float d, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);

///* Turning Functions */
//void turnToFace(float x, float y, tFacingDir facingDir = facingFront, tStopType stopType);
//void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true);
//void turnToTargetNewAlg(float x, float y, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true, float offset = 0);
//void sweepTurnToTarget(float x, float y, float a, float r, tTurnDir turnDir, byte power, bool slow = true);

/* Movement Functions */
PREP_FUNC_STATE_VOID_7(void, followLineVec, float, x, float, y, float, offsetA, byte, power, tMttMode, mode, bool, correction, tStopType, stopType);
PREP_FUNC_STATE_VOID_6(void, followLine, float, x, float, y, byte, power, tMttMode, mode, bool, correction, tStopType, stopType);
PREP_FUNC_STATE_VOID_6(void, moveToTargetSimple, float, x, float, y, byte, power, tMttMode, mode, bool, correction, bool, harshStop);
PREP_FUNC_STATE_VOID_12(void, moveToTarget, float, x, float, y, float, xs, float, ys, byte, power, byte, startPower, float, maxErrX, float, decelEarly, byte, decelPower, float, dropEarly, tStopType, stopType, tMttMode, mode);
PREP_FUNC_STATE_VOID_12(void, moveToTargetDis, float, a, float, d, float, xs, float, ys, byte, power, byte, startPower, float, maxErrX, float, decelEarly, byte, decelPower, float, dropEarly, tStopType, stopType, tMttMode, mode);

/* Turning Functions */
PREP_FUNC_STATE_VOID_4(void, turnToFace, float, x, float, y, tFacingDir, facingDir, tStopType, stopType);
PREP_FUNC_STATE_VOID_7(void, turnToAngleNewAlg, float, a, tTurnDir, turnDir, float, fullRatio, byte, coastPower, float, stopOffsetDeg, bool, mogo, bool, harshStop);
PREP_FUNC_STATE_VOID_9(void, turnToTargetNewAlg, float, x, float, y, tTurnDir, turnDir, float, fullRatio, byte, coastPower, float, stopOffsetDeg, bool, mogo, bool, harshStop, float, offset);
PREP_FUNC_STATE_VOID_7(void, sweepTurnToTarget, float, x, float, y, float, a, float, r, tTurnDir, turnDir, byte, power, bool, slow);

ADD_FUNCS_TO_MACHINE_9(drive, followLineVec, followLine, moveToTargetSimple, moveToTarget, moveToTargetDis, turnToFace, turnToAngleNewAlg, turnToTargetNewAlg, sweepTurnToTarget);
