
/* LCD Controls */
typedef enum _tLCDScreen
{
  batteryLevel,
  turnBreakPow,
  turnBreakWait,
  turnCurveColour,
  turnCurveAdjust,
  //sensorView,
  trackingTune,
  shotCount,
  shootTuneMode,

  anglerTargetTuning,
  selectPosition,
  selectFlag,
  changeTarget,
  kNumLcdScreens
} tLCDScreen;

tLCDScreen gLCDScreen = trackingTune;//shootTuneMode;

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
