/* Structures */
typedef struct _sMotor
{
	word powerCur;
	float powerScale;
} sMotor; // Represent a single motor

/* Functions */
void setupMotors(); // Initilize all the motors
void setMotor(tMotor mot, word power); // Update a motor output
void killAllMotors(); // Sets all motors to 0

/* Variables */
sMotor gMotor[kNumbOfTotalMotors]; // The global motor array
