/* Functions */
bool autoLogs = 0;
bool excelLogs = 1;

//TODO: Test Offset Calc
//TODO: Make local y be delta x - if delta x is larger than delta y
//TODO: Make the while loop end when abs(currentLocalPos.hypotenuse) < 0.5

void  moveToTargetAngle(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType)
{
	byte facingDir = facingCoord(x,y,1.13);

	float deltaY = gPosition.y-y;
	if (facingDir && fabs(deltaY)>4 )
	{
		//Vectors & Magnitudes - relative to the end coordinate
		sTrianglePos currentLocalPos; //constructTrianglePos(currentLocalPos, gPosition.x - x, gPosition.y - y);
		sTrianglePos offsetLocalPos;

		//General Variables
		word throttle, turn, left, right;

		//Drive Variables
		const float propKP = 6.6;
		float softExit = LIM_TO_VAL(fabs(deltaY/4.5) + 1, 12);

		//Correction-Turn Variables
		float curLineAngle;
		sLine followLine;
		tTurnDir turnDir;
		//const float turnKP = 5.0;

		sCycleData cycle;
		initCycle(cycle, 40, "followLine");

		if (facingDir && fabs(deltaY) > 4)
		{
			do
			{
				VEL_CHECK_INC(drive, velLocalY);
				constructTrianglePos(currentLocalPos, gPosition.y - y, gPosition.x - x, true);
				tHog();

				switch (mode)
				{
				case mttSimple:
					{
						if (fabs(currentLocalPos.vector.y) > 3)
							throttle = abs(power) * facingDir;
						else
							throttle = 7 * facingDir;
						break;
					}
				case mttProportional:
					{
						throttle = LIM_TO_VAL((currentLocalPos.vector.y * propKP), 127);
						if (fabs(currentLocalPos.vector.y) < 5)
							LIM_TO_VAL_SET(throttle, 15);
						break;
					}
				}
				throttle = LIM_TO_VAL(abs(throttle) * facingDir, 127);

				if (correction)
				{
					if (fmod(currentLocalPos.a - gPosition.a, PI * 2) < PI) turnDir = ccw; else turnDir = cw;
					turnDir *= facingDir;
					if (abs(turnDir) < 0.05) turnDir = 0;

					float angleErr = nearAngle(currentLocalPos.a, gPosition.a) - gPosition.a;

					float turn = LIM_TO_VAL(10.0 * exp(angleErr), 127) * facingDir;

					switch(turnDir)
					{
					case (ccw): // turn left
						//LOG(auto)("turn right");
						right = LIM_TO_VAL(throttle + turn, 127);
						left = MIN_LIM_TO_VAL(right - (2*turn), 5, throttle);
						break;
					case(cw): // turn right
						//LOG(auto)("turn left");
						left = LIM_TO_VAL(throttle + turn, 127);
						right = MIN_LIM_TO_VAL(left - (2*turn), 5, throttle);
						break;
					case(0): //straight
						//LOG(auto)("turn straight");
						right = left = throttle;
						break;
					}
				}
				else
				{
					left = right = throttle;
				}

				LIM_TO_VAL_SET(left, 127);
				LIM_TO_VAL_SET(right, 127);

				setDrive(left,right);
				tRelease();

				endCycle(cycle);
			}DO_WHILE(drive, ( fabs(currentLocalPos.vector.y) > ((stopType & stopSoft)? softExit : 0.8) ));

		}
	}
}

