//#define RED_FRONT_X (69 - BACK_OFFSET)
//#define RED_FRONT_Y 12
//#define RED_BACK_X (115.875 - BACK_OFFSET)
//#define RED_BACK_Y 12

/* Flag Locations */
#define FLAG_X 10
#define RED_FLAG_Y 16
#define BLUE_FLAG_Y (144-18)

/* Starting Positions */
#define RED_FRONT_X 58.75
#define RED_FRONT_Y BACK_OFFSET
#define RED_BACK_X 107
#define RED_BACK_Y BACK_OFFSET

#define BLUE_FRONT_X 58.75
#define BLUE_FRONT_Y (144-BACK_OFFSET)
#define BLUE_BACK_X 107
#define BLUE_BACK_Y (144-BACK_OFFSET)

/* String Constants (displayed on LCD) */
#define S_ALLIANCE_BLUE "blue"
#define S_ALLIANCE_RED "red"

#define S_AUTO_FRONT "front"
#define S_AUTO_BACK "back"
#define S_AUTO_SKILLS "skills"

typedef enum _tAlliance
{
	allianceRed,
	allianceBlue
} tAlliance;

typedef enum _tAuto
{
	autoFront,
	autoBack,
	autoSkills,

	kAutoOptionsCount,

	autoTest
} tAuto;

typedef enum _tFlag
{
	flagTop,
	flagMid,
	flagBottom
} tFlag;

sNextShot gDummyNextShot; //Dummy variable to pass to angleShot function

tAlliance gAlliance = allianceRed;
string gSAlliance = S_ALLIANCE_RED;
tAuto gAuto = autoBack;
string gSAuto = S_AUTO_BACK;
bool gAutoPark = true;

bool gAutoLock = false;

//tFlag gAutoPreloadFlag = flagTop;

void testTurn();

void selectAuto(); //selects auto based on potentiometer and gAutoPreloadFlag variable
void runAuto(); //runs auto depending on gAuto

void anglerUnderAxle(); //waits for angler to move below the axle (call at beginning of auton, before shooting)
void redFrontBeginning(bool skills); //used for red front auto and skills
void redFrontPark(bool skills); //used for red front auto and skills
