//#define RED_FRONT_X (69 - BACK_OFFSET)
//#define RED_FRONT_Y 12
#define RED_FRONT_X 58.75
#define RED_FRONT_Y BACK_OFFSET
#define RED_BACK_X (115.875 - BACK_OFFSET)
#define RED_BACK_Y 12

#define BACK_OFFSET (3.875+S_DISTANCE_IN)

typedef enum _tAlliance
{
	allianceRed,
	allianceBlue
} tAlliance;

typedef enum _tAuto
{
	autoFront,
	autoBack,

	kAutoOptionsCount
} tAuto;

typedef enum _tFlag
{
	flagTop,
	flagMid,
	flagBottom
} tFlag;

tAlliance gAlliance = allianceRed;
tAuto gAuto = autoBack;
tFlag gAutoPreloadFlag = flagTop;

void selectAuto(); //selects auto based on potentiometer and gAutoPreloadFlag variable
void runAuto(); //runs auto depending on gAuto

void anglerUnderAxle(); //waits for angler to move below the axle (call at beginning of auton, before shooting)
