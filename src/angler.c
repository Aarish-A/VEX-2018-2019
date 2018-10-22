/* Angler Functions */
void setAngler(word power)
{
	motor[angler] = LIM_TO_VAL(power, 127);
}

void moveAnglerUpTo(int target, int holdPower, float kP, float kD, int targetOffset, int timeout)
{
	writeDebugStreamLine("%d | Moving up to %d, Holding at: %d, kP: %f, kD: %f", nPgmTime, target, holdPower, kP, kD);
	anglerTarget = target;
	anglerHoldPower = holdPower;
	anglerKP = kP;
	anglerKD = kD;
	anglerTargetOffset = targetOffset;
	anglerTimeout = timeout;
	anglerState = anglerRaising;
}

void moveAnglerDownTo(int target, int holdPower, float kP, float kD, int targetOffset, int timeout)
{
	writeDebugStreamLine("%d | Moving down to %d, Holding at: %d, kP: %f, kD: %f", nPgmTime, target, holdPower, kP, kD);
	anglerTarget = target;
	anglerHoldPower = holdPower;
	anglerKP = kP;
	anglerKD = kD;
	anglerTargetOffset = targetOffset;
	anglerTimeout = timeout;
	anglerState = anglerLowering;
}

void moveAnglerTo(int target, int holdPower, float kP, float kD, int targetOffset, int timeout)
{
	if (SensorValue[anglerPoti] < target)
		moveAnglerUpTo(target, holdPower, kP, kD, targetOffset, timeout);
	else if (SensorValue[anglerPoti] > target)
		moveAnglerDownTo(target, holdPower, kP, kD, targetOffset, timeout);
}

/* Angler Task */
task handleAngler()
{
	writeDebugStreamLine("%d | Started Angler State Machine Task. Angler position at %d", nPgmTime, anglerPosition);

	while(true)
	{
		if (RISING(BTN_ANGLER_UP))
		{
			if (anglerPosition < 2) anglerPosition++;
		}
		else if (RISING(BTN_ANGLER_DOWN))
		{
			if (anglerPosition >  0) anglerPosition--;
		}

		if (RISING(BTN_ANGLER_UP) || RISING(BTN_ANGLER_DOWN))
		{
			switch(anglerPosition)
			{
				case 0:
					moveAnglerTo(ANGLER_POS_0, -10, 0.09, 5.5, 10, 1500)
					break;
				case 1:
					moveAnglerTo(ANGLER_POS_1, 6, 0.09, 5.5, 10, 1500)
					break;
				case 2:
					moveAnglerTo(ANGLER_POS_2, 9, 0.09, 5.5, 10, 1500)
					break;
			}
		}

		switch(anglerState)
		{
			case anglerIdle:
			{
				setAngler(0);
				break;
			}
			case anglerHolding:
			{
				setAngler(anglerHoldPower);
				break;
			}
			case anglerRaising:
			{
				unsigned long startTime = nPgmTime;
				int curError = 0;
				int lastError = 0;
				unsigned long curTime = nPgmTime;
				unsigned long lastTime = 0;

				while(SensorValue[anglerPoti] < (anglerTarget - anglerTargetOffset) && (nPgmTime - startTime) < anglerTimeout)
				{
					curTime = nPgmTime;
					curError = (anglerTarget - SensorValue[anglerPoti]);
					float pVal = curError * anglerKP;
					float dVal = (((float)(curError - lastError) / (float)(curTime - lastTime)) * anglerKD);
					float power = pVal + dVal;
					setAngler(power);
					writeDebugStreamLine("%d | Error: %d, pVal: %f, dVal: %f, Power: %f, dT = %d", nPgmTime, curError, pVal, dVal, power, curTime - lastTime);
					lastError = curError;
					lastTime = curTime;
					sleep(10);
				}
				// 6 at 1500
				setAngler(anglerHoldPower);
				int deltaTime = (nPgmTime - startTime);
				writeDebugStreamLine("%d | Done moving up to Position %d at %d. Poti: %d, Error: %d, Took: %d.%d s", nPgmTime, anglerPosition, anglerTarget, SensorValue[anglerPoti], anglerTarget - SensorValue[anglerPoti], deltaTime / 1000, deltaTime % 1000);
				sleep(400);
				deltaTime = (nPgmTime - startTime);
				writeDebugStreamLine("%d | After rest. Targ %d. Sen: %d, Error: %d, Took: %d.%d s", nPgmTime, anglerTarget, SensorValue[anglerPoti], anglerTarget - SensorValue[anglerPoti], deltaTime / 1000, deltaTime % 1000);
				anglerState = anglerHolding;
				break;
			}
			case anglerLowering:
			{
				unsigned long startTime = nPgmTime;
				int curError = 0;
				int lastError = 0;
				unsigned long curTime = nPgmTime;
				unsigned long lastTime = 0;

				while(SensorValue[anglerPoti] > (anglerTarget + anglerTargetOffset) && (nPgmTime - startTime) < anglerTimeout)
				{
					curTime = nPgmTime;
					curError = (anglerTarget - SensorValue[anglerPoti]);
					float pVal = curError * anglerKP;
					float dVal = (((float)(curError - lastError) / (float)(curTime - lastTime)) * anglerKD);
					float power = pVal + dVal;
					setAngler(power);
					writeDebugStreamLine("%d | Error: %d, pVal: %f, dVal: %f, Power: %f, dT = %d", nPgmTime, curError, pVal, dVal, power, curTime - lastTime);
					lastError = curError;
					lastTime = curTime;
					sleep(10);
				}
				// 6 at 1500
				setAngler(anglerHoldPower);
				int deltaTime = (nPgmTime - startTime);
				writeDebugStreamLine("%d | Done moving up to Position %d at %d. Poti: %d, Error: %d, Took: %d.%d s", nPgmTime, anglerPosition, anglerTarget, SensorValue[anglerPoti], anglerTarget - SensorValue[anglerPoti], deltaTime / 1000, deltaTime % 1000);
				sleep(400);
				deltaTime = (nPgmTime - startTime);
				writeDebugStreamLine("%d | After rest. Targ %d. Sen: %d, Error: %d, Took: %d.%d s", nPgmTime, anglerTarget, SensorValue[anglerPoti], anglerTarget - SensorValue[anglerPoti], deltaTime / 1000, deltaTime % 1000);
				anglerState = anglerHolding;
				break;
			}
		}
		sleep(10);
	}
}
