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

	//writeDebugStream("	>Run Auto: %d, Alliance: %d, pLFlag: %d", gAuto, gAlliance, gAutoPreloadFlag);

	//if (oldAlliance != gAlliance || selected - gCurAuto != 1 || autoVal % (2048 / AUTO_OPTIONS_COUNT) > 100)
	//	gCurAuto = selected;
}

task autoShooterReload()
{
	sleep(50);
	writeDebugStreamLine("%d autoShooterReload Task Start", nPgmTime);
	while (SensorValue[anglerPoti] >= ANGLER_AXEL_POS) sleep(10);
	setShooterState(shooterReload);
	return;
}

void runAuto()
{
	bool dummyBool = false;

	if (gAlliance == allianceRed) //red autos
	{
		switch (gAuto)
		{
			case autoFront:

				resetTracking(gPosition, RED_FRONT_X, RED_FRONT_Y, 0);

				////shoot
				//anglerUnderAxle();
				//angleShoot(gAutoPreloadFlag == flagTop? gAnglerFrontPFTopFlag : gAnglerFrontPFMidFlag,
				//	70, false, BTN_SHOOT, dummyBool);

				//1 grab ball
				anglerMoveToPos(ANGLER_GROUND_PICKUP_POS, 70);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(59, 40, 90, 40, 4, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);
				moveToTarget(59, 51, 40, 40, 4, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//

				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
				resetTracking(gPosition, RED_BACK_X, RED_BACK_Y, -90);

				//1 shoot
				anglerUnderAxle();
				angleShoot(gAutoPreloadFlag == flagTop? gAnglerBackPFTopFlag : gAnglerBackPFMidFlag,
					40, false, BTN_SHOOT, dummyBool);

				//2 back up and raise angler and start intake
				anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				moveToTarget(130, RED_BACK_Y, -70, -40, 5, 7, -30, 0, (stopSoft | stopHarsh), mttProportional);
				setIntakeState(intakeUp);

				//3 pickup balls
				turnToTargetP(129, 50, 40, 22, true, 70);
				moveToTarget(129, 53, 90, 40, 1, 12, 40, 0, (stopSoft | stopHarsh), mttProportional); //pick up balls

				//4 turn to face platform
				moveToTarget(131, 46, -70, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional); //backup
				turnToTargetP(105, 46, 35, 20, true, 70);

				//5 run into platform
				moveToTarget(100, 48, 70, 40, 5, 12, 40, 0, stopSoft, mttProportional);
				setDrive(60, 60);
				while (gVelocity.x < -0.05)
				{
					LOG(auto)("%d vel:%d", nPgmTime, gVelocity.a);
					sleep(10);
				}

				//turnToTargetP(126, 48, 40, 22, true, 70);
				//moveToTarget(126, 49, 90, 40, 5, 12, 40, 0, (stopSoft | stopHarsh), mttProportional); //pick up balls
				//moveToTarget(116, 43, -70, -40, 5, 7, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//turnToAngleP(-65, 35, 25, true, 70);
				//moveToTarget(100, 48, 70, 40, 5, 12, 40, 0, stopSoft, mttProportional);


				//moveToTarget(84, 48, 127, 35, 5, 4, 20, 0, (stopSoft | stopHarsh), mttProportional);
				//while (gAnglerGoodCount < 5) sleep(10);
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
