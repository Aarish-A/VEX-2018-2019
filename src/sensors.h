/* Defines */
#define SENSOR_DATA_POINT_COUNT 20

/* Enumerations */
typedef enum _tSensorClass
{
	snclsNone,
	snclsInput,
	snclsOutput,
	snclsOther
} tSensorClass; // Represents a sensor base type

typedef enum _tSensorMode
{
	snmdNormal,
	snmdDgtIn,
	snmdInverted
} tSensorMode;

/* Structures */
typedef struct _sSensorDataPoint
{
	int value;
	unsigned long timestamp;
} sSensorDataPoint;

typedef struct _sSensor
{
	int value;
	int lstValue;
	int rawValue;
	tSensorClass cls;
	tSensors port;
	float velocity;
	float lstVelocity;
	sSensorDataPoint dataPointArr[SENSOR_DATA_POINT_COUNT];
	ubyte arrHead;
	ubyte arrTail;
	ubyte dataCount;
	//bool velGood;
	tSensorMode mode;
	bool potiCheckVel;
	int dgtMin;
	int dgtMax;

#ifdef MOTOR_SENSOR_LOGS
	byte rawDatalog;
	byte valueDatalog;
	byte velDatalog;
#endif

#ifdef CHECK_POTI_JUMPS
	ubyte filterAcc;
	ubyte jumpCount;
#endif
} sSensor; // Represent a single sensor

/* Functions */
void setupSensors(); // Initilize all the sensors
void updateSensorOutput(tSensors sen); // Update the output for a single sensor
void updateSensorOutputs(); // Update all the output sensors
void updateSensorInput(tSensors sen); // Update the input for a singe sensor
void updateSensorInputs(); // Update all the input sensors
tSensorClass checkSenClass(tSensors sen); // Check the sensor class of a motor
bool correctBtnIn(tSensors sen); // Call this function to interpret a sensor set up using setupDgtIn
void setupDgtIn(tSensors sen, int min, int max); // Setup a sensor to be interpreted as a boolean using a custom range
void setupInvertedSen(tSensors sen); // Setup a sensor to be interpreted as a boolean and inverted
void resetQuadratureEncoder(tSensors sen); // Reset a quadrature encoder
void velocityCheck(tSensors sen, int offset = -1); // Check the velocity of a sensor
void startSensor(tSensors sen); // Set a sensors starting value
void startSensors(); // Set all the sensors starting value

/* Variables */
sSensor gSensor[kNumbOfTotalSensors]; // The global sensor array

#ifdef MOTOR_SENSOR_LOGS
bool _sensorDoDatalog = false;
#endif
