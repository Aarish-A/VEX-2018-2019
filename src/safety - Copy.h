#define VEL_ACCEL_TIME 150
#define VEL_BAD_COUNT_LIM 5

#define WHILE(safety, condition) while( (isTimeoutSafe(safety) && isVelSafe(safety)) && (condition) )

typedef enum _tVelDir
{
	velEither,
	velUp,
	velDown
}tVelDir;

typedef enum _tVelType
{
	velNone,
	velSensor,
	velLocalY,
	velAngle
} tVelType;

typedef struct _sSafety
{
	string name;
	string message;

	unsigned long startTime;
	unsigned long timeOut;

	int velBadCount;
	tVelDir velDir;
	tVelType velType;
	tSensors sensor;
	float velThresh;

	word val;
	word valLst;
	unsigned long time;
	unsigned long timeLst;
} sSafety;

void initSafety(sSafety& safety, const string name, tVelType velType, tSensors sensor); //Create a new instance of sSafety for each subsystem
																																												//and call this function once to initilize it

void _setSafetyMessage(sSafety& safety, const string message); //Used internally by setSafety and setSafetyTO to set safety.message
void setSafety(sSafety& safety, const string message, int timeOut, tVelDir velDir, float velThresh, tVelType velType = velSensor); //Setup safety conditions (including velSafeties)
void setSafetyTO(sSafety& safety, const string message, int timeOut); //Setup timeout conditions
bool isTimeoutSafe(sSafety& safety); //Called in while loop condition to check if it has timedout
bool isVelSafe(sSafety& safety); //Called in while loop condition to check if velocity is safe
