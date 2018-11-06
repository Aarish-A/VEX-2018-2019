task handleLCD()
{
  bool curLCDLeft, lstLCDLeft;
  bool curLCDMiddle, lstLCDMiddle;
  bool curLCDRight, lstLCDRight;
  int pointer = 2;

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
      		if (LCD_R) gDriveBreakWait += 10;
      		if (LCD_L) gDriveBreakWait -= 10;

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
      case sensorView:
      {
      	string anglerSen, shooterSen;
      	sprintf(anglerSen, "Angler: %d", SensorValue[anglerPoti]);
      	sprintf(shooterSen, "Shooter: %d", SensorValue[shooterEnc]);
      	displayLCDCenteredString(0, anglerSen);
      	displayLCDCenteredString(1, shooterSen);

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
        displayLCDCenteredString(0, "Front Mid Back");
        if (pointer == 0) displayLCDChar(1, 0, '<');
        else if (pointer == 1) displayLCDChar(1, 3, '^');
        else if (pointer == 2) displayLCDChar(1, 8, '^');
        else if (pointer == 3) displayLCDChar(1, 12, '^');

        // Controls
        if (curLCDLeft && !lstLCDLeft)
        {
          if (pointer > 0) pointer--;
          else gLCDScreen = anglerTargetTuning;
        }
        else if (curLCDRight && !lstLCDRight)
        {
          if (pointer < 3) pointer++;
        }
        else if (curLCDMiddle && !lstLCDMiddle)
        {
          if (pointer == 1) gCurLCDSelection.position = 0;
          else if (pointer == 2) gCurLCDSelection.position = 1;
          else if (pointer == 3) gCurLCDSelection.position = 2;

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
        else if (pointer == 3) pointer = 2;

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
            case 0:
              if (gCurLCDSelection.flag == 0)
              {
                gCurLCDSelection.name = "Front Mid: ";
                gCurLCDSelection.value = &gAnglerFrontPFMidFlag;
              }
              else if (gCurLCDSelection.flag == 1)
              {
                gCurLCDSelection.name = "Front Top: ";
                gCurLCDSelection.value = &gAnglerFrontPFTopFlag;
              }
              break;
            case 1:
              if (gCurLCDSelection.flag == 0)
              {
                gCurLCDSelection.name = "Mid Mid ";
                gCurLCDSelection.value = &gAnglerBackPFMidFlag;
              }
              else if (gCurLCDSelection.flag == 1)
              {
                gCurLCDSelection.name = "Mid Top ";
                gCurLCDSelection.value = &gAnglerBackPFTopFlag;
              }
              break;
            case 2:
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
