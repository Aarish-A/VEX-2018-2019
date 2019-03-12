#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "subsystems/intake.hpp"
#include "libraries/subsystem.hpp"

void opcontrol() {
	constexpr int ARR_SIZE = 1000;
	char arr[ARR_SIZE];
	for (int i = 0; i < ARR_SIZE; i++) arr[i] = 'c';
	long counter = 0;
	while(true)
	{
		if (log_file == NULL) log_file = fopen(log_file_name, log_mode);
		if (log_file == NULL || errno != 0) {
			printf(" ERR %d log_file fopen failed | errno: %d | %ld\n", pros::millis(), errno, counter);
		} else printf("\n       >>> %d OPEN LOG_FILE: %p | %ld \n", pros::millis(), log_file, counter);

		int flush_amount = fwrite(arr, sizeof(char), ARR_SIZE, log_file);
		if (flush_amount > 0 || !ferror(log_file))  {
			printf("   >>> %d fwrite LOG_FILE: %p | Ferror:%d | errno: %d | %ld \n", pros::millis(), log_file, ferror(log_file), errno, counter);
		} else {
			if (flush_amount <= 0) printf("\n%d Flushed 1FAIL %d | Ferror:%d | errno: %d \n", pros::millis(), flush_amount, ferror(log_file), errno);
			if (ferror(log_file)) printf("\n%d Flushed 2FAIL %d | Ferror:%d | errno: %d \n", pros::millis(), flush_amount, ferror(log_file), errno);
		}
		//close
		if (log_file != NULL)
		{
			int f_close_ret = -1;
			while (f_close_ret != 0)
			{
				f_close_ret = fclose(log_file);
				if (f_close_ret != 0) {
					printf("  %d ERR log_file fclose failed | errno: %d | f_close_ret: %d | %ld\n", pros::millis(), errno, f_close_ret, counter);
				}
			 else {
				 log_file = NULL; // Set log_file to null upon successful close
				 printf("    >>> %d CLOSELOG_FILE: %p | %ld \n", pros::millis(), log_file, counter);
			 }
		 }
		}
		counter++;
	}
}
