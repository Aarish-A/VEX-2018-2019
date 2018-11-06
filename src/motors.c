/* Functions */
void setupMotors()
{
	// Clear both motor arrays
	for (ubyte i = 0; i < kNumbOfTotalMotors; ++i) {
		gMotor[i].powerCur = motor[i] = 0;
		gMotor[i].powerScale = -1;
	}

}

void setMotor(tMotor mot, word power)
{
	LIM_TO_VAL_SET(power, 127);
	if (power != gMotor[mot].powerCur)
	{
		//writeDebugStreamLine("%d Set Motor %d", nPgmTime, power);
		motor[mot] = gMotor[mot].powerCur = gMotor[mot].powerScale >= 0 ? round(power * gMotor[mot].powerScale) : power;
	}
}

void killAllMotors()
{
	for (ubyte i = 0; i < kNumbOfTotalMotors; ++i)
		setMotor(i, 0);
}
