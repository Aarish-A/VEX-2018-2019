/* Functions */
void initSafety(sSafety& safety, const string name, tVelType velType, tSensors sensor)
{
	safety.name = name;
	safety.velType = velType;
	safety.sensor = sensor;
}

void _setSafetyMessage(sSafety& safety, const string message)
{
	tHog();
	safety.message = safety.name;
	string temp = " ";
	strcat(temp, message);
	writeDebugStreamLine("%d name:%s temp:%s mssg:%s", nPgmTime, safety.name, temp, safety.message);
	strcat(safety.message, temp);
	tRelease();
}

void setSafety(sSafety& safety, const string message, int timeOut, tVelDir velDir, float velThresh, tVelType velType)
{
	safety.message = message;
	safety.velType = velType;

	safety.startTime = nPgmTime;
	safety.timeOut = safety.startTime + timeOut;
	safety.time = safety.startTime;
	safety.timeLst = 0;

	safety.velBadCount = 0;
	safety.velDir = velDir;
	if (velDir == velEither) velThresh = abs(velThresh);
	safety.velThresh = velThresh;

	safety.val = SensorValue[safety.sensor];
	safety.valLst = safety.val;
}

void setSafetyTO(sSafety& safety, const string message, int timeOut)
{
	safety.message = message;
	safety.velType = velNone;

	safety.startTime = nPgmTime;
	safety.timeOut = safety.startTime + timeOut;
	safety.time = safety.startTime;
	safety.timeLst = 0;
}

bool isTimeoutSafe(sSafety& safety)
{
	if(nPgmTime < safety.timeOut) return true;
	else
	{
		writeDebugStreamLine("%d %s |%s| timedout. TO:%d", nPgmTime, safety.name, safety.message, safety.timeOut);
		return false;
	}
}

bool isVelSafe(sSafety& safety)
{
	if (safety.velType != velNone)
	{
		if (safety.velType == velSensor)
		{
			safety.val = SensorValue[safety.sensor];
			safety.time = nPgmTime;
		}

		unsigned long deltaTime = (safety.time - safety.timeLst);
		float vel;
		if (deltaTime > 0 || safety.velType != velSensor)
		{
			if (safety.velType == velSensor) vel = (float)(safety.val - safety.valLst) / deltaTime;
			//ToDo:: Replace error message with commented out code (once tracking code is included)
			else writeDebugStreamLine("	%d Invalid safetyVelType: %d", nPgmTime, safety.velType);
			//else if (safety.velType == velLocalY) vel = gVelocity.localY;
			//else if (safety.velType == velAngle) vel = gVelocity.a;

			if (nPgmTime > (safety.startTime+(unsigned long)VEL_ACCEL_TIME))
			{
				switch(safety.velDir)
				{
					case velEither:
						if (abs(vel) < safety.velThresh) safety.velBadCount++;
						else safety.velBadCount = 0;
						break;
					case velUp:
						if (vel < safety.velThresh) safety.velBadCount++;
						else safety.velBadCount = 0;
						break;
					case velDown:
						if (vel > safety.velThresh) safety.velBadCount++;
						else safety.velBadCount = 0;
						break;
				}
			}
		}
		else if (deltaTime < 0)
		{
			writeDebugStreamLine("%d Safety Lib Time Error. DeltaTime: %d. Time:%d, TimeLst:%d", nPgmTime, deltaTime, safety.time, safety.timeLst);
		}

		if (safety.velType == velSensor)
		{
			safety.valLst = safety.val;
			safety.timeLst = safety.time;
		}

		if (safety.velBadCount <= VEL_BAD_COUNT_LIM) return true;
		else
		{
			string dir;
			switch(safety.velDir)
			{
				case velEither: dir = "either"; break;
				case velUp: dir = "up"; break;
				case velDown: dir = "down"; break;
				default: dir = "unknown"; break;
			}
			writeDebugStreamLine("%d %s |%s| - Vel Safety. Thresh:%f, Dir: %s, BadVelCount:%d, Vel:%f.", nPgmTime, safety.name, safety.message, safety.velThresh, dir, safety.velBadCount, vel);
			return false;
		}
	}
	else return true;
}