void followLineVec(float x, float y, float offsetA, byte power, tMttMode mode, bool correction, tStopType stopType)
{
	byte facingDir = facingCoord(x,y,1.13); // Must be facing within 130 degrees of target

	bool invertAxes = false;
	float deltaX = gPosition.x - x;
	float deltaY = gPosition.y - y;
	/*
	if (fabs(deltaX) > fabs(deltaY))
	{
	invertAxes = true;
	float temp = deltaX;
	deltaX = deltaY;
	deltaY = temp;
	}
	*/
	if (facingDir && fabs(deltaY) > 4)
	{
		//Vectors & Magnitudes - relative to the end coordinate
		sTrianglePos currentLocalPos; //constructTrianglePos(currentLocalPos, gPosition.x - x, gPosition.y - y);
		sTrianglePos offsetLocalPos;
		sTrianglePos targetLocalPos;
		sTrianglePos error;

		//Angle of the line we are following - relative to vertical
		if (!invertAxes)
			const float a = atan2(deltaX, deltaY);
		//else
		//	const float a = atan2(deltaY, deltaX);
		const float cosA = cos(a);
		const float sinA = sin(a);

		//General Variables
		word throttle, turn, left, right;

		//Drive Variables
		const float propKP = 6.6;
		float softExit = LIM_TO_VAL(fabs(deltaY/4.5) + 1, 12);

		//Correction-Turn Variables
		float offset = ( offsetA==-1? (S_DISTANCE_IN+10.0) : offsetA);
		float curLineAngle;
		sLine followLine;
		tTurnDir turnDir;

		sCycleData cycle;
		initCycle(cycle, 40, "followLine");

		LOG(auto)("%dStart FollowLine to (%f,%f),a:%f. Offset:%f. softExit:%f", npgmtime, deltaX, deltaY, a, offset, softExit);
		LOG(excel)("%dStart FollowLine to (%f,%f),a:%f. Offset:%f. softExit:%f", npgmtime, deltaX, deltaY, a, offset, softExit);
		do
		{
			VEL_CHECK_INC(drive, velLocalY);

			//Construct triangle connecting end point and robot position
			if (!invertAxes)
				constructTrianglePos(currentLocalPos, gPosition.x - x, gPosition.y - y);
			else
				constructTrianglePos(currentLocalPos, gPosition.y - y, gPosition.x - x);

			switch (mode)
			{
			case mttSimple:
				{
					if (fabs(currentLocalPos.vector.y) > 3)
						throttle = abs(power) * facingDir;
					else
						throttle = 7 * facingDir;
					break;
				}
			case mttProportional:
				{
					throttle = LIM_TO_VAL((currentLocalPos.vector.y * propKP), 127);
					if (fabs(currentLocalPos.vector.y) < 5)
						LIM_TO_VAL_SET(throttle, 15);
					break;
				}
			}
			throttle = LIM_TO_VAL(abs(throttle) * facingDir, 127);
			//LOG(auto)("\t Facing%d, Drive throttle mode:%d, Pwr%f", facingDir, mode, throttle);

			if (correction)
			{
				const float turnBase = 1.42;
				tHog();
				//setCurLineAngle
				if (!invertAxes)
					curLineAngle = atan2(deltaX, deltaY);
				/*
				else
				curLineAngle = atan2(deltaY, deltaX);
				*/
				//Make offset value smaller as when we are close to the target
				if (abs(currentLocalPos.vector.y) <= (offset+2))
				{
					offset = abs(currentLocalPos.vector.y) - softExit;
					LOG(auto)("\t\t Offset Reset - %f", offset);
				}

				//Construct triangle connecting end point and offset position
				sVector localOffset;
				localOffset.x = offset * sin(gPosition.a);
				localOffset.y = offset * cos(gPosition.a);
				constructTrianglePos(offsetLocalPos, currentLocalPos.vector.x+localOffset.x, currentLocalPos.vector.y+localOffset.y);
				LOG(auto)("\t\tOffsetPos:(%f,%f),hyp(%f), localOffset(%f,%f)", offsetLocalPos.vector.x, offsetLocalPos.vector.y, offsetLocalPos.hypotenuse, localOffset.x, localOffset.y);

				//Construct triangle connecting end point and target position
				targetLocalPos.hypotenuse = offsetLocalPos.hypotenuse;
				targetLocalPos.vector.x = targetLocalPos.hypotenuse * sinA;
				targetLocalPos.vector.y = targetLocalPos.hypotenuse * cosA;
				LOG(auto)("\t\tTargetPos:(%f,%f),hyp(%f)", targetLocalPos.vector.x, targetLocalPos.vector.y, targetLocalPos.hypotenuse);

				//Hypotenuse in the error triange is the error
				constructTrianglePos(error, offsetLocalPos.vector.x - targetLocalPos.vector.x, offsetLocalPos.vector.y - targetLocalPos.vector.y);
				float errorVal = fabs(error.hypotenuse);
				LOG(auto)("\t\tErrorPos:(%f,%f),hyp(%f)", error.vector.x, error.vector.y, error.hypotenuse);


				if (fabs(errorVal) <= 1.5)
					turn = 0;
				else
					turn = LIM_TO_VAL( ((float)5.5 * (exp(0.2 * errorVal))), 127); //turn = 4.5(e^(0.2x))
				turn *= facingDir;

				if (fmod(curLineAngle - gPosition.a, PI * 2) < PI) turnDir = ccw; else turnDir = cw;
				turnDir *= facingDir;

				//byte dir = sgn(currentLocalPos.vector.x) * sgn(currentLocalPos.vector.y) * facingDir;
				//if (abs(simplifyAngle(gPosition.a)) > pi/4 && abs(simplifyAngle(gPosition.a)) < 0.75*pi)
				//	dir *= -1; //Toggle dir if we are horizontal
				switch(turnDir)
				{
				case (ccw): // turn left
					{
						//LOG(auto)("turn right");
						right = LIM_TO_VAL(throttle + turn, 127);
						left = MIN_LIM_TO_VAL(right - (2*turn), 5, throttle);
						break;
					}
				case(cw): // turn right
					{
						//LOG(auto)("turn left");
						left = LIM_TO_VAL(throttle + turn, 127);
						right = MIN_LIM_TO_VAL(left - (2*turn), 5, throttle);
						break;
					}
				case(0): //straight
					{
						//LOG(auto)("turn straight");
						right = left = throttle;
						break;
					}
				}
				LOG(auto)("%d Err:%f, D:%f,LocalPos:(%f,%f), vel:%f f:%f t:%f, l:%d r:%d, trttle:%d, trn:%d",npgmtime, error.hypotenuse, currentLocalPos.hypotenuse, currentLocalPos.vector.x, currentLocalPos.vector.y, gVelocity.localY, facingDir, turnDir, left, right, throttle, turn);
				//LOG(auto)("%d Err:%f, LocalPos:(%f,%f), OffsetPos(%f,%f), TargPos(%f,%f), vel:%f, l:%d r:%d, trttle:%d, trn:%d",npgmtime, error.hypotenuse, currentLocalPos.vector.x, currentLocalPos.vector.y, offsetLocalPos.vector.x, offsetLocalPos.vector.y, targetLocalPos.vector.x, targetLocalPos.vector.y, gVelocity.localY, facingDir, dir, errorVal, left, right, throttle, turn);
				//LOG(excel)("left:%d,right%d,throttle%d, turn%d",left, right, throttle, turn);
				LOG(excel)("%f, %f, %f, %f, %f, %f, %f, %f, 0, %f, %f, %f, %f", currentLocalPos.vector.x, currentLocalPos.vector.y, offsetLocalPos.vector.x, offsetLocalPos.vector.y, targetLocalPos.vector.x, targetLocalPos.vector.y, error.vector.x, error.vector.y, error.hypotenuse, turnDir, left, right);
			}
			else
			{
				left = right = throttle;
			}

			LIM_TO_VAL_SET(left, 127);
			LIM_TO_VAL_SET(right, 127);

			setDrive(left,right);
			tRelease();

			endCycle(cycle);
		} DO_WHILE(drive, ( fabs(currentLocalPos.vector.y) > ((stopType & stopSoft)? softExit : 0.8) ));

		LOG(auto)("%d Done LineFollow(%f, %f)", npgmtime, gPosition.x, gPosition.y);
		//LOG(excel)("Done lf gPos(%f,%f)a:%f,curA:%f, turn:%d", gPosition.x, gPosition.y, gPosition.a, curLineAngle, turnDir);
		LOG(excel)(",,,,,,,,,,,,,Done lf,%f,%f", currentLocalPos.vector.x, currentLocalPos.vector.y);
		if (stopType & stopSoft)
		{
			//Construct triangle connecting end point and robot position
			if (!invertAxes)
				constructTrianglePos(currentLocalPos, gPosition.x - x, gPosition.y - y);
			else
				constructTrianglePos(currentLocalPos, gPosition.y - y, gPosition.x - x);
			//

			LOG(auto)("%d Starting LineFollow stopSoft(%f,%f), vel:%f", npgmtime, gPosition.x, gPosition.y, gVelocity.localY);

			WHILE(drive, fabs(currentLocalPos.vector.y) > 0.6 && fabs(gVelocity.localY) > 0.3)
			{
				throttle = facingDir * -6;
				setDrive(throttle, throttle);
				LOG(excel)("%f, %f, %f, %f, %f, %f, %f, %f, 0, %f, %f, %f, %f", currentLocalPos.vector.x, currentLocalPos.vector.y, offsetLocalPos.vector.x, offsetLocalPos.vector.y, targetLocalPos.vector.x, targetLocalPos.vector.y, error.vector.x, error.vector.y, error.hypotenuse, turnDir, left, right);
				//LOG(auto)("%d LocalPos:(%f,%f), %d, %d, pow:%d",npgmtime, currentLocalPos.vector.x, currentLocalPos.vector.y, facingDir, dir, throttle);

				sleep(10);
			}
		}

		LOG(auto)("%d Done LineFollow stopSoft(%f, %f)", npgmtime, gPosition.x, gPosition.y);

		if (stopHarsh & stopType)
			applyHarshStop();
		else
			setDrive(0,0);

		LOG_2(auto, excel)("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
	}
}

void followLine(float x, float y, byte power, tMttMode mode, bool correction, tStopType stopType)
{
	byte facingDir = facingCoord(x,y,(pi/4));

	if (facingDir)
	{
		sVector currentLocalVector;
		//if ((gPosition.x - x) == 0) //makeLine divides by change-in x - this prevents divide-by-zero error
		//	correction = 0;

		//General Variables
		word throttle, turn, left, right;
		byte dir;

		//Drive Variables
		const float propKP = 6.0;
		float softExit = 3;

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
			/*
			if (stopSoft & stopType) //Determine how many inches before target to begin softStop
			{
			if (gVelocity.localY > 6)
			softExit = 10
			else if(gVelocity.localY > 2)
			softExit = 3.5;
			else
			softExit = 1.5;
			}
			*/
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
			LOG(auto)("\t Facing%d, Drive throttle mode:%d, Pwr%f", facingDir, mode, throttle);

			if (correction)
			{
				//const float turnBase = 1.42;

				offsetPos(offsetGlobalVector.x, offsetGlobalVector.y, offset);
				if (abs(currentLocalVector.y) <= (offset+2))
				{
					offset = abs(currentLocalVector.y) - softExit;
					LOG(auto)("\t\t Offset Reset - %f", offset);
				}

				float targX = findX(followLine, offsetGlobalVector.y);
				float errorX = fabs(offsetGlobalVector.x - targX);

				//turn = LIM_TO_VAL(pow(turnBase, errorX), 127);
				if (fabs(errorX) <= 1)
					turn = 0;
				else
					turn = LIM_TO_VAL( ((float)3.5 * (exp(0.2 * errorX))), 127); //turn = 3.5(e^(0.2x))

				turn *= facingDir;

				if (abs(errorX) > 6 && abs(throttle) > 65)
					throttle /= 2;

				byte dir = sgn(currentLocalVector.x) * sgn(currentLocalVector.y) * facingDir;
				switch(dir)
				{
				case (-1): // turn right
					{
						//LOG(auto)("turn right");
						left = throttle;
						right = throttle + turn;
						break;
					}
				case(1): // turn left
					{
						//LOG(auto)("turn left");
						right = throttle;
						left = throttle + turn;
						break;
					}
				case(0): //straight
					{
						//LOG(auto)("turn straight");
						right = left = throttle;
						break;
					}
				}

				LOG(auto)("%d LocalPos:(%f,%f), targ:%f - projx:%f = error%f, vel:%f, %d, %d, t:%f l:%d r:%d, trttle:%d, trn:%d",npgmtime, currentLocalVector.x, currentLocalVector.y, targX, offsetGlobalVector.x, errorX, gVelocity.localY, facingDir, dir, errorX, left, right, throttle, turn);
			}
			else
			{
				left = right = throttle;
			}

			LIM_TO_VAL_SET(left, 127);
			LIM_TO_VAL_SET(right, 127);

			setDrive(left,right);

			sleep(10);
		} DO_WHILE(drive, ( abs(currentLocalVector.y) > ((stopType & stopSoft)? softExit : 0.8) ));

		LOG(auto)("%d Done LineFollow(%f, %f)", npgmtime, gPosition.x, gPosition.y);

		if (stopType & stopSoft)
		{
			currentLocalVector.x = gPosition.x - x;
			currentLocalVector.y = gPosition.y - y;

			LOG(auto)("%d Starting LineFollow stopSoft(%f,%f), vel:%f", npgmtime, gPosition.x, gPosition.y, gVelocity.localY);

			WHILE(drive, abs(currentLocalVector.y) > 0.6 && abs(gVelocity.localY) > 0.3)
			{
				throttle = facingDir * -6;
				setDrive(throttle, throttle);
				LOG(auto)("%d LocalPos:(%f,%f), %d, %d, pow:%d",npgmtime, currentLocalVector.x, currentLocalVector.y, facingDir, dir, throttle);

				sleep(10);
			}
		}

		LOG(auto)("%d Done LineFollow stopSoft(%f, %f)", npgmtime, gPosition.x, gPosition.y);

		if (stopHarsh & stopType)
			applyHarshStop();
		else
			setDrive(0,0);

		LOG(auto)("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
	}
}

void turnToFace(float x, float y, tFacingDir facingDir, tStopType stopType)
{
	float curX = gPosition.x;
	float curY = gPosition.y;
	float gAngleToFront = aTan2((x-curX),(y-curY));
	float gAngleToBack = aTan2((curX-x),(curY-y));

	LOG(auto)("%d Begin turnToFace(%f,%f)", npgmtime, x, y);

	tTurnDir turnDir;
	if (facingDir == facingFront)
		if (fmod(gPosition.a - gAngleToFront, PI * 2) < PI) turnDir = ccw; else turnDir = cw;
	else if (facingDir == facingBack)
		if (fmod(gPosition.a - gAngleToBack, PI * 2) < PI) turnDir = ccw; else turnDir = cw;
	else
		LOG(auto)("%d Exit turnToFace. facingDir must be facingFront or facingBack", npgmtime);

	if (turnDir == cw)
		setDrive(90, -90);
	else
		setDrive(90, 90);

	WHILE(drive, (facingCoord(x, y, 0.2) != facingDir))
	{
		sleep(10);
	}

	if (stopType & stopHarsh)
		applyHarshStop();
	else
		setDrive(0,0);
}

void moveToTargetSimple(float x, float y, byte power, tMttMode mode, bool correction, bool harshStop)
{
	byte facingDir = facingCoord(x,y,(pi/4))
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
					LOG(auto)("\t simple drive throttle");
					if (abs(currentLocalVector.y) > 3)
						throttle = abs(power) * facingDir;
					else
						throttle = 7 * facingDir;
					break;
				}
			case mttProportional:
				{
					throttle = LIM_TO_VAL((currentLocalVector.y * propKP), 127);
					LOG(auto)("\t prop drive throttle pwr%f", throttle);
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
				//if (fmod(a - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;
				dir = sgn(turnLocalVector.x) * sgn(turnLocalVector.y) * facingDir;
				if (abs(turnLocalVector.x) < 2) //when within two inches of target, go straight
					dir = 0;

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

			LOG(auto)("loc coord:(%f,%f), %d, %d, t:%f l:%d r:%d, throttle:%d, turn:%d", currentLocalVector.x, currentLocalVector.y, facingDir, dir, turnLocalVector.x, left, right, throttle, turn);

			sleep(10);
		} DO_WHILE(drive, (abs(currentLocalVector.y) > 0.8) );
		LOG(auto)("%d (%f, %f)", npgmtime, gPosition.x, gPosition.y);

		if (harshStop)
			applyHarshStop();

		LOG(auto)("%d After harsh stop:(%f, %f)", npgmtime, gPosition.x, gPosition.y);
	}
	else
	{
		LOG(auto)("%d Move Simple Exit - Not Facing", npgmtime);
	}
}

