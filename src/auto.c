/* Tracking Functions/Tasks */
void trackPosition(int left, int right, int back, sPos& position)
{
	tHog();
	float L = (left - position.leftLst) * SPIN_TO_IN_LR; // The amount the left side of the robot moved
	float R = (right - position.rightLst) * SPIN_TO_IN_LR; // The amount the right side of the robot moved
	float S = (back - position.backLst) * SPIN_TO_IN_S; // The amount the back side of the robot moved

	float LTurn = (left - position.leftStart) * SPIN_TO_IN_LR; // The amount the left side of the robot moved since the beginning
	float RTurn = (right - position.rightStart) * SPIN_TO_IN_LR; // The amount the right side of the robot moved since the beginning

	// Update the last values
	position.leftLst = left;
	position.rightLst = right;
	position.backLst = back;

	float h; // The hypotenuse of the triangle formed by the middle of the robot on the starting position and ending position and the middle of the circle it travels around
	float i; // Half on the angle that I've traveled
	float h2; // The same as h but using the back instead of the side wheels
	float a = (L - R) / (L_DISTANCE_IN + R_DISTANCE_IN); // The angle that I've traveled
	tRelease();

	if (a)
	{
		float r = R / a; // The radius of the circle the robot travel's around with the right side of the robot
		i = a / 2.0;
		float sinI = sin(i);
		h = ((r + R_DISTANCE_IN) * sinI) * 2.0;

		float r2 = S / a; // The radius of the circle the robot travel's around with the back of the robot
		h2 = ((r2 + S_DISTANCE_IN) * sinI) * 2.0;
	}
	else
	{
		h = R;
		i = 0;

		h2 = S;
	}
	float p = i + position.a; // The global ending angle of the robot
	float cosP = cos(p);
	float sinP = sin(p);

	// Update the global position
	position.y += h * cosP;
	position.x += h * sinP;

	position.y += h2 * -sinP; // -sin(x) = sin(-x)
	position.x += h2 * cosP; // cos(x) = cos(-x)

	position.a = position.aStart + ( ((float)(LTurn - RTurn)) / ((float)(L_DISTANCE_IN + R_DISTANCE_IN)) );
}

void resetPosition(sPos& position)
{
	position.leftLst = position.rightLst = position.backLst = 0;
	position.y = position.x = position.a = 0;
}

void resetVelocity(sVel& velocity, sPos& position)
{
	velocity.a = velocity.y = velocity.x = 0;

	velocity.lstPosA = position.a;
	velocity.lstPosY = position.y;
	velocity.lstPosX = position.x;

	velocity.lstChecked = nPgmTime;
}

