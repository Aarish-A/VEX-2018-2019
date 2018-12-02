#define DATALOG_TURN -1
#define DATALOG_SWEEP -1

/* Structures */
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

typedef struct _turnState //For accurate turn that holds to velocity
{
	float target;
	float power;
	float error;
	float lstError;
	float integral;
	float input;
	unsigned long time;
	unsigned long lstTime;
	unsigned long nextDebug;
} sTurnState;

/* Variables */
sVector gTargetLast;

/* Auxilary Movement Functions */
void applyHarshStop();

/* Movement Functions */
void moveToTarget(float x, float y, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 9.5, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);
void moveToTargetDis(float a, float d, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);

/* Turning Functions */
void turnToAngleNewAlg(float a, tAutoTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool harshStop = true);
void turnToTargetNewAlg(float x, float y, tAutoTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool harshStop = true, float offset = 0);
void turnToTargetSide(float x, float y, word big, word small, float stopOffset, bool harshStop);
