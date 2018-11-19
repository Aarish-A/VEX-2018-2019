#pragma config(Sensor, dgtl1,  saver,          sensorTouch)
#pragma config(Sensor, dgtl2,  adder,          sensorTouch)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Limit switch with plate = add
// Limit switch without plate = save

#include <FlashLib.h>

#define LCD_L (curLCDLeft && !lstLCDLeft)
#define LCD_R (curLCDRight && !lstLCDRight)
#define LCD_M (curLCDMiddle && !lstLCDMiddle)

int integer = 400;
static char tempData[sizeof(integer)];
unsigned char* data;
int fileLength;
char name[16];

task main()
{
	clearDebugStream();
	writeDebugStreamLine("%d | Starting Program...", nPgmTime);

	// If file is found, copy contents of file into integer number
	// If file isn't found, write integer number to new file
	//RCFS_GetLastFilename(name, 16);

	if (RCFS_GetFile("debug001", &data, &fileLength) == RCFS_SUCCESS)
	{
		RCFS_GetLastFilename(name, 16);
		RCFS_GetFile(name, &data, &fileLength)
		memcpy(&integer, data, sizeof(integer));
		writeDebugStreamLine("%d | Copied data from file, Integer = %d", nPgmTime, integer);
		writeDebugStreamLine("Last file written was %s", &name[0]);
 		writeDebugStreamLine("data ptr is %08X", data);
 		writeDebugStreamLine("length is %08X", fileLength);
	}
	else
	{
		writeDebugStreamLine("%d | Could not find file", nPgmTime);
		memcpy(&tempData, &integer, sizeof(integer));
		//if(RCFS_AddFile(tempData, sizeof(integer), "Angler Position") == RCFS_ERROR)
		if (RCFS_AddFile(tempData, sizeof(integer)) == RCFS_ERROR)
			writeDebugStreamLine("%d | File Write Error", nPgmTime);
		else writeDebugStreamLine("%d | Wrote new file", nPgmTime);
	}
	writeDebugStreamLine("%d | Integer at %d", nPgmTime, integer);

	// Get info of last file
	//int newInt = 0;
	//char name[16];
	//RCFS_GetLastFilename(name, 16);
	//RCFS_GetFile(name, &data, &fileLength);
 	//writeDebugStreamLine("Last file written was %s", &name[0]);
 	//writeDebugStreamLine("data ptr is %08X", data);
 	//writeDebugStreamLine("length is %08X", fileLength);
 	//memcpy(&newInt, data, sizeof(newInt));
 	//writeDebugStreamLine("File Number: %d", newInt);

	// Get list/info of all files on flash
	RCFS_ReadVTOC();

	//bool curLCDLeft, lstLCDLeft;
 // bool curLCDMiddle, lstLCDMiddle;
 // bool curLCDRight, lstLCDRight;
	bool curAdder, lastAdder;
	bool curSaver, lastSaver;

	clearLCDLine(0);
	clearLCDLine(1);

	while(true)
	{
		curAdder = SensorValue[adder];
		curSaver = SensorValue[saver];

		if (curAdder && !lastAdder)
		{
			integer++;
			writeDebugStreamLine("%d | Integer bumped up to %d", nPgmTime, integer);
		}
		else if (curSaver && !lastSaver) break;

    sleep(10);
		//if (nLCDButtons == 1) curLCDLeft = true;
  //  else curLCDLeft = false;

  //  if (nLCDButtons == 2) curLCDMiddle = true;
  //  else curLCDMiddle = false;

  //  if (nLCDButtons == 4) curLCDRight = true;
  //  else curLCDRight = false;

  //  if(LCD_L) integer--;
  //  else if (LCD_R) integer++;
  //  else if (LCD_M) break;

  //  displayLCDCenteredString(0, "Adjust Integer");
  //  displayLCDString(1, 0, "Integer: ");
  //  displayNextLCDNumber(integer, 5);

    	lastAdder = curAdder;
    	lastSaver = curSaver;
  //  lstLCDLeft = curLCDLeft;
  //  lstLCDMiddle = curLCDMiddle;
  //  lstLCDRight = curLCDRight;
	}
	//clearLCDLine(0);
	//clearLCDLine(1);
	//displayLCDCenteredString(0, "Saved!");

	// Write new integer value to new file
	memcpy(&tempData, &integer, sizeof(integer));
	//if(RCFS_AddFile(tempData, sizeof(integer), "Angler Position") == RCFS_ERROR)
	if (RCFS_AddFile(tempData, sizeof(integer)) == RCFS_ERROR)
		writeDebugStreamLine("%d | File Write Error", nPgmTime);
	else writeDebugStream("%d | Wrote new file", nPgmTime);

	RCFS_ReadVTOC();
	sleep(5000);
}
