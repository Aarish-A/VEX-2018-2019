void selectAuto()
{
	if (allianceForce > -1 && autoForce > -1)
	{
		gAlliance = allianceForce;
		gAuto = autoForce;
	}
	else
	{
		int autoVal = SensorValue[autoPoti] - 2048;
		tAlliance oldAlliance = gAlliance;
		if (autoVal < 0) gAlliance = allianceBlue;
		else gAlliance = allianceRed;

		autoVal = abs(autoVal);
		if (autoVal % (2048 / kAutoOptionsCount) > 100)
			gAuto = autoVal * kAutoOptionsCount / 2048;
	}

	writeDebugStream("	>Run Auto: %d, Alliance: %d, pLFlag: %d", gAuto, gAlliance, gAutoPreloadFlag);

	//if (oldAlliance != gAlliance || selected - gCurAuto != 1 || autoVal % (2048 / AUTO_OPTIONS_COUNT) > 100)
	//	gCurAuto = selected;
}

void runAuto()
{
	bool dummyBool = false;

	if (gAlliance == allianceRed) //red autos
	{
		switch (gAuto)
		{
			case autoFront:
				resetTracking(gPosition, RED_FRONT_X, RED_FRONT_Y, -90);

				anglerUnderAxle();
				angleShoot(gAutoPreloadFlag == flagTop? gAnglerFrontPFTopFlag : gAnglerFrontPFMidFlag,
					70, false, BTN_SHOOT, dummyBool);

				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
				resetTracking(gPosition, RED_BACK_X, RED_BACK_Y, -90);

				anglerUnderAxle();
				angleShoot(gAutoPreloadFlag == flagTop? gAnglerBackPFTopFlag : gAnglerBackPFMidFlag,
					40, false, BTN_SHOOT, dummyBool);

				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;
		}
	}
	else //blue autos
	{
		switch (gAuto)
		{
			case autoFront:
				resetTracking(gPosition, 62, 3.6+S_DISTANCE_IN, 0);

				anglerUnderAxle();
				angleShoot(gAutoPreloadFlag == flagTop? gAnglerFrontPFTopFlag : gAnglerFrontPFMidFlag,
					70, false, BTN_SHOOT, dummyBool);

				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
				resetTracking(gPosition, 62, 3.6+S_DISTANCE_IN, 0);

				anglerUnderAxle();
				angleShoot(gAutoPreloadFlag == flagTop? gAnglerBackPFTopFlag : gAnglerBackPFMidFlag,
					40, false, BTN_SHOOT, dummyBool);

				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;
		}
	}
}

void anglerUnderAxle()
{
	if (SensorValue[anglerPoti] >= ANGLER_AXEL_POS)
	{
		anglerMoveToPos(ANGLER_AXEL_POS-100, 100);
		while (SensorValue[anglerPoti] >= ANGLER_AXEL_POS) sleep(10);
	}
}
