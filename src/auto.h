/* Defines */
// The diameter of the tracking wheels in inches
#define WHEEL_DIAMETER_IN_LR 2.783 // 2.843
#define WHEEL_DIAMETER_IN_S 2.783 // 2.843

// The distance between the tracking wheels and the centre of the robot in inches
#define L_DISTANCE_IN 4.766 //6.8198
#define R_DISTANCE_IN 4.766 //6.8198
#define S_DISTANCE_IN 7

// The number of tick per rotation of the tracking wheel
#define TICKS_PER_ROTATION 360.0

// Used internally by trackPosition
#define SPIN_TO_IN_LR (WHEEL_DIAMETER_IN_LR * PI / TICKS_PER_ROTATION)
#define SPIN_TO_IN_S (WHEEL_DIAMETER_IN_S * PI / TICKS_PER_ROTATION)

// Translate gPosition.a to normal angle (in radians)
#define TRANS_ANG(a) (-a + (pi/2))

//Find the x value given y
#define X_OF_LINE(line, y) ( ((float)y - line.b) / line.m )

//Find the y value given x
#define Y_OF_LINE(line, x) ( (line.m * (float)x) + line.b )

/* Enumerations */
typedef enum _turnDir
{
	cw,
	ccw,
	ch
} tTurnDir;

typedef enum _facingDir
{
	facingBack = -1,
	facingNone = 0,
	facingFront = 1
}

/* Structures */
typedef struct _pos
{
	float a;
	float y;
	float x;
	int leftLst;
	int rightLst;
	int backLst;
} sPos; // Position of the robot

typedef struct _vel
{
	float a;
	float y;
	float x;
	unsigned long lstChecked;
	float lstPosA;
	float lstPosY;
	float lstPosX;
} sVel; // Velocity of the robot

typedef struct _vector
{
	float y;
	float x;
} sVector; // 2D cartesian vector

typedef struct _polar
{
	float magnitude;
	float angle;
} sPolar; // 2D polar vector

typedef struct _line
{
	sVector p1;
	sVector p2;

	float m;
	float b;
} sLine;

/* Functions */
void trackPosition(int left, int right, int back, sPos& position); // Update the position of the robot
void resetPosition(sPos& position); // Reset the position
void resetVelocity(sVel& velocity, sPos& position); // Reset the velocity
void trackVelocity(sPos position, sVel& velocity); // Update the velocity of the robot
void vectorToPolar(sVector& vector, sPolar& polar); // Convert a cartesian vector to a polar vector
void polarToVector(sPolar& polar, sVector& vector); // Convert a polar vector to a cartesian vector
void makeLine(sLine& line); //Constructs line following y = mx + b
float getAngleOfLine(sLine line);
float getLengthOfLine(sLine line);
void offsetPos(float& x, float& y, float offset);
byte facing(float targX, float targY, float offset = (PI/4)); //Check to see if robot is facing target
task trackPositionTask();
task autoMotorSensorUpdateTask(); // Update motors and sensors during auto
void applyHarshStop();
void resetPositionFull(sPos& position, float x, float y, float a); // Reset the position to a desired value and starts tracking

/* Variables */
unsigned long gAutoTime = 0;
sPos gPosition;
sVel gVelocity;
