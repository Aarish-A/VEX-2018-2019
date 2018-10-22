/* Angler Defines */
#define ANGLER_BOTTOM 125
#define ANGLER_TOP 3600
#define ANGLER_POS_0 125
#define ANGLER_POS_1 1900
#define ANGLER_POS_2 2500

/* Angler States */
typedef enum _tAnglerStates
{
	anglerIdle,
	anglerHolding,
	anglerRaising,
	anglerLowering
} tAnglerStates;

tAnglerStates anglerState = anglerIdle;

/* Angler Variables */
int anglerPosition = 0;
int anglerTarget;
int anglerHoldPower = -10;
float anglerKP = 0.0;
float anglerKD = 0.0;
int anglerTargetOffset = 0;
int anglerTimeout = 2500;

/* Angler Functions */
void setAngler(word power);

//void moveAnglerUpTo(int target, int holdPower, float kP, float kD, int targetOffset, int timeout);
//void moveAnglerDownTo(int target, int holdPower, float kP, float kD, int targetOffset, int timeout);
void moveAnglerTo(int target, int holdPower, float kP, float kD, int targetOffset, int timeout, tAnglerStates _anglerState);

/* Angler Tasks */
task handleAngler();
