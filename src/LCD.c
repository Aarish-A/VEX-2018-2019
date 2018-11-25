task handleLCD()
{
  bool curLCDLeft, lstLCDLeft;
  bool curLCDMiddle, lstLCDMiddle;
  bool curLCDRight, lstLCDRight;
  int pointer = 0;

  bool selected = false;

  while(true)
  {
    clearLCDLine(0);
    clearLCDLine(1);

    if (nLCDButtons == 1) curLCDLeft = true;
    else curLCDLeft = false;

    if (nLCDButtons == 2) curLCDMiddle = true;
    else curLCDMiddle = false;

    if (nLCDButtons == 4) curLCDRight = true;
    else curLCDRight = false;

    switch(gLCDScreen)
    {
      case batteryLevel:
      {
        displayLCDString(0, 0, "Battery: ");
        gBatteryLevel = nImmediateBatteryLevel;
        displayNextLCDNumber(gBatteryLevel, 4);

        //displayLCDString(1, 0, "BU Battery: ");
        //gBackupBatteryLevel = BackupBatteryLevel;
        //displayNextLCDNumber(gBackupBatteryLevel, 4);
        //displayLCDChar(1, 0, '<');

        selected = false;
        if (LCD_L) gLCDScreen = anglerTargetTuning;
        else if (LCD_R) gLCDScreen++;
        break;
      }
      case turnBreakPow:
      {
      	displayLCDCenteredString(0, "Turn Break Power:");
       displayLCDNumber(1, 0, gDriveBreakPow);

      	if (!selected && LCD_M)
      	{
      		selected = true;
      		writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      		goto end;
      	}

      	if (selected)
      	{
      		if (LCD_R) gDriveBreakPow += 1;
      		if (LCD_L) gDriveBreakPow -= 1;

      		if (LCD_M)
      		{
      			selected = false;
      			writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      			//goto end;
      		}

      		//writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      	}
      	else if (!selected && LCD_R) gLCDScreen++;
      	else if (!selected && LCD_L) gLCDScreen--;
      	break;
      }
      case turnBreakWait:
      {
      	displayLCDCenteredString(0, "Turn Break Wait");
        displayLCDNumber(1, 0, gDriveBreakWait);

      	if (!selected && LCD_M)
      	{
      		selected = true;
      		writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      		goto end;
      	}

      	if (selected)
      	{
      		if (LCD_R) gDriveBreakWait += 50;
      		if (LCD_L) gDriveBreakWait -= 50;

      		if (LCD_M)
      		{
      			selected = false;
      			writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      		}

      		//writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      	}
      	else if (!selected && LCD_R) gLCDScreen++;
      	else if (!selected && LCD_L) gLCDScreen--;
      	break;
      }
      case turnCurveColour:
      {
      	displayLCDCenteredString(0, "Turn Colour:");
      	if (gTurnAlg)	displayLCDCenteredString(1, "blue");
      	else displayLCDCenteredString(1, "red");

      	if (LCD_M)
      	{
      		gTurnAlg = !gTurnAlg;
      	}
      	else if (LCD_R)
      	{
      		updateTurnLookup();
      		gLCDScreen++;
      	}
      	else if (LCD_L)
      	{
      		updateTurnLookup();
      		gLCDScreen--;
      	}
      	break;
      }
      case turnCurveAdjust:
      {
     		displayLCDCenteredString(0, "Turn Curve:");
       displayLCDNumber(1, 0, gTurnCurvature);

      	if (!selected && LCD_M)
      	{
      		selected = true;
      		//writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      		goto end;
      	}

      	if (selected)
      	{
      		if (LCD_R) gTurnCurvature += 1;
      		if (LCD_L) gTurnCurvature -= 1;

      		if (LCD_M)
      		{
      			selected = false;
      			//writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      			//goto end;
      		}

      		//writeDebugStreamLine("%d Selected: %d", nPgmTime, selected);
      	}
      	else if (!selected && LCD_R)
      	{
      		updateTurnLookup();
      		gLCDScreen++;
      	}
      	else if (!selected && LCD_L)
      	{
      		updateTurnLookup();
      		gLCDScreen--;
      	}
      	break;
      }
      case trackingTune:
      {
      	string line1, line2;
      	float LTurn = (SensorValue[trackL] - gPosition.leftStart) * SPIN_TO_IN_LR; // The amount the left side of the robot moved since the beginning
				float RTurn = (SensorValue[trackR] - gPosition.rightStart) * SPIN_TO_IN_LR; // The amount the right side of the robot moved since the beginning
      	float t = (float)(LTurn - RTurn);
      	sprintf(line1, "(%3.2f, %3.2f)", gPosition.x, gPosition.y);
      	sprintf(line2, "a:%3.2f, t:%3.2f", gPosition.a * 180 / PI, t);
      	displayLCDCenteredString(0, line1);
      	displayLCDCenteredString(1, line2);

      	if (LCD_M) resetPositionFull(gPosition, 0, 0, 0);

      	if (LCD_R) gLCDScreen++;
      	else if (LCD_L) gLCDScreen--;
      	break;
      }
      case shotCount:
      {
      	displayLCDString(0, 0, "ShotCount: ");
     		displayLCDNumber(0, 11, gShooterShotCount);

     		if (LCD_R) gLCDScreen++;
      	else if (LCD_L) gLCDScreen--;
      	break;
      }
      case shootTuneMode:
      {
      	displayLCDCenteredString(0, "Shoot Tune Mode?");
      	if (gShootTuneMode)	displayLCDCenteredString(1, "true");
      	else displayLCDCenteredString(1, "false");

      	if (LCD_M)
      	{
      		gShootTuneMode = !gShootTuneMode;
      	}

      	else if (LCD_R) gLCDScreen++;
      	else if (LCD_L) gLCDScreen--;
      	break;
      }
      case anglerTargetTuning:
      {
        displayLCDCenteredString(0, "Angler Tuning");
        //displayLCDChar(1, 15, '>');
        if (LCD_M) gLCDScreen = selectPosition;
        //else if (curLCDRight && !lstLCDRight) gLCDScreen = batteryLevel;
        else if (LCD_L) gLCDScreen --;
        else if (LCD_R) gLCDScreen = 0;
        break;
      }
      case selectPosition:
      {
        // Display
        displayLCDCenteredString(0, "F_PF M_PF B_PF B");

        switch(pointer)
        {
        	case 0: displayLCDChar(1, 0, '<'); break;
        	case 1: displayLCDChar(1, 1, '^'); break;
        	case 2: displayLCDChar(1, 6, '^'); break;
        	case 3: displayLCDChar(1, 11, '^'); break;
        	case 4: displayLCDChar(1, 15, '^'); break;
        }

        // Controls
        if (curLCDLeft && !lstLCDLeft)
        {
          if (pointer > 0) pointer--;
          else gLCDScreen = anglerTargetTuning;
        }
        else if (curLCDRight && !lstLCDRight)
        {
          if (pointer < kPosNum) pointer++;
        }
        else if (curLCDMiddle && !lstLCDMiddle)
        {
          //if (pointer == 1) gCurLCDSelection.position = 0;
          //else if (pointer == 2) gCurLCDSelection.position = 1;
          //else if (pointer == 3) gCurLCDSelection.position = 2;
          //else if (pointer == 4) gCurLCDSelection.position = 3;

        	gCurLCDSelection.position = pointer;

          if (pointer == 0) gLCDScreen = anglerTargetTuning;
          else gLCDScreen = selectFlag;
        }
        break;
      }
      case selectFlag:
      {
        // Display
        displayLCDCenteredString(0, "MidF    TopF");
        if (pointer == 0) displayLCDChar(1, 0, '<');
        else if (pointer == 1) displayLCDChar(1, 4, '^');
        else if (pointer == 2) displayLCDChar(1, 11, '^');
        else pointer = 2;

        // Controls
        if (curLCDLeft && !lstLCDLeft)
        {
          if (pointer > 0) pointer--;
          else gLCDScreen = selectPosition;
        }
        else if (curLCDRight && !lstLCDRight)
        {
          if (pointer < 2) pointer++;
        }
        else if (curLCDMiddle && !lstLCDMiddle)
        {
          if (pointer == 1) gCurLCDSelection.flag = 0;
          else if (pointer == 2) gCurLCDSelection.flag = 1;

          if (pointer == 0) gLCDScreen = selectPosition;
          else gLCDScreen = changeTarget;
        }
        break;
      }
      case changeTarget:
      {
        // Set current flag
        switch(gCurLCDSelection.position)
        {
            case 1:
              if (gCurLCDSelection.flag == 0)
              {
                gCurLCDSelection.name = "F_PF Mid: ";
                gCurLCDSelection.value = &gAnglerFrontPFMidFlag;
              }
              else if (gCurLCDSelection.flag == 1)
              {
                gCurLCDSelection.name = "F_PF Top: ";
                gCurLCDSelection.value = &gAnglerFrontPFTopFlag;
              }
              break;
            case 2:
              if (gCurLCDSelection.flag == 0)
              {
                gCurLCDSelection.name = "M_PF Mid ";
                gCurLCDSelection.value = &gAnglerMidPFMidFlag;
              }
              else if (gCurLCDSelection.flag == 1)
              {
                gCurLCDSelection.name = "M_PF Top ";
                gCurLCDSelection.value = &gAnglerMidPFTopFlag;
              }
              break;
             case 3:
              if (gCurLCDSelection.flag == 0)
              {
                gCurLCDSelection.name = "B_PF Mid ";
                gCurLCDSelection.value = &gAnglerBackPFMidFlag;
              }
              else if (gCurLCDSelection.flag == 1)
              {
                gCurLCDSelection.name = "B_PF Top ";
                gCurLCDSelection.value = &gAnglerBackPFTopFlag;
              }
              break;
            case 4:
              if (gCurLCDSelection.flag == 0)
              {
                gCurLCDSelection.name = "Back Mid ";
                gCurLCDSelection.value = &gAnglerBackMidFlag;
              }
              else if (gCurLCDSelection.flag == 1)
              {
                gCurLCDSelection.name = "Back Top ";
                gCurLCDSelection.value = &gAnglerBackTopFlag;
              }
              break;
        }

        // Display
        displayLCDString(0, 0, gCurLCDSelection.name);
        displayNextLCDNumber(*gCurLCDSelection.value, 4);

        // Controls
        if ((curLCDLeft && !lstLCDLeft) || (curLCDRight && !lstLCDRight))
        {
          int adjust;
          if (curLCDLeft) adjust = -10;
          else if (curLCDRight) adjust = 10;

          *gCurLCDSelection.value += adjust;
          //writeDebugStreamLine("%d | %d", nPgmTime, gAnglerBackTopFlag);
        }
        else if (curLCDMiddle && !lstLCDMiddle)
        {
          gLCDScreen = selectFlag;
        }
        break;
      }
    }

    end:
    lstLCDLeft = curLCDLeft;
    lstLCDMiddle = curLCDMiddle;
    lstLCDRight = curLCDRight;

    sleep(10);
  }
}