void trackVelocity(sPos position, sVel& velocity)
{
	unsigned long curTime = nPgmTime;
	long passed = curTime - velocity.lstChecked;
	if (passed > 40)
	{
		float posA = position.a;
		float posY = position.y;
		float posX = position.x;
		velocity.a = ((posA - velocity.lstPosA) * 1000.0) / passed;
		velocity.y = ((posY - velocity.lstPosY) * 1000.0) / passed;
		velocity.x = ((posX - velocity.lstPosX) * 1000.0) / passed;

		velocity.localY = (velocity.x * (float) sin(position.a) )+ (velocity.y * (float) cos(position.a);

		velocity.lstPosA = posA;
		velocity.lstPosY = posY;
		velocity.lstPosX = posX;
		velocity.lstChecked = curTime;
	}
}

task trackPositionTask()
{
	while (true)
	{
		updateSensorInput(trackL);
		updateSensorInput(trackR);
		updateSensorInput(trackB);
		trackPosition(gSensor[trackL].value, gSensor[trackR].value, gSensor[trackB].value, gPosition);
		trackVelocity(gPosition, gVelocity);
		sleep(1);
	}
}

void resetPositionFull(sPos& position, float x, float y, float a)
{
	tStop(trackPositionTask);
	writeDebugStreamLine("Resetting position %f %f %f | %f %f %f", position.y, position.x, radToDeg(fmod(gPosition.a, PI * 2)), y, x, radToDeg(fmod(a, PI * 2)));
	resetPosition(position);

	resetQuadratureEncoder(trackL);
	resetQuadratureEncoder(trackR);
	resetQuadratureEncoder(trackB);

	position.leftStart = gSensor[trackL].value;
	position.rightStart = gSensor[trackR].value;
	position.backStart = gSensor[trackB].value;

	position.y = y;
	position.x = x;
	position.aStart = a;

	tStart(trackPositionTask);
}

/* Vector Translation Functions */
void constructTrianglePos(sTrianglePos& pos, float x, float y, bool findAngle)
{
	pos.vector.x = x;
	pos.vector.y = y;
	pos.hypotenuse = sqrt((x * x) + (y * y));
	if (findAngle)
		pos.a = atan2(x, y);
}

void vectorToPolar(sVector& vector, sPolar& polar)
{
	if (vector.x || vector.y)
	{
		polar.magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
		polar.angle = atan2(vector.y, vector.x);
	}
	else
		polar.magnitude = polar.angle = 0;
}

void polarToVector(sPolar& polar, sVector& vector)
{
	if (polar.magnitude)
	{
		vector.x = polar.magnitude * cos(polar.angle);
		vector.y = polar.magnitude * sin(polar.angle);
	}
	else
		vector.x = vector.y = 0;
}

/* Line Manipulation Functions */
void makeLine(sLine& line)
{
	line.m = line.b = line.xVer = line.yHor = 0; // Reset all the line constants

	float x = line.deltaX = line.p2.x - line.p1.x;
	float y = line.deltaY = line.p2.y - line.p1.y;


	line.length = sqrt((x*x) + (y*y));

	if (x == 0 && y != 0)
	{
		line.lineType = vertical;
		line.xVer = line.p2.y;
	}
	else if (y == 0 && x != 0)
	{
		line.lineType = horizontal;
		line.yHor = line.p2.x;
	}
	else if (y == 0 && x == 0)
	{
		line.lineType = point;
	}
	else if (y != 0 && x != 0)
	{
		line.lineType = diagonal;
		line.m = y / x;
		line.b = line.p2.y - (line.m * line.p2.x);
	}
}

void makeLineInverse(const sLine& original, sLine& inverse, sVector pOI)
{
	inverse.m = inverse.b = inverse.xVer = inverse.yHor = 0; // Reset all the line constants
	switch (original.lineType)
	{
		case diagonal:
			inverse.lineType = diagonal;
			inverse.m = -1 * (1 / original.m);
			inverse.b = pOI.y - (inverse.m * pOI.x);
			break;
		case horizontal:
			inverse.lineType = vertical;
			inverse.xVer = pOI.y;
			break;
		case vertical;
			inverse.lineType = horizontal;
			inverse.yHor = pOI.x;
			break;
	}
}

float findX(sLine line, float y)
{
	switch (line.lineType)
	{
		case point:
			return line.p2.x;
			break;
		case diagonal:
			return (y - line.b) / line.m;
			break;
		case horizontal:
			writeDebugStreamLine("%d Error, cannot find x of horizontal line", npgmtime);
			return -1;
			break;
		case vertical;
			return line.xVer;
			break;
	}
	writeDebugStreamLine("%d No X for line", npgmtime);
	return -1;
}

float findY(sLine line, float x)
{
	switch (line.lineType)
	{
		case point:
			return line.p2.y;
			break;
		case diagonal:
			return ((line.m*x)+line.b);
			break;
		case horizontal:
			return line.yHor;
			break;
		case vertical;
			writeDebugStreamLine("%d Error, cannot find y of horizontal line", npgmtime);
			return -1;
			break;
	}
	writeDebugStreamLine("%d No X for line", npgmtime);
	return -1;
}

float getAngleOfLine(sLine line)
{
	return atan2(line.p2.x - line.p1.x, line.p2.y - line.p1.y);
}

float getLengthOfLine(sLine line)
{
	float x = line.p2.x - line.p1.x;
	float y = line.p2.y - line.p1.y;
	return sqrt(x * x + y * y);
}

/* Misc Auto Functions/Tasks */
void offsetPos(float& x, float& y, float offset)
{
	x = offset * sin(gPosition.a);
	y = offset * cos(gPosition.a);

	x += gPosition.x;
	y += gPosition.y;
}

byte facingCoord(float targX, float targY, float offset)
{
	float curX = gPosition.x;
	float curY = gPosition.y;
	float curA = simplifyAngle(gPosition.a);
	float gAngleToFront = aTan2((targX-curX),(targY-curY));
	float gAngleToBack = aTan2((curX-targX),(curY-targY));

	writeDebugStreamLine("(%f,%f)RobotPos%f, PosTo%f", gPosition.x, gPosition.y, gPosition.a, gAngleToFront);
	//Check if the front, or back of robot is facing the target
	if ( abs( (curA-gAngleToFront) ) < offset ) //< (gAngleTo - pi/4) || gPosition.a > (gAngleTo + pi/4) )
	{
		writeDebugStreamLine("Front facing target (%f, %f)", targX, targY);
		return facingFront;
	}
	else if ( abs( (curA-gAngleToBack) ) < offset )
	{
		writeDebugStreamLine("Back facing target (%f, %f)", targX, targY);
		return facingBack;
	}
	else
	{
		writeDebugStreamLine("MOVEMENT ERROR - WRONG DIR. RobotPos%f, PosTo%f", gPosition.a, gAngleToFront);
		return facingNone;
	}
}

task autoMotorSensorUpdateTask()
{
	sCycleData cycle;
	initCycle(cycle, 10, "auto motor/sensor");
	writeDebugStreamLine("%d Start autoMotorSensorUpdateTask", npgmtime);

	while (true)
	{
		nBatteryLevel = nImmediateBatteryLevel;
		updateMotors();
		updateSensorInputs();
		updateSensorOutputs();

		if (DATALOG_BATTERY != -1)
		{
			tHog();
			datalogDataGroupStart();
			datalogAddValue(DATALOG_BATTERY + 0, nImmediateBatteryLevel);
			datalogAddValue(DATALOG_BATTERY + 2, BackupBatteryLevel);
			datalogDataGroupEnd();
			tRelease();
		}

		endCycle(cycle);
	}
}

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

	LIM_TO_VAL_SET(left, 30);
	LIM_TO_VAL_SET(right, 30);

	writeDebugStreamLine("Applying harsh stop: %d %d", left, right);
	setDrive(left, right);
	updateMotors();

	unsigned long startTime = npgmtime;
	WHILE(drive, (npgmtime-startTime) < 150)
		sleep(10);

	setDrive(0, 0);
	updateMotors();
}