void moveToTarget(float x, float y, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
	LOG(auto)("Moving to %f %f from %f %f at %d", y, x, ys, xs, power);

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
	LOG(auto)("Line length: %.2f", lineLength);
	float lineAngle = getAngleOfLine(followLine); // Get the angle of the line that we're following relative to the vertical
	float pidAngle = nearAngle(lineAngle - (power < 0 ? PI : 0), gPosition.a);
	LOG(auto)("Line | Pid angle: %f | %f", radToDeg(lineAngle), radToDeg(pidAngle));

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
	} DO_WHILE(drive, (currentPosVector.y < -dropEarly - MAX((vel * ((stopType & stopSoft) ? 0.175 : 0.098)), decelEarly)) );

	LOG(auto)("%f %f", currentPosVector.y, vel);

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
	}DO_WHILE(drive,  (currentPosVector.y < -dropEarly - (vel * ((stopType & stopSoft) ? 0.175 : 0.098))));

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
		} DO_WHILE(drive,  (vel > 7 && currentPosVector.y < 0));
	}

	if (stopType & stopHarsh)
		applyHarshStop();
	else
		setDrive(0, 0);

	LOG(auto)("Moved to %f %f from %f %f | %f %f %f", y, x, ys, xs, gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void moveToTargetDis(float a, float d, float xs, float ys, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly, tStopType stopType, tMttMode mode)
{
	moveToTarget(xs + d * sin(a), ys + d * cos(a), xs, ys, power, startPower, maxErrX, decelEarly, decelPower, dropEarly, stopType, mode);
}

void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo, bool harshStop)
{
	LOG(auto)("Turning to %f", radToDeg(a));


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
		LOG(auto)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(-20, 20);
			sleep(150);
			LOG(auto)("Break done: %d",  gPosition.a);
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
		LOG(auto)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(20, -20);
			sleep(150);
			LOG(auto)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	}
	LOG(auto)("Turned to %f | %f %f %f", radToDeg(a), gPosition.y, gPosition.x, radToDeg(gPosition.a));
}

