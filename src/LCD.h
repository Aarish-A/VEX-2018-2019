/* LCD Controls */
typedef enum _tLCDScreen
{
  batteryLevel,
  turnBreakPow,
  turnBreakWait,
  turnCurveColour,
  turnCurveAdjust,
  //sensorView,
  shotCount,
  shootTuneMode,

  anglerTargetTuning,
  selectPosition,
  selectFlag,
  changeTarget,
  saveTuning,
  kNumLcdScreens
} tLCDScreen;

tLCDScreen gLCDScreen = shootTuneMode;

const int kPosNum = 4;

typedef struct _sCurLCDSelection
{
  int position;
  int flag;
  string name;
  int* value;
} sCurLCDSelection;

sCurLCDSelection gCurLCDSelection;
#define LCD_L (curLCDLeft && !lstLCDLeft)
#define LCD_R (curLCDRight && !lstLCDRight)
#define LCD_M (curLCDMiddle && !lstLCDMiddle)

// Flash Variables
static char tempData[sizeof(int) * 8];
unsigned char* data;
int fileLength;
char name[16];

// Flash Functions
void setupAnglerFiles();
void updateAnglerFiles();
