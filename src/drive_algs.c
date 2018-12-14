/* Functions */
void applyHarshStop()
{
	sVector vel;
	vel.x = gVelocity.x;
	vel.y = gVelocity.y;
	sPolar polarVel;
	vectorToPolar(vel, polarVel);
	polarVel.angle += gPosition.a;
	polarToVector(polarVel, vel);
	float yPow = vel.y, aPow = gVelocity.a;

	writeDebugStreamLine("Vel y | a: %f | %f", yPow, aPow);

	yPow *= -0.7;
	aPow *= -6.3;

	word left = yPow + aPow;
	word right = yPow - aPow;

	left = sgn(left) * MAX(fabs(left), 7);
	right = sgn(right) * MAX(fabs(right), 7);

	LIM_TO_VAL_SET(left, 20);
	LIM_TO_VAL_SET(right, 20);

	writeDebugStreamLine("Applying harsh stop: %d %d", left, right);
	setDrive(left, right);
	//updateMotors();

	unsigned long startTime = nPgmTime;
	while( (nPgmTime-startTime) < 200)
		sleep(10);

	setDrive(0, 0);
	//updateMotors();
}

void moveToTargetY(float y, byte power, byte startPower, tStopType stopType)
{
	word last = startPower;
	word finalPower = startPower;

	while (gPosition.y < y)
	{
		LIM_TO_VAL_SET(finalPower, abs(power));
		if (finalPower * sgn(power) < 30)
			finalPower = 30 * sgn(power);
		word delta = finalPower - last;
		LIM_TO_VAL_SET(delta, 5);
		finalPower = last += delta;

		setDrive(finalPower, finalPower);

		sleep(10);
	}

	if (stopType & stopHarsh)
		applyHarshStop();
	else
		setDrive(0, 0);

	//LOG(auto)("TargY: %f | %f %f %f", y,  gPosition.x, gPosition.y, radToDeg(gPosition.a));
}

