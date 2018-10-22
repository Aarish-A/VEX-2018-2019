/* Intake Defines */

/* Intake States */
typedef enum _tIntakeStates
{
	intakeIdle,
	intakeIn,
	intakeOut
} tIntakeStates;

tIntakeStates intakeState = intakeIdle;

/* Intake Variables */


/* Intake Functions */
void setIntake(word power);

/* Intake Tasks */
task handleIntake();