void turnToTargetNewAlg(float x, float y, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo, bool harshStop, float offset)
{
	LOG(auto)("Turning to %f %f", y, x);

	if (turnDir == ch)
		if (fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2) > PI) turnDir = ccw; else turnDir = cw;

	float endFull, target;


	unsigned long timeStart =  nPgmTime;

	switch (turnDir)
	{
	case cw:
		target = gPosition.a + fmod(atan2(x - gPosition.x, y - gPosition.y) + offset - gPosition.a, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + target * fullRatio;
		LOG(auto)("%f %f", radToDeg(target), radToDeg(endFull));
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
		LOG(auto)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(-20, 20);
			sleep(150);
			LOG(auto)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	case ccw:
		target = gPosition.a - fmod(gPosition.a - atan2(x - gPosition.x, y - gPosition.y) - offset, PI * 2);
		endFull = gPosition.a * (1 - fullRatio) + (target) * fullRatio;
		LOG(auto)("%f %f", radToDeg(target), radToDeg(endFull));
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
		LOG(auto)("Turn done: %d",  gPosition.a);
		if (harshStop)
		{
			setDrive(20, -20);
			sleep(150);
			LOG(auto)("Break done: %d",  gPosition.a);
		}
		setDrive(0, 0);
		break;
	}
	LOG(auto)("Turned to %f %f | %f %f %f", y, x, gPosition.y, gPosition.x, radToDeg(gPosition.a));
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

		LOG(auto)("%d Sweep to %f around %f %f", nPgmTime, radToDeg(a), yOrigin, xOrigin);

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
		} DO_WHILE(drive,  ((power > 0 ? gPosition.a : (gPosition.a + PI)) - a < (slow ? -0.1 : -0.15)));
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

		LOG(auto)("%d Sweep to %f around %f %f", nPgmTime, radToDeg(a), yOrigin, xOrigin);

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
		} DO_WHILE(drive,  ((power > 0 ? gPosition.a : (gPosition.a + PI)) - a > (slow ? 0.1 : 0.15)));
		break;
	}
	setDrive(0, 0);
	LOG(auto)("%d Done sweep turn", nPgmTime);
}
