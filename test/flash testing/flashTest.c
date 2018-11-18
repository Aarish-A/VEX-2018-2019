#include <FlashLib.h>

#define LCD_L (curLCDLeft && !lstLCDLeft)
#define LCD_R (curLCDRight && !lstLCDRight)
#define LCD_M (curLCDMiddle && !lstLCDMiddle)

int integer = 300;
static char tempData[sizeof(integer)];
unsigned char* data;
int fileLength;
task main()
{
	clearDebugStream();
	writeDebugStreamLine("%d | Starting Program...", nPgmTime);

	// If file is found, copy contents of file into integer number
	// If file isn't found, write integer number to new file
	//if (RCFS_GetFile("anglerPos", &data, &fileLength) == RCFS_SUCCESS)
	//{
	//	memcpy(&integer, data, sizeof(integer));
	//	writeDebugStreamLine("%d | Copied data from file, Integer = %d", nPgmTime, integer);
	//}
	//else
	//{
		writeDebugStreamLine("%d | Could not find file", nPgmTime);
		memcpy(&tempData, &integer, sizeof(integer));
		if(RCFS_AddFile(tempData, sizeof(integer), "angler position") == RCFS_ERROR)
			writeDebugStreamLine("%d | File Write Error", nPgmTime);
		else writeDebugStreamLine("%d | Wrote new file", nPgmTime);
	//}
	writeDebugStreamLine("%d", integer);

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

	bool curLCDLeft, lstLCDLeft;
  bool curLCDMiddle, lstLCDMiddle;
  bool curLCDRight, lstLCDRight;

	clearLCDLine(0);
	clearLCDLine(1);

	while(true)
	{
		if (nLCDButtons == 1) curLCDLeft = true;
    else curLCDLeft = false;

    if (nLCDButtons == 2) curLCDMiddle = true;
    else curLCDMiddle = false;

    if (nLCDButtons == 4) curLCDRight = true;
    else curLCDRight = false;

    if(LCD_L) integer--;
    else if (LCD_R) integer++;
    else if (LCD_M) break;

    displayLCDCenteredString(0, "Adjust Integer");
    displayLCDString(1, 0, "Integer: ");
    displayNextLCDNumber(integer, 5);

    lstLCDLeft = curLCDLeft;
    lstLCDMiddle = curLCDMiddle;
    lstLCDRight = curLCDRight;
    sleep(10);
	}
	clearLCDLine(0);
	clearLCDLine(1);
	displayLCDCenteredString(0, "Saved!");

	// Write new integer value to new file
	//memcpy(&tempData, &integer, sizeof(integer));
	//if(RCFS_AddFile(tempData, sizeof(integer), "anglerPos") == RCFS_ERROR)
	//	writeDebugStreamLine("%d | File Write Error", nPgmTime);
	//else writeDebugStream("%d | Wrote new file", nPgmTime);

	sleep(5000);


}