void moveToTarget(float x, float y, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
	float xs = gPosition.x;
	float ys = gPosition.y;
	LOG_AUTO(("Moving to %f %f from %f %f at %d. decelEarly:%f, decelPower:%f, dropEarly:%f", x, y, xs, ys, power, decelEarly, decelPower, dropEarly))

	gTargetLast.y = y;
	gTargetLast.x = x;

	// Create the line to follow
	sLine followLine;

	// Start points
	followLine.p1.y = ys;
	followLine.p1.x = xs;

	// End points
	followLine.p2.y = y;
	followLine.p2.x = x;

	float lineLength = getLengthOfLine(followLine);
	////LOG_AUTO(("Line length: %.2f", lineLength))
	float lineAngle = getAngleOfLine(followLine); // Get the angle of the line that we're following relative to the vertical
	float pidAngle = nearAngle(lineAngle - (power < 0 ? PI : 0), gPosition.a);
	//LOG(auto)("Line | Pid angle: %f | %f", radToDeg(lineAngle), radToDeg(pidAngle));

	// Current position relative to the ending point
	sVector currentPosVector;
	sPolar currentPosPolar;

	sCycleData cycle;
	initCycle(cycle, 10, "moveToTarget");

	float vel;
	float _sin = sin(lineAngle);
	float _cos = cos(lineAngle);

	word last = startPower;
	float correction = 0;

	if (mode == mttSimple)
		setDrive(power, power);

	word finalPower = power;

	unsigned long timeStart = nPgmTime;
	do
	{
		//VEL_CHECK_INC(drive, velLocalY);

		currentPosVector.x = gPosition.x - x;
		currentPosVector.y = gPosition.y - y;
		vectorToPolar(currentPosVector, currentPosPolar);
		currentPosPolar.angle += lineAngle;
		polarToVector(currentPosPolar, currentPosVector);

		if (maxErrX)
		{
			float errA = gPosition.a - pidAngle;
			float errX = currentPosVector.x + currentPosVector.y * sin(errA) / cos(errA);
			float correctA = atan2(x - gPosition.x, y - gPosition.y);
			if (power < 0)
				correctA += PI;
			correction = fabs(errX) > maxErrX ? 6.0 * (nearAngle(correctA, gPosition.a) - gPosition.a) * sgn(power) : 0;
		}

		//if (mode != mttSimple)
		//{
			switch (mode)
			{
			case mttProportional:
				finalPower = round(-127.0 / 70.0 * currentPosVector.y) * sgn(power);
				break;
			case mttCascading:
				const float kB = 2.8;
				const float kP = 2.0;
				/*
#if SKILLS_ROUTE == 0
				const float kB = 2.8;
				const float kP = 2.0;
#else
				float kB, kP;
				if (nPgmTime - gAutoTime > 40000)
				{
				kB = 5.0;
				kP = 3.2;
				}
				else
				{
				kB = 4.5;
				kP = 2.5;
				}
#endif
				*/
				float vTarget = 56 * (1 - exp(0.07 * (currentPosVector.y + dropEarly)));
				finalPower = round(kB * vTarget + kP * (vTarget - vel)) * sgn(power);
				break;
			}
			LIM_TO_VAL_SET(finalPower, abs(power));
			if (finalPower * sgn(power) < 30)
				finalPower = 30 * sgn(power);
			word delta = finalPower - last;
			LIM_TO_VAL_SET(delta, 5);
			finalPower = last += delta;
		//}

		switch (sgn(correction))
		{
		case 0:
			setDrive(finalPower, finalPower);
			break;
		case 1:
			setDrive(finalPower, finalPower * exp(-correction));
			break;
		case -1:
			setDrive(finalPower * exp(correction), finalPower);
			break;
		}

		vel = _sin * gVelocity.x + _cos * gVelocity.y;

		////LOG(auto)("%d %f < %f?. pow:%d, vel:%d, sin%f, xVel:%f, cos%f, yVel:%f", nPgmTime, currentPosVector.y,(currentPosVector.y < -dropEarly - MAX((vel * ((stopType & stopSoft) ? 0.175 : 0.098)), decelEarly)), finalPower, vel, _sin, gVelocity.x, _cos, gVelocity.y, decelEarly);

		endCycle(cycle);
	} while( (currentPosVector.y < -dropEarly - MAX((vel * ((stopType & stopSoft) ? 0.175 : 0.098)), decelEarly)) );

	LOG_AUTO_2(("%dDone fast %f < %f?. pow:%d, vel:%d, sin%f, xVel:%f, cos%f, yVel:%f. a:%f, velA:%f", nPgmTime, currentPosVector.y,(currentPosVector.y < -dropEarly - MAX((vel * ((stopType & stopSoft) ? 0.175 : 0.098)), decelEarly)), finalPower, vel, _sin, gVelocity.x, _cos, gVelocity.y, decelEarly, gPosition.a, gVelocity.a));

	setDrive(decelPower, decelPower);

	do
	{
		currentPosVector.x = gPosition.x - x;
		currentPosVector.y = gPosition.y - y;
		vectorToPolar(currentPosVector, currentPosPolar);
		currentPosPolar.angle += lineAngle;
		polarToVector(currentPosPolar, currentPosVector);

		vel = _sin * gVelocity.x + _cos * gVelocity.y;

		endCycle(cycle);
	}while(  (currentPosVector.y < -dropEarly - (vel * ((stopType & stopSoft) ? 0.175 : 0.098))));

	if (stopType & stopSoft)
	{
		setDrive(-6 * sgn(power), -6 * sgn(power));
		do
		{
			currentPosVector.x = gPosition.x - x;
			currentPosVector.y = gPosition.y - y;
			vectorToPolar(currentPosVector, currentPosPolar);
			currentPosPolar.angle += lineAngle;
			polarToVector(currentPosPolar, currentPosVector);

			vel = _sin * gVelocity.x + _cos * gVelocity.y;

			endCycle(cycle);
		} while(  (vel > 7 && currentPosVector.y < 0));
	}

	if (stopType & stopHarsh)
		applyHarshStop();
	else
		setDrive(0, 0);

	LOG_AUTO(("Moved to %f %f from %f %f | %f %f %f. velA:%f", x, y, xs, ys,  gPosition.x, gPosition.y, radToDeg(gPosition.a), gVelocity.a));
}

