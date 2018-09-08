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
	ccw = -1,
	ch,
	cw
} tTurnDir;

typedef enum _facingDir
{
	facingBack = -1,
	facingNone = 0,
	facingFront = 1
} tFacingDir;

/* Structures */
typedef struct _pos
{
	int leftStart;
	int rightStart;
	int backStart;

	float aStart; //Since a is recalculated each cycle, starting angle needs to be stored and added

	float a; //Recalculate angle each cycle
	float y; //Accumulate y over time
	float x; //Accumulate x over time

	int leftLst;
	int rightLst;
	int backLst;
} sPos; // Position of the robot

typedef struct _vel
{
	float a;
	float y;
	float x;
	float localY;

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

typedef struct _trianglePos
{
	sVector vector;
	float hypotenuse;
	float a;
} sTrianglePos; // Triangle - used to store location and magnitude

typedef struct _polar
{
	float magnitude;
	float angle;
} sPolar; // 2D polar vector

typedef enum _lineType
{
	point,
	diagonal,
	vertical,
	horizontal
} tLineType; // Type of linear function

typedef struct _line
{
	sVector p1;
	sVector p2;

	float length;
	float deltaX;
	float deltaY;

	tLineType lineType;
	//Used Internally to Find Equation of Line
	float m; // Used if line is diagonal
	float b;
	float xVer; // Used if line is vertical
	float yHor; // Used if line is horizontal
} sLine;

/* Tracking Functions/Tasks */
void trackPosition(int left, int right, int back, sPos& position); // Update the position of the robot
void resetPosition(sPos& position); // Reset the position
void resetVelocity(sVel& velocity, sPos& position); // Reset the velocity
void trackVelocity(sPos position, sVel& velocity); // Update the velocity of the robot
task trackPositionTask();
void resetPositionFull(sPos& position, float x, float y, float a); // Reset the position to a desired value and starts tracking

/* Vector Translation Functions */
void constructTrianglePos(sTrianglePos& pos, float x, float y, bool findAngle = false); // Creates triangle location position - setting vector and hypotenuse
void vectorToPolar(sVector& vector, sPolar& polar); // Convert a cartesian vector to a polar vector
void polarToVector(sPolar& polar, sVector& vector); // Convert a polar vector to a cartesian vector

/* Line Manipulation Functions */
void makeLine(sLine& line); //Constructs line following y = mx + b
void makeLineInverse(const sLine& original, sLine& inverse, sVector pOI); //Populares inverse line (struct)
float getAngleOfLine(sLine line); //Get angle of a line
float getLengthOfLine(sLine line); //Get length of a line
float findX(sLine line, float y);
float findY(sLine line, float x);

/* Misc Auto Functions/Tasks */
void offsetPos(float& x, float& y, float offset); //Store offset of current position in x and y
byte facingAngle(float targAngle, float offset); //Check to see if back OR front of robot is facing angle
byte facingCoord(float targX, float targY, float offset = (PI/4)); //Check to see if back OR front of robot is facing target
task autoMotorSensorUpdateTask(); // Update motors and sensors during auto
void applyHarshStop();

/* Variables */
unsigned long gAutoTime = 0;
sPos gPosition;
sVel gVelocity;
