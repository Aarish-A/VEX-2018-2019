/* Functions */
void followLine(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop)
{
	byte facingDir = facing(x,y,(pi/4));

	if (facingDir)
	{
		sVector currentLocalVector;
		if ((gPosition.x - x) == 0 && correction) //makeLine divides by change-in x - this prevents divide-by-zero error
			correction = 0;

		//General Variables
		word throttle, turn, left, right;
		byte dir;

		//Drive Variables
		const float propKP = 6.0;

		//Correction-Turn Variables
		sVector offsetGlobalVector;
		float offset = 5.0;
		sLine followLine;
		if (correction)
		{
			followLine.p1.x = gPosition.x;
			followLine.p1.y = gPosition.y;
			followLine.p2.x = x;
			followLine.p2.y = y;
			makeLine(followLine);
			writeDebugStreamLine("%d Constructing line to follow: y=%fx+%f", npgmtime, followLine.m, followLine.b);
		}

		do
		{
			VEL_CHECK_INC(drive, velLocalY);

			currentLocalVector.x = gPosition.x - x;
			currentLocalVector.y = gPosition.y - y;

			switch (mode)
			{
				case mttSimple:
				{
					if (abs(currentLocalVector.y) > 3)
						throttle = abs(power) * facingDir;
					else
						throttle = 7 * facingDir;
					break;
				}
				case mttProportional:
				{
					throttle = LIM_TO_VAL((currentLocalVector.y * propKP), 127);
					if (abs(currentLocalVector.y) < 5)
						LIM_TO_VAL_SET(throttle, 15);
					break;
				}
			}
			throttle = abs(throttle) * facingDir;
			LOG(drive)("\t Facing%d, Drive throttle mode:%d, Pwr%f", facingDir, mode, throttle);

			if (correction)
			{
				const float turnBase = 1.42;

				if (abs(currentLocalVector.y) <= offset)
					offset = abs(currentLocalVector.y) - 0.2;

				offsetPos(offsetGlobalVector.x, offsetGlobalVector.y, offset);

				float targX = X_OF_LINE(followLine, offsetGlobalVector.y);
				float errorX = fabs(targX - gPosition.x);

				turn = LIM_TO_VAL(pow(turnBase, errorX), 127) * facingDir;

				if (abs(errorX) > 8 && abs(throttle) > 62)
					throttle /= 2;

				byte dir = sgn(currentLocalVector.x) * sgn(currentLocalVector.y) * facingDir;
				switch(dir)
				{
					case (-1): // turn right
					{
						//LOG(drive)("turn right");
						left = throttle;
						right = throttle + turn;
						break;
					}
					case(1): // turn left
					{
						//LOG(drive)("turn left");
						right = throttle;
						left = throttle + turn;
						break;
					}
					case(0): //straight
					{
						//LOG(drive)("turn straight");
						right = left = throttle;
						break;
					}
				}

			LOG(drive)("%d LocalPos:(%f,%f), OffsetPos(%f,%f), %d, %d, t:%f l:%d r:%d, trttle:%d, trn:%d",npgmtime, currentLocalVector.x, currentLocalVector.y, targX, offsetGlobalVector.y, facingDir, dir, errorX, left, right, throttle, turn);
			}
			else
			{
				left = right = throttle;
			}

			LIM_TO_VAL_SET(left, 127);
			LIM_TO_VAL_SET(right, 127);

			setDrive(left,right);

			sleep(10);
		} WHILE(drive, (abs(currentLocalVector.y) > 0.8) );
		LOG(drive)("%d Done LineFollow(%f, %f)", npgmtime, gPosition.x, gPosition.y);

		LOG(drive)("%d (%f, %f)", npgmtime, gPosition.x, gPosition.y);

		if (harshStop)
			applyHarshStop();

		LOG(drive)("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
	}
}

void moveToTargetSimple(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop)
{
	byte facingDir = facing(x,y,(pi/4))
	if (facingDir)
	{
		power = abs(power) * facingDir; //facingDir is -1 if we need to go backwards, 1 if we are going forwards

		//General Variables
		word throttle, turn, left, right;
		byte dir;

		sVector currentLocalVector;
		sPolar currentLocalPolar;

		sVector turnGlobalVector;
		sVector turnLocalVector;
		sPolar turnLocalPolar;

		//Drive Variables
		const float propKP = 6.0;

		//Turn Variables
		float offset = 4.0;
		const float turnBase = 1.42;
		const float turnKP = -5.0;

		do
		{
			VEL_CHECK_INC(drive, velLocalY);

			currentLocalVector.x = gPosition.x - x;
			currentLocalVector.y = gPosition.y - y;

			switch (mode)
			{
				case mttSimple:
				{
					LOG(drive)("\t simple drive throttle");
					if (abs(currentLocalVector.y) > 3)
						throttle = abs(power) * facingDir;
					else
						throttle = 7 * facingDir;
					break;
				}
				case mttProportional:
				{
					throttle = LIM_TO_VAL((currentLocalVector.y * propKP), 127);
					LOG(drive)("\t prop drive throttle pwr%f", throttle);
					if (abs(currentLocalVector.y) < 5)
						LIM_TO_VAL_SET(throttle, 15);
					break;
				}
			}

			throttle = abs(throttle) * facingDir;

			//if (abs(currentLocalVector.y) < 4 && correction)
			//	correction = 0;

			if (correction)
			{

				if (abs(currentLocalVector.y) <= offset)
					offset = abs(currentLocalVector.y) - 0.2;

				offsetPos(turnGlobalVector.x, turnGlobalVector.y, offset);

				turnLocalVector.x = turnGlobalVector.x - x;
				turnLocalVector.y = turnGlobalVector.y - y;

				if (abs(turnLocalVector.x) > 8 && abs(throttle) > 62)
					throttle /= 2;

				if (fabs(turnLocalVector.x) > 2 && fabs(turnLocalVector.x) < 7)
					turn = LIM_TO_VAL((fabs(turnLocalVector.x) * 4), 127) * facingDir;
				else
					turn = LIM_TO_VAL(pow(turnBase, fabs(turnLocalVector.x)), 127) * facingDir;

				//if (driveStateCycCount == 1)
				dir = sgn(turnLocalVector.x) * sgn(turnLocalVector.y) * facingDir;
				//LOG(drive)("gTurn:%f, lTurn:%f, dir:%d, turn:%d", turnGlobalVector.x, turnLocalVector.x, dir, turn);

				//if (abs(turnLocalVector.x) < 2) //when within two inches of target, go straight
					//dir = 0;

				switch(dir)
				{
					case (-1): // turn right
					{
						left = throttle;
						right = throttle + turn;
						break;
					}
					case(1): // turn left
					{
						right = throttle;
						left = throttle + turn;
						break;
					}
					case(0): //straight
					{
						right = left = throttle;
						break;
					}
				}
			}
			else
			{
				left = right = throttle;
			}

			LIM_TO_VAL_SET(left, 127);
			LIM_TO_VAL_SET(right, 127);

			setDrive(left,right);

			LOG(drive)("loc coord:(%f,%f), %d, %d, t:%f l:%d r:%d, throttle:%d, turn:%d", currentLocalVector.x, currentLocalVector.y, facingDir, dir, turnLocalVector.x, left, right, throttle, turn);

			sleep(10);
		} WHILE(drive, (abs(currentLocalVector.y) > 0.8) );
		LOG(drive)("%d (%f, %f)", npgmtime, gPosition.x, gPosition.y);

		if (harshStop)
			applyHarshStop();

		LOG(drive)("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
	}
	else
	{
		LOG(drive)("%d Move Simple Exit - Not Facing", npgmtime);
	}
}

void moveToTarget(float x, float y, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
	LOG(drive)("Moving to %f %f from %f %f at %d", y, x, ys, xs, power);

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
	LOG(drive)("Line length: %.2f", lineLength);
	float lineAngle = getAngleOfLine(followLine); // Get the angle of the line that we're following relative to the vertical
	float pidAngle = nearAngle(lineAngle - (power < 0 ? PI : 0), gPosition.a);
	LOG(drive)("Line | Pid angle: %f | %f", radToDeg(lineAngle), radToDeg(pidAngle));

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
		VEL_CHECK_INC(drive, velLocalY);

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
			correction = fabs(errX) > maxErrX ? 8.0 * (nearAngle(correctA, gPosition.a) - gPosition.a) * sgn(power) : 0;
		}

		if (mode != mttSimple)
		{
			switch (mode)
			{
			case mttProportional:
				finalPower = round(-127.0 / 40.0 * currentPosVector.y) * sgn(power);
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
				float vTarget = 45 * (1 - exp(0.07 * (currentPosVector.y + dropEarly)));
				finalPower = round(kB * vTarget + kP * (vTarget - vel)) * sgn(power);
				break;
			}
			LIM_TO_VAL_SET(finalPower, abs(power));
			if (finalPower * sgn(power) < 30)
				finalPower = 30 * sgn(power);
			word delta = finalPower - last;
			LIM_TO_VAL_SET(delta, 5);
			finalPower = last += delta;
		}

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

		endCycle(cycle);
	} WHILE(drive, (currentPosVector.y < -dropEarly - MAX((vel * ((stopType & stopSoft) ? 0.175 : 0.098)), decelEarly)) );

	LOG(drive)("%f %f", currentPosVector.y, vel);

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
	} WHILE(drive,  (currentPosVector.y < -dropEarly - (vel * ((stopType & stopSoft) ? 0.175 : 0.098))));

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
		} WHILE(drive,  (vel > 7 && currentPosVector.y < 0));
	}

	if (stopType & stopHarsh)
		applyHarshStop();
	else
		setDrive(0, 0);

	LOG(drive)("Moved to %f %f from %f %f | %f %f %f", y, x, ys, xs, gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void moveToTargetDis(float a, float d, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
	moveToTarget(xs + d * sin(a), ys + d * cos(a), xs, ys, power, startPower, maxErrX, decelEarly, decelPower, dropEarly, stopType, mode);
}

void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo, bool harshStop)
{
	LOG(drive)("Turning to %f", radToDeg(a));


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
		WHILE(drive, (gPosition.a < endFull))
		{
			driveVelSafetyCheck()
			if (DATALOG_TURN != -1)
			{
				VEL_CHECK_INC(drive, velAngle);
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
		WHILE(drive, (gPosition.a < a - degToRad(stopOffsetDeg)))
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
		LOG(drive)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(-20, 20);
			sleep(150);
			LOG(drive)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	case ccw:
		a = gPosition.a - fmod(gPosition.a - a, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + a * fullRatio;
		setDrive(-127, 127);
		WHILE(drive, (gPosition.a > endFull))
		{
			VEL_CHECK_INC(drive, velAngle);
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
		timeStart = npgmTime;
		WHILE(drive,  (gPosition.a > a + degToRad(stopOffsetDeg)))
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
		LOG(drive)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(20, -20);
			sleep(150);
			LOG(drive)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	}
	LOG(drive)("Turned to %f | %f %f %f", radToDeg(a), gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void turnToTargetNewAlg(float x, float y, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo, bool harshStop, float offset)
{
	LOG(drive)("Turning to %f %f", y, x);

	if (turnDir == ch)
		if (fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

	float endFull, target;


	unsigned long timeStart =  nPgmTime;

	switch (turnDir)
	{
	case cw:
		target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + target * fullRatio;
		LOG(drive)("%f %f", radToDeg(target), radToDeg(endFull));
		setDrive(127, -127);
		WHILE(drive, (gPosition.a < endFull ))
		{
			VEL_CHECK_INC(drive, velAngle);
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
		timeStart = npgmTime;
		WHILE(drive, (gPosition.a < nearAngle(atan2(x - gPosition.x, y - gPosition.y) + offset, target) - degToRad(stopOffsetDeg)) )
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
		LOG(drive)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(-20, 20);
			sleep(150);
			LOG(drive)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	case ccw:
		target = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y) - offset, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + (target) * fullRatio;
		LOG(drive)("%f %f", radToDeg(target), radToDeg(endFull));
		setDrive(-127, 127);
		WHILE(drive, (gPosition.a > endFull))
		{
			VEL_CHECK_INC(drive, velAngle);
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
		WHILE(drive,  (gPosition.a > nearAngle(atan2(x - gPosition.x, y - gPosition.y) + offset, target) + degToRad(stopOffsetDeg)))
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
		LOG(drive)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(20, -20);
			sleep(150);
			LOG(drive)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	}
	LOG(drive)("Turned to %f %f | %f %f %f", y, x, gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void sweepTurnToTarget(float x, float y, float a, float r, tTurnDir turnDir, byte power, bool slow)
{
	sVector vector;
	sPolar polar;


	if (turnDir == ch)
	{
		vector.x = gPosition.x - x;
		vector.y = gPosition.y - y;
		vectorToPolar(vector, polar);
		polar.angle += a;
		polarToVector(polar, vector);

		turnDir = vector.x > 0 ? cw : ccw;
	}

	float yOrigin, xOrigin;
	float linearV, angularV, angularVLast = 0;
	float localR, localA;

	const float kR = 15.0;
	const float kA = 5.0;
	const float kB = 60.0;
	const float kP = 30.0;
	const float kD = 2000.0;

	sCycleData cycle;
	initCycle(cycle, 40, "sweepTurnToTarget");

	unsigned long timeStart = nPgmTime;
	switch (turnDir)
	{
	case cw:
		vector.y = 0;
		vector.x = r;
		vectorToPolar(vector, polar);
		polar.angle -= a;
		polarToVector(polar, vector);
		yOrigin = y + vector.y;
		xOrigin = x + vector.x;

		localA = atan2(gPosition.x - xOrigin, gPosition.y - yOrigin);

		a = nearAngle(a, power > 0 ? gPosition.a : (gPosition.a + PI));

		LOG(drive)("%d Sweep to %f around %f %f", nPgmTime, radToDeg(a), yOrigin, xOrigin);

		do
		{
			VEL_CHECK_INC(drive, velAngle);
			float aGlobal = gPosition.a;
			if (power < 0)
				aGlobal += PI;
			angularV = gVelocity.a;
			float _y = gPosition.y - yOrigin;
			float _x = gPosition.x - xOrigin;
			localR = sqrt(_y * _y + _x * _x);
			localA = nearAngle(atan2(_x, _y), localA);
			linearV = gVelocity.x * sin(localA + PI / 2) + gVelocity.y * cos(localA + PI / 2);

			float target = MAX(linearV, 15) / localR + kR * log(localR / r) + kA * (nearAngle(localA + PI / 2, aGlobal) - aGlobal);
			word turn = round(kB * target + kP * (target - angularV) + kD * (angularVLast - angularV) / 40);
			angularVLast = angularV;

			if (turn < 0)
				turn = 0;
			else if (turn > 150)
				turn = 150;

			if (power > 0)
				setDrive(power, power - turn);
			else
				setDrive(power + turn, power);

			if (DATALOG_SWEEP != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_SWEEP + 0, localR * 100);
				datalogAddValue(DATALOG_SWEEP + 1, radToDeg(localA) * 10);
				datalogAddValue(DATALOG_SWEEP + 2, radToDeg(target) * 10);
				datalogAddValue(DATALOG_SWEEP + 3, turn * 10);
				datalogAddValue(DATALOG_SWEEP + 4, linearV * 10);
				datalogAddValue(DATALOG_SWEEP + 5, radToDeg(angularV) * 10);
				datalogDataGroupEnd();
				tRelease();
			}

			endCycle(cycle);
		} WHILE(drive,  ((power > 0 ? gPosition.a : (gPosition.a + PI)) - a < (slow ? -0.1 : -0.15)));
		break;
	case ccw:
		vector.y = 0;
		vector.x = r;
		vectorToPolar(vector, polar);
		polar.angle += a;
		polarToVector(polar, vector);
		yOrigin = y + vector.y;
		xOrigin = x + vector.x;

		localA = atan2(gPosition.x - xOrigin, gPosition.y - yOrigin);

		a = nearAngle(a, power > 0 ? gPosition.a : (gPosition.a + PI));

		LOG(drive)("%d Sweep to %f around %f %f", nPgmTime, radToDeg(a), yOrigin, xOrigin);

		do
		{
			VEL_CHECK_INC(drive, velAngle);
			float aGlobal = gPosition.a;
			if (power < 0)
				aGlobal += PI;
			angularV = gVelocity.a;
			float _y = gPosition.y - yOrigin;
			float _x = gPosition.x - xOrigin;
			localR = sqrt(_y * _y + _x * _x);
			localA = nearAngle(atan2(_x, _y), localA);
			linearV = gVelocity.x * sin(localA - PI / 2) + gVelocity.y * cos(localA - PI / 2);

			float target = -MAX(linearV, 15) / localR + kR * log(r / localR) + kA * (nearAngle(localA - PI / 2, aGlobal) - aGlobal);
			word turn = round(kB * target + kP * (target - angularV) + kD * (angularVLast - angularV) / 40);
			angularVLast = angularV;

			if (turn > 0)
				turn = 0;
			else if (turn < -150)
				turn = -150;

			if (power > 0)
				setDrive(power + turn, power);
			else
				setDrive(power, power - turn);

			if (DATALOG_SWEEP != -1)
			{
				tHog();
				datalogDataGroupStart();
				datalogAddValue(DATALOG_SWEEP + 0, localR * 100);
				datalogAddValue(DATALOG_SWEEP + 1, radToDeg(localA) * 10);
				datalogAddValue(DATALOG_SWEEP + 2, radToDeg(target) * 10);
				datalogAddValue(DATALOG_SWEEP + 3, turn * 10);
				datalogAddValue(DATALOG_SWEEP + 4, linearV * 10);
				datalogAddValue(DATALOG_SWEEP + 5, radToDeg(angularV) * 10);
				datalogDataGroupEnd();
				tRelease();
			}

			endCycle(cycle);
		} WHILE(drive,  ((power > 0 ? gPosition.a : (gPosition.a + PI)) - a > (slow ? 0.1 : 0.15)));
		break;
	}
	setDrive(0, 0);
	LOG(drive)("%d Done sweep turn", nPgmTime);
}
