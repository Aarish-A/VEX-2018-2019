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

/* Movement Functions */
void followLineVec(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType);
void followLine(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType);
void moveToTargetSimple(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop);
void moveToTarget(float x, float y, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);
void moveToTargetDis(float a, float d, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);

/* Turning Functions */
void turnToFace(float x, float y, tFacingDir facingDir = facingFront, tStopType stopType);
void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true);
void turnToTargetNewAlg(float x, float y, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true, float offset = 0);
void sweepTurnToTarget(float x, float y, float a, float r, tTurnDir turnDir, byte power, bool slow = true);

ADD_FUNCS_TO_MACHINE_9(drive, followLineVec, followLine, moveToTargetSimple, moveToTarget, moveToTargetDis, turnToFace, turnToAngleNewAlg, turnToTargetNewAlg, sweepTurnToTarget);

PREP_FUNC_STATE_6(followLineVec, float, float, byte, tMttMode, bool, tStopType);
PREP_FUNC_STATE_6(followLine, float, float, byte, tMttMode, bool, tStopType);
PREP_FUNC_STATE_6(moveToTargetSimple, float, float, byte, tMttMode, bool, bool);
PREP_FUNC_STATE_12(moveToTarget, float, float, float, float, byte, byte, float, float, byte, float, tStopType, tMttMode);
PREP_FUNC_STATE_12(moveToTargetDis, float, float, float, float, byte, byte, float, float, byte, float, tStopType, tMttMode);
PREP_FUNC_STATE_4(turnToFace, float, float, tFacingDir, tStopType);
PREP_FUNC_STATE_7(turnToAngleNewAlg, float, tTurnDir, float, byte, float, bool, bool);
PREP_FUNC_STATE_9(turnToTargetNewAlg, float, float, tTurnDir, float, byte, float, bool, bool, float);
PREP_FUNC_STATE_7(sweepTurnToTarget, float, float, float, float, tTurnDir, byte, bool);