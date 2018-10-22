/* Angler Functions */
void setIntake(word power)
{
	motor[intake] = LIM_TO_VAL(power, 127);
}

/* Angler Tasks */
task handleIntake()
{
	writeDebugStreamLine("%d | Started Intake State Machine Task.", nPgmTime);

	while(true)
	{
		if (RISING(BTN_INTAKE_IN))
		{
			if (intakeState != intakeIdle)
			{
				intakeState = intakeIdle;
				writeDebugStreamLine("Intake Stopped");
			}
			else
			{
				intakeState = intakeIn;
				writeDebugStreamLine("Intaking In");
			}
		}
		else if (RISING(BTN_INTAKE_OUT))
		{
			if (intakeState != intakeIdle)
			{
				intakeState = intakeIdle;
				writeDebugStreamLine("Intake Stopped");
			}
			else
			{
				intakeState = intakeOut;
				writeDebugStreamLine("Intaking Out");
			}
		}

		switch(intakeState)
		{
			case intakeIdle:
			{
				setIntake(0);
				break;
			}
			case intakeIn:
			{
				setIntake(127);
				break;
			}
			case intakeOut:
			{
				setIntake(-127);
				break;
			}
		}
		sleep(10);
	}
}
