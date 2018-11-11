
/* LCD Controls */
typedef enum _tLCDScreen
{
  batteryLevel,
  turnBreakPow,
  turnBreakWait,
  turnCurveColour,
  turnCurveAdjust,
  sensorView,
  shootTuneMode,

  anglerTargetTuning,
  selectPosition,
  selectFlag,
  changeTarget,
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
