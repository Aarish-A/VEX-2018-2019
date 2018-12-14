void selectAuto()
{
	if (gAutoLock) return;

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

	switch (gAlliance) //Set Alliance String
	{
		case allianceBlue: gSAlliance = S_ALLIANCE_BLUE; break;
		case allianceRed: gSAlliance = S_ALLIANCE_RED; break;
		default: gSAlliance = "U_D"; break;
	}
	switch (gAuto) //Set Auto String
	{
		case autoFront: gSAuto = S_AUTO_FRONT; break;
		case autoBack: gSAuto = S_AUTO_BACK; break;
		case autoSkills: gSAuto = S_AUTO_SKILLS; break;
		default: gSAuto = "U_D"; break;
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
	unsigned long autoStartTime = nPgmTime;

	if (gAuto == autoTest)
	{
		resetTracking(gPosition, gVelocity, RED_FRONT_X, RED_FRONT_Y, 0);
		resetPositionFull(gPosition, RED_FRONT_X, RED_FRONT_Y, 0);

		//turnToTargetAccurate();
	}
	else if (gAuto == autoSkills)
	{
		writeDebugStreamLine(" >> %d Auto Skills" ,nPgmTime);

		//1-4 Grab balls, shoot, toggle flag
		redFrontBeginning(true); //Finishes on toggle flag

		//5 Back up and reset angle
		moveToTarget(60, 15, -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
		turnToAngleNewAlg(10,ch,0.35,20,10,true);
		setDrive(-50, -50);
		sleep(350);
		while (gVelocity.y > 0.1) sleep(10);

		setDrive(-60,-15);
		sleep(350);
		do{
			sleep(10);
		}while(gVelocity.a < -0.05);
		setDrive(0,0);
		setDrive(-15,-60);
		sleep(350);
		do{
			sleep(10);
		}while(gVelocity.a > 0.05);
		setDrive(-15,-15);
		sleep(150);
		resetPositionFull(gPosition, gPosition.x, RED_FRONT_Y, 0);

		//6 Pick up balls
		moveToTarget(60, 24, 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);
		turnToTargetAccurate(35,37,ch,80,80,0);
		startTask(autoShooterReload);
		anglerMoveToPos(ANGLER_CAP_PICKUP_POS,20);
		setIntakeState(intakeUp);
		moveToTarget(35, 37, 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);
		sleep(1000);
		anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
		sleep(1000);
		turnToTargetAccurate(12,65,ch,80,80,0);
		//sleep(1000);
		//turnToTargetAccurate(12,65,ch,80,80,0);
		//sleep(2000); //Delete?
		//anglerMoveToPos(gAnglerFrontPFMidFlag, 70);

		//7 Shoot mid flags (top + mid)
		sleep(1500);
		setIntakeState(intakeIdle);
		angleShoot(gAnglerFrontPFMidFlag, 70, true, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, gDummyNextShot);
		//turnToTargetAccurate(12,65,ch,80,80,0);
		angleShoot(gAnglerFrontPFTopFlag+250, 70, true, MAX_ANGLE_TIME, BTN_SHOOT, gDummyNextShot);
		sleep(250);

		//8 Back up and flip cap
		moveToTarget(48, 24, -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
		anglerMoveToPos(ANGLER_CAP_FLIP_POS,70);
		setIntakeState(intakeDown);
		moveToTarget(24, 48, 40, 40, 3, 8, 40, 0, (stopSoft), mttProportional);
		anglerMoveToPos(ANGLER_CAP_FLIP_POS+300,70);

		//9 Get on platform
		moveToTarget(60, 24, -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
		setIntakeState(intakeIdle);
		turnToTargetAccurate(82, 15, ch,80,80,180);
		redFrontPark(true);
		//setDrive(35, 35);
		//		sleep(300);
		//		while (gVelocity.y > 0.1)
		//		{
		//			//LOG(auto)("%d vel:%f", nPgmTime, gVelocity.y);
		//			sleep(10);
		//		}
		//	anglerMoveToPos(950, 50);
		//	sleep(200);
		//	setDrive(60, 60);
		//	sleep(600);

	}
	else if (gAlliance == allianceRed) //red autos
	{
		switch (gAuto)
		{
			case autoFront:
				redFrontBeginning(false); //finishes on flag toggle

				if (gAutoPark)
				{
					//5 Back up and get on platform
					redFrontPark(false);
				}

				//anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
				resetTracking(gPosition, gVelocity, RED_BACK_X, RED_BACK_Y, 0);

				//1 grab ball
				anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(RED_BACK_X, 54.5, 127, 40, 5, 15, 30, 0, (stopSoft | stopHarsh), mttProportional);
				//moveToTarget(108, 40, 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				//moveToTarget(RED_BACK_X, 55, 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(RED_BACK_X, 40, -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerBackPFMidFlag, 70);
				moveToTarget(RED_BACK_X, 16, -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				//turnToAngleNewAlg(-90,ch,0.35,20,10,true);
				turnToTargetAccurate(FLAG_X, RED_FLAG_Y, ch, 80, 80, 0);
				angleShoot(gAnglerBackPFMidFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, gDummyNextShot);
				angleShoot(gAnglerBackPFTopFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, gDummyNextShot);

				if (gAutoPark)
				{
					//4 Get on platform
					moveToTarget(81, RED_FLAG_Y, 90, 40, 4, 12, 30, 0, (stopSoft | stopHarsh), mttProportional);
					turnToAngleNewAlg(0,ch,0.35,20,10,true);
					//turnToTargetAccurate(84, 50, ch, 80, 80, 0);
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
				}

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
				moveToTarget(BLUE_FRONT_X, (144-55), 127, 40, 5, 15, 30, 0, (stopSoft | stopHarsh), mttProportional);
				//moveToTarget(59, (144-40), 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				//moveToTarget(59, (144-57), 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(BLUE_FRONT_X, (144-40), -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
				moveToTarget(BLUE_FRONT_X, BLUE_FLAG_Y, -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				//turnToTargetAccurate(11, (144-16), ch, 80, 80, 0);
				turnToTargetAccurate(FLAG_X, BLUE_FLAG_Y, ch, 80, 80, 0);
				angleShoot(gAnglerFrontPFMidFlag, 70, false, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, gDummyNextShot);
				angleShoot(gAnglerFrontPFTopFlag, 70, true, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, gDummyNextShot);

				//4 Toggle low flag
				setIntakeState(intakeIdle);
				//anglerMoveToPos(ANGLER_GROUND_PICKUP_POS, 70);
				//setIntakeState(intakeUp);
				setShooterState(shooterIdle);
				anglerMoveToPos(ANGLER_TOP_POS, 70);
				moveToTarget((FLAG_X-4), (144-11.5), 127, 40, 1, 12, 30, 0, (stopSoft), mttProportional);
				//anglerMoveToPos(ANGLER_HORIZONTAL_POS, 70);
				//sleep(250);
				//anglerMoveToPos(ANGLER_TOP_POS, 70);

				if (gAutoPark)
				{
					//5 Back up and get on platform
					moveToTarget(80, BLUE_FLAG_Y, -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
					//turnToTargetAccurate(82, (144-50), ch, 80, 80, 0);
					turnToTargetNewAlg(81,(144-50),ch,0.35,20,10,true);
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
				}

				//resetTracking(gPosition, gVelocity, 62, 3.6+S_DISTANCE_IN, 0);
				//anglerUnderAxle();
				//angleShoot(gAutoPreloadFlag == flagTop? gAnglerFrontPFTopFlag : gAnglerFrontPFMidFlag,
				//	70, false, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, gDummyNextShot);
				//anglerMoveToPos(ANGLER_CAP_PICKUP_POS, 100);
				break;

			case autoBack:
			resetTracking(gPosition, gVelocity, BLUE_BACK_X, BLUE_BACK_Y, 180);

				//1 grab ball
				anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
				setIntakeState(intakeUp);
				startTask(autoShooterReload);
				moveToTarget(BLUE_BACK_X, (144-55), 127, 40, 5, 15, 30, 0, (stopSoft | stopHarsh), mttProportional);
				//moveToTarget(108, (144-40), 127, 40, 4, 8, 40, 0, (stopSoft), mttProportional);
				//moveToTarget(RED_BACK_X, (144-57), 40, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

				//2 Back up
				moveToTarget(BLUE_BACK_X, (144-40), -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
				anglerMoveToPos(gAnglerBackPFMidFlag, 70);
				moveToTarget(BLUE_BACK_X, BLUE_FLAG_Y, -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

				//3 Turn and shoot
				turnToTargetAccurate(FLAG_X, BLUE_FLAG_Y, ch, 80, 80, 0);
				angleShoot(gAnglerBackPFMidFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, gDummyNextShot);
				angleShoot(gAnglerBackPFTopFlag, 40, true, MAX_ANGLE_TIME, BTN_SHOOT, gDummyNextShot);


				if (gAutoPark)
				{
					//4 Get on platform
					moveToTarget(81, BLUE_FLAG_Y, 90, 40, 4, 12, 30, 0, (stopSoft | stopHarsh), mttProportional);
					turnToTargetNewAlg(81,(144-50),ch,0.35,20,10,true);
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
				}

				//resetTracking(gPosition, gVelocity, 62, 3.6+S_DISTANCE_IN, 0);
				//anglerUnderAxle();
				//angleShoot(gAutoPreloadFlag == flagTop? gAnglerBackPFTopFlag : gAnglerBackPFMidFlag,
				//	40, false, MAX_ANGLE_TIME, BTN_SHOOT, gDummyNextShot);
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

void redFrontBeginning(bool skills)
{
	resetTracking(gPosition, gVelocity, RED_FRONT_X, RED_FRONT_Y, 0);

	//1 Grab balls
	anglerMoveToPos(ANGLER_BELOW_CAP_PICKUP_POS, 20);
	setIntakeState(intakeUp);
	startTask(autoShooterReload);
	moveToTarget(59, skills? 56.5 : 54.5, 127, 40, 5, 15, 30, 0, (stopSoft | stopHarsh), mttProportional);
	//moveToTarget(59, 44, 127, 40, 4, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);
	//moveToTarget(59, 54, 30, 40, 3, 8, 40, 0, (stopSoft | stopHarsh), mttProportional);

	//2 Back up
	moveToTarget(59, 40, -70, -40, 4, 12, -30, 0, (stopSoft), mttProportional);
	anglerMoveToPos(gAnglerFrontPFMidFlag, 70);
	moveToTarget(59, (RED_FLAG_Y+0.5), -127, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);

	//3 Turn and shoot
	turnToTargetAccurate(FLAG_X, RED_FLAG_Y, ch, 80, 80, 0);
	angleShoot(gAnglerFrontPFMidFlag, 70, false, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, gDummyNextShot);
	angleShoot(gAnglerFrontPFTopFlag, 70, true, MAX_ANGLE_TIME_FRONT, BTN_SHOOT, gDummyNextShot);

	//4 Toggle low flag
	setIntakeState(intakeIdle);
	setShooterState(shooterIdle);
	anglerMoveToPos(ANGLER_TOP_POS, 70);
	//anglerMoveToPos(ANGLER_GROUND_PICKUP_POS, 70);
	//setIntakeState(intakeUp);
	moveToTarget((FLAG_X-4), 13, 127, 40, 1, 12, 30, 0, (stopSoft), mttProportional);
	//anglerMoveToPos((ANGLER_HORIZONTAL_POS+200), 70);
	//sleep(250);
	//anglerMoveToPos(ANGLER_TOP_POS, 70);
}

void redFrontPark(bool skills)
{
	moveToTarget(80, RED_FLAG_Y, -90, -40, 4, 12, -30, 0, (stopSoft | stopHarsh), mttProportional);
	turnToTargetNewAlg(84, 50, ch,0.35,20,10,true);
	//turnToAngleNewAlg(5,ch,0.35,20,10,true);
	//turnToTargetAccurate(84, 50, ch, 80, 80, 0);
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
}

void testTurn(bool right)
{
	stopTask(driveStateSet);
	writeDebugStreamLine("%d Test Turn", nPgmTime);
	resetTracking(gPosition, gVelocity, RED_FRONT_X, RED_FRONT_Y, 0);
	sleep(10);
	setDrive(60, 60);
	sleep(200);
	if (right)
		turnToAngleNewAlg(359,cw,0.15,30,30,true);
	else
		turnToAngleNewAlg(1,ccw,0.15,30,30,true);

	setDrive(0,0);
	startTask(driveStateSet);
}