void moveToTargetDis(float a, float d, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
	a += gPosition.a;

	float xs = gPosition.x;
	float ys = gPosition.y;
	moveToTarget(xs + d * sin(a), ys + d * cos(a), power, startPower, maxErrX, decelEarly, decelPower, dropEarly, stopType, mode);
	//moveToTarget(xs + d * sin(a), ys + d * cos(a), xs, ys, power, startPower, maxErrX, decelEarly, decelPower, dropEarly, stopType, mode);
}

void turnToAngleNewAlg(float a, tAutoTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool harshStop)
{
	////LOG_AUTO(("Turning to %f", radToDeg(a)))
	a = degToRad(a);

	if (turnDir == ch)
		if (fmod(a - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

	float endFull;

	unsigned long timeStart = nPgmTime;

	switch (turnDir)
	{
	case cw:
		a = gPosition.a + fmod(a - gPosition.a, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + a * fullRatio;
		setDrive(127, -127);
		while((gPosition.a < endFull))
		{
			//driveVelSafetyCheck()
			if (DATALOG_TURN != -1)
			{
				//VEL_CHECK_INC(drive, velAngle);
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		setDrive(coastPower, -coastPower);
		timeStart = nPgmTime;
		while( (gPosition.a < a - degToRad(stopOffsetDeg)))
		{
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		////LOG_AUTO(("Turn done: %d",  gPosition.a))
		if (harshStop)
		{
			setDrive(-20, 20);
			sleep(150);
			////LOG_AUTO(("Break done: %d",  gPosition.a))
		}
		setDrive(0, 0);
		break;
	case ccw:
		a = gPosition.a - fmod(gPosition.a - a, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + a * fullRatio;
		setDrive(-127, 127);
		while( (gPosition.a > endFull))
		{
			//VEL_CHECK_INC(drive, velAngle);
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		setDrive(-coastPower, coastPower);
		timeStart = nPgmTime;
		while(  (gPosition.a > a + degToRad(stopOffsetDeg)))
		{
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		////LOG_AUTO(("Turn done: %d",  gPosition.a))
		if (harshStop)
		{
			setDrive(20, -20);
			sleep(150);
			////LOG_AUTO(("Break done: %d",  gPosition.a))
		}
		setDrive(0, 0);
		break;
	}
	LOG_AUTO(("Turned to %f | %f %f %f", radToDeg(a), gPosition.y, gPosition.x, radToDeg(gPosition.a)));
}

void turnToTargetSide(float x, float y, word big, word small, float stopOffset, bool harshStop)
{
	unsigned long startTime = nPgmTime;
	tAutoTurnDir turnDir;
	float target, error;

	if (fmod(atan2(x - gPosition.x, y - gPosition.y) - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;
	//LOG_AUTO(("%d turnSide:%d", nPgmTime, turnDir))

	switch (turnDir)
	{
	case cw:
		do
		{
			target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) - gPosition.a, PI * 2);
			error = target-gPosition.a;

			setDrive(big, small);

			sleep(10);
		} while( error > degToRad(abs(stopOffset)) );
		break;

	case ccw:
		do
		{
			target = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y), PI * 2);
			error = target-gPosition.a;

			setDrive(small, big);

			sleep(10);
		} while( error < degToRad(-abs(stopOffset)) );
		break;
	}

	//LOG_AUTO(("%d turnSide done(%f,%f) %f targ:%f, err:%f", nPgmTime, gPosition.x, gPosition.y, gPosition.a, radToDeg(target), radToDeg(target-gPosition.a)))

	if (harshStop) applyHarshStop();
	else setDrive(0,0);
	//LOG_AUTO(("%d turnSide exit(%f,%f) %f targ:%f, err:%f T:%d", nPgmTime, gPosition.x, gPosition.y, radToDeg(gPosition.a), radToDeg(target), radToDeg(target-gPosition.a), (nPgmTime-startTime)))

}
void turnToAngleP(float target, word startPower, float stopOffset, bool harshStop, float kP = 60)
{
	target = degToRad(target);

	unsigned long startTime = nPgmTime;
	tAutoTurnDir turnDir;
	float error;
	float absPower;

	if (abs(startPower) < 35) startPower = 35;
	word lastPower = startPower;
	word finalPower = startPower;

	float absTarget = (target < 0)? ((PI * 2)+target) : target;
	if (fmod(absTarget - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;
	//LOG_AUTO(("%d turnToTargetP:%d", nPgmTime, turnDir))

	switch (turnDir)
	{
	case cw:
		do
		{
			error = target-gPosition.a;

			absPower = fabs(error * kP);

			word delta = absPower-lastPower;
			LIM_TO_VAL_SET(delta, 5);
			finalPower = lastPower += delta;

			setDrive(finalPower, -finalPower);

			//LOG_AUTO(("%d P:%d Err:%f, errRad:%f", nPgmTime, finalPower, radToDeg(error), error))
			sleep(10);
		} while( error > degToRad(abs(stopOffset)) );
		break;

	case ccw:
		do
		{
			error = target-gPosition.a;
			absPower = fabs(error * kP);

			word delta = absPower-lastPower;
			LIM_TO_VAL_SET(delta, 5);
			finalPower = lastPower += delta;

			setDrive(-finalPower, finalPower);

			//LOG_AUTO(("%d P:%d Err:%f, errRad:%f", nPgmTime, finalPower, radToDeg(error), error))
			sleep(10);
		} while( error < degToRad(-abs(stopOffset)) );
		break;
	}

	//LOG_AUTO(("%d turnP done(%f,%f) %f targ:%f, err:%f", nPgmTime, gPosition.x, gPosition.y, gPosition.a, radToDeg(target), radToDeg(target-gPosition.a)))

	if (harshStop) applyHarshStop();
	else setDrive(0,0);
	//LOG_AUTO(("%d turnP exit(%f,%f) %f targ:%f, err:%f T:%d", nPgmTime, gPosition.x, gPosition.y, radToDeg(gPosition.a), radToDeg(target), radToDeg(target-gPosition.a), (nPgmTime-startTime)))

}

void turnToTargetP(float x, float y, word startPower, float stopOffset, bool harshStop, float kP = 60)
{
	unsigned long startTime = nPgmTime;
	tAutoTurnDir turnDir;
	float target, error;
	float absPower;

	if (abs(startPower) < 35) startPower = 35;
	word lastPower = startPower;
	word finalPower = startPower;

	if (fmod(atan2(x - gPosition.x, y - gPosition.y) - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;
	LOG_AUTO_2(("%d turnToTargetP:%d", nPgmTime, turnDir))

	switch (turnDir)
	{
	case cw:
		do
		{
			target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) - gPosition.a, PI * 2);
			error = target-gPosition.a;

			absPower = fabs(error * kP);

			word delta = absPower-lastPower;
			LIM_TO_VAL_SET(delta, 5);
			finalPower = lastPower += delta;

			setDrive(finalPower, -finalPower);

			LOG_AUTO_2(("%d P:%d Err:%f, errRad:%f", nPgmTime, finalPower, radToDeg(error), error))
			sleep(10);
		} while( error > degToRad(abs(stopOffset)) );
		break;

	case ccw:
		do
		{
			target = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y), PI * 2);
			error = target-gPosition.a;
			absPower = fabs(error * kP);

			word delta = absPower-lastPower;
			LIM_TO_VAL_SET(delta, 5);
			finalPower = lastPower += delta;

			setDrive(-finalPower, finalPower);

			LOG_AUTO_2(("%d P:%d Err:%f, errRad:%f", nPgmTime, finalPower, radToDeg(error), error))
			sleep(10);
		} while( error < degToRad(-abs(stopOffset)) );
		break;
	}

	LOG_AUTO_2(("%d turnP done(%f,%f) %f targ:%f, err:%f", nPgmTime, gPosition.x, gPosition.y, gPosition.a, radToDeg(target), radToDeg(target-gPosition.a)))

	if (harshStop) applyHarshStop();
	else setDrive(0,0);
	LOG_AUTO_2(("%d turnP exit(%f,%f) %f targ:%f, err:%f T:%d", nPgmTime, gPosition.x, gPosition.y, radToDeg(gPosition.a), radToDeg(target), radToDeg(target-gPosition.a), (nPgmTime-startTime)))

}

void turnAccurateInternalCw(float a, sTurnState& state)
{
	unsigned long deltaTime = state.time - state.lstTime;
	float vel = gVelocity.a;

	const float kP = 16, kI = 0.00;

	if (deltaTime >= 1)
	{
		state.input = vel;

		state.lstError = state.error;
		state.error = state.target - state.input;

		state.integral += state.error * deltaTime;
		//if (state.integral < -8) state.integral = 0;

		state.power = state.error * kP + state.integral * kI;

		state.power += 29;//31;

		if (state.power < 28 && gVelocity.a < 1.3) state.power = 28;

		//if (state.power < 0) state.power /= 6.0;

		//if (state.power > 50) state.power = 50;
		if (state.power < -5) state.power = -5;

		setDrive(state.power, -state.power);

		if (state.time >= state.nextDebug)
		{
			//writeDebugStreamLine("%f %f %f", state.power, state.error, state.integral);
			state.nextDebug = state.time + 20;

		}

		datalogDataGroupStart();
		datalogAddValue(0, 10*gVelocity.a);
		datalogAddValue(1, state.power);
		datalogAddValue(2, state.error*10);
		datalogAddValue(3, (state.error*kP));
		datalogAddValue(4, (state.integral*kI));
		datalogAddValue(5, state.target*10);
		datalogDataGroupEnd();

		state.lstTime = state.time;
	}

	sleep(1);

	state.time = nPgmTime;
}

void turnAccurateInternalCcw(float a, sTurnState& state)
{
	tHog();
	unsigned long deltaTime = state.time - state.lstTime;
	float vel = gVelocity.a;

	const float kP = 16, kI = 0.00;

	if (deltaTime >= 1)
	{
		state.input = vel;

		state.lstError = state.error;
		state.error = state.target - state.input;

		state.integral += state.error * deltaTime;
		//if (state.integral > 8) state.integral = 0;

		state.power = state.error * kP + state.integral * kI;

		state.power -= 29;//31;

		//if (state.power > 0) state.power /= 6.0;

		//if (state.power < -50) state.power = -50;
		if (state.power > 5) state.power = 5;

		setDrive(state.power, -state.power);

		if (state.time >= state.nextDebug)
		{
			//writeDebugStreamLine("%f %f %f", state.power, state.error, state.integral);
			state.nextDebug = state.time + 20;

		}

		datalogDataGroupStart();
		datalogAddValue(0, 10*gVelocity.a);
		datalogAddValue(1, state.power);
		datalogAddValue(2, state.error*10);
		datalogAddValue(3, (state.error*kP));
		datalogAddValue(4, (state.integral*kI));
		datalogAddValue(5, state.target*10);
		datalogDataGroupEnd();

		state.lstTime = state.time;
	}

	sleep(1);

	state.time = nPgmTime;
	tRelease();
}

void turnToTargetAccurate(float x, float y, tAutoTurnDir turnDir, byte left, byte right, float offset)
{
	offset = degToRad(offset);
	LOG_AUTO_2(("%d Turning to %f %f", nPgmTime, x, y));

	sTurnState state;
	state.time = nPgmTime;
	state.lstTime = state.time;
	state.nextDebug = 0;
	state.input = gVelocity.a;
	state.power = state.error = state.integral = 0;

	left = abs(left);
	right = abs(right);

	float a = atan2(x - gPosition.x, y - gPosition.y) + offset;

	if (turnDir == ch)
		if (fmod(a - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

	switch (turnDir)
	{
	case cw:
		a = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) - gPosition.a, PI * 2);
		writeDebugStreamLine("%f", a);
		setDrive(left, -right);
		while (gPosition.a < a - gVelocity.a * 0.6)
		{
			a = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
			sleep(1);
		}
		LOG_AUTO(("%d turn fast done err:%f vel:%f", nPgmTime, radToDeg(a-gPosition.a), gVelocity.a));
		//
		state.target = 0.78; //0.940;
		while (gPosition.a < a + degToRad(-3.6 + (state.target - gVelocity.a) * 0.1))//+ degToRad(-5.3 + (state.target - gVelocity.a) * 0.25))
		{
			a = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
			turnAccurateInternalCw(a, state);
		}
		LOG_AUTO(("%d turn slow done err:%f vel:%f", nPgmTime, radToDeg(a-gPosition.a), gVelocity.a));
		setDrive(-15, 15);
		sleep(150);
		setDrive(0, 0);
		LOG_AUTO(("%d turn break done(%f,%f) %f targ:%f, err:%f vel:%f", nPgmTime, gPosition.x, gPosition.y, gPosition.a, radToDeg(a), radToDeg(a-gPosition.a), gVelocity.a));
		break;
	case ccw:
		a = gPosition.a - fmod(gPosition.a - a, PI * 2);
		writeDebugStreamLine("%f", a);
		setDrive(-left, right);
		while (gPosition.a > a - gVelocity.a * 0.6)
		{
			a = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y) - offset, PI * 2);
			sleep(1);
		}
		LOG_AUTO(("%d turn fast done err:%f vel:%f", nPgmTime, radToDeg(a-gPosition.a), gVelocity.a));
		//writeDebugStreamLine("%f", gVelocity.a);
		//
		state.target = -0.78;//0.940;
		while (gPosition.a > a - degToRad(-3.6 + (state.target - gVelocity.a) * 0.1))//+ (state.target - gVelocity.a) * 0.15
		{
			a = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y) - offset, PI * 2);
			turnAccurateInternalCcw(a, state);
		}
		LOG_AUTO(("%d turn slow done err:%f vel:%f", nPgmTime, radToDeg(a-gPosition.a), gVelocity.a));
		setDrive(15, -15);
		sleep(150);
		setDrive(0, 0);
		LOG_AUTO(("%d turn break done(%f,%f) %f targ:%f, err:%f vel:%f", nPgmTime, gPosition.x, gPosition.y, gPosition.a, radToDeg(a), radToDeg(a-gPosition.a), gVelocity.a));
		break;
	}

	writeDebugStreamLine("Turned to %f %f | %f %f %f", y, x, gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void turnToTargetNewAlg(float x, float y, tAutoTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool harshStop, float offset)
{
	////LOG_AUTO(("Turning to %f %f", y, x))

	unsigned long startTime = nPgmTime;

	if (turnDir == ch)
		if (fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

	float endFull, target;


	unsigned long timeStart =  nPgmTime;

	switch (turnDir)
	{
	case cw:
		target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + target * fullRatio;
		////LOG_AUTO(("%f %f", radToDeg(target), radToDeg(endFull)))
		setDrive(127, -127);
		while( (gPosition.a < endFull ))
		{
			//VEL_CHECK_INC(drive, velAngle);
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		setDrive(coastPower, -coastPower);
		timeStart = nPgmTime;
		while( (gPosition.a < nearAngle(atan2(x - gPosition.x, y - gPosition.y) + offset, target) - degToRad(stopOffsetDeg)) )
		{
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		////LOG_AUTO(("Turn done: %d",  gPosition.a))
		if (harshStop)
		{
			setDrive(-20, 20);
			sleep(150);
			////LOG_AUTO(("Break done: %d",  gPosition.a))
		}
		setDrive(0, 0);
		break;
	case ccw:
		target = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y) - offset, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + (target) * fullRatio;
		////LOG_AUTO(("%f %f", radToDeg(target), radToDeg(endFull)))
		setDrive(-127, 127);
		while( (gPosition.a > endFull))
		{
			//VEL_CHECK_INC(drive, velAngle);
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		setDrive(-coastPower, coastPower);
		timeStart = nPgmTime;
		while(  (gPosition.a > nearAngle(atan2(x - gPosition.x, y - gPosition.y) + offset, target) + degToRad(stopOffsetDeg)))
		{
			if (DATALOG_TURN != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_TURN + 0, radToDeg(gPosition.a));
				datalogAddValue(DATALOG_TURN + 1, 127);
				datalogDataGroupEnd();
				tRelease();
			}
			sleep(10);
		}
		////LOG_AUTO(("Turn done: %d",  gPosition.a))
		if (harshStop)
		{
			setDrive(20, -20);
			sleep(150);
			////LOG_AUTO(("Break done: %d",  gPosition.a))
		}
		setDrive(0, 0);
		break;
	}

	LOG_AUTO(("Turned to %f %f ATarg:%f Err:%f| %f %f %f. T:%d", y, x, radToDeg(target), radToDeg(target-gPosition.a), gPosition.y, gPosition.x, radToDeg(gPosition.a), (nPgmTime-startTime)));
}
