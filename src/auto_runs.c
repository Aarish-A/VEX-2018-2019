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
	unsigned long autoStartTime = nPgmTime;

	if (gAlliance == allianceRed) //red autos
	{
		switch (gAuto)
		{
			case autoFront:

				resetTracking(gPosition, gVelocity, RED_FRONT_X, RED_FRONT_Y, 0);

				//1 grab ball
				anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(59, 40, 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				moveToTarget(59, 55, 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(59, 40, -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
				moveToTarget(59, 16, -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				turnToTargetAccurate(11, 16, ch, 80, 80, 0);
				angleShoot(gAnglerFrontPFMidFlag, 70, false, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, dummyBool);
				angleShoot(gAnglerFrontPFTopFlag, 70, true, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, dummyBool);

				//4 Toggle low flag
				setIntakeState(intakeIdle);
				setShooterState(shooterIdle);
				anglerMoveToPos(2800, 70);
				moveToTarget(11, 15, 127, 40, 3, 12, 30, 0, (stopSoft | stopHarsh), mttProportional);
				LOG_AUTO(("%d t:%d", nPgmTime-autoStartTime));

				//5 Back up and get on platform
				moveToTarget(82, 15, -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
				turnToTargetAccurate(84, 50, ch, 80, 80, 0);
				setDrive(90, 90);
				sleep(400);
				while (gVelocity.y > 0.1)
				{
					//LOG(auto)("%d vel:%f", nPgmTime, gVelocity.y);
					sleep(10);
				}
				setDrive(-15,-15);
				sleep(150);
				setDrive(127,127);
				sleep(1200);
				setDrive(0, 0);
				sleep(0);
				//moveToTargetDisY(84, 50, 100, 40, 4, 12, 30, 0, (stopSoft|stopHarsh), mttSimple);
				//moveToTarget(84, 40, 50, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);


				//anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
				resetTracking(gPosition, gVelocity, RED_BACK_X, RED_BACK_Y, 0);

				//1 grab ball
				anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(108, 40, 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				moveToTarget(RED_BACK_X, 55, 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(RED_BACK_X, 40, -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
				moveToTarget(RED_BACK_X, 16, -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				turnToTargetAccurate(11, 16, ch, 80, 80, 0);
				angleShoot(gAnglerBackPFMidFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, dummyBool);
				angleShoot(gAnglerBackPFTopFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, dummyBool);

				//4 Get on platform
				moveToTarget(84, 15, 90, 40, 4, 12, 30, 0, (stopSoft | stopHarsh), mttProportional);
				turnToTargetAccurate(84, 50, ch, 80, 80, 0);
				setDrive(90, 90);
				sleep(400);
				while (gVelocity.y > 0.1)
				{
					//LOG(auto)("%d vel:%f", nPgmTime, gVelocity.y);
					sleep(10);
				}
				setDrive(-15,-15);
				sleep(150);
				setDrive(127,127);
				sleep(1200);
				setDrive(0, 0);
				sleep(0);

				break;
		}
	}
	else //blue autos
	{
		switch (gAuto)
		{
			case autoFront:
				resetTracking(gPosition, gVelocity, BLUE_FRONT_X, BLUE_FRONT_Y, 180);

				//1 grab ball
				anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(59, (144-40), 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				moveToTarget(59, (144-57), 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(59, (144-40), -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
				moveToTarget(59, (144-15), -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				turnToTargetAccurate(11, (144-16), ch, 80, 80, 0);
				angleShoot(gAnglerFrontPFMidFlag, 70, false, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, dummyBool);
				angleShoot(gAnglerFrontPFTopFlag, 70, true, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, dummyBool);

				//4 Toggle low flag
				setIntakeState(intakeIdle);
				setShooterState(shooterIdle);
				anglerMoveToPos(2800, 70);
				moveToTarget(11, (144-12), 127, 40, 2, 12, 30, 0, (stopSoft | stopHarsh), mttProportional);
				LOG_AUTO(("%d t:%d", nPgmTime-autoStartTime));

				//5 Back up and get on platform
				moveToTarget(82, (144-12), -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
				turnToTargetAccurate(82, (144-50), ch, 80, 80, 0);
				setDrive(90, 90);
				sleep(400);
				while (gVelocity.y < -0.1)
				{
					//LOG(auto)("%d vel:%f", nPgmTime, gVelocity.y);
					sleep(10);
				}
				setDrive(-15,-15);
				sleep(150);
				setDrive(127,127);
				sleep(1200);
				setDrive(0, 0);
				sleep(0);

				//resetTracking(gPosition, gVelocity, 62, 3.6+S_DISTANCE_IN, 0);
				//anglerUnderAxle();
				//angleShoot(gAutoPreloadFlag == flagTop? gAnglerFrontPFTopFlag : gAnglerFrontPFMidFlag,
				//	70, false, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, dummyBool);
				//anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
			resetTracking(gPosition, gVelocity, BLUE_BACK_X, BLUE_BACK_Y, 180);

				//1 grab ball
				anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(108, (144-40), 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				moveToTarget(RED_BACK_X, (144-57), 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(RED_BACK_X, (144-40), -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
				moveToTarget(RED_BACK_X, (144-16), -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				turnToTargetAccurate(11, (144-16), ch, 80, 80, 0);
				angleShoot(gAnglerBackPFMidFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, dummyBool);
				angleShoot(gAnglerBackPFTopFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, dummyBool);

				//4 Get on platform
				moveToTarget(83, (144-12), 90, 40, 4, 12, 30, 0, (stopSoft | stopHarsh), mttProportional);
				turnToTargetAccurate(82, (144-50), ch, 80, 80, 0);
				setDrive(90, 90);
				sleep(400);
				while (gVelocity.y < -0.1)
				{
					//LOG(auto)("%d vel:%f", nPgmTime, gVelocity.y);
					sleep(10);
				}
				setDrive(-15,-15);
				sleep(150);
				setDrive(127,127);
				sleep(1200);
				setDrive(0, 0);
				sleep(0);

				//resetTracking(gPosition, gVelocity, 62, 3.6+S_DISTANCE_IN, 0);
				//anglerUnderAxle();
				//angleShoot(gAutoPreloadFlag == flagTop? gAnglerBackPFTopFlag : gAnglerBackPFMidFlag,
				//	40, false, MAX_ANGLE_TIME, BTN_SHOOT, dummyBool);
				//anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
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
