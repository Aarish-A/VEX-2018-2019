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
	//pros::delay(3500);
	while(true)
	{
		open_log_file();

		if (log_file != NULL)
		{
			//flush
			int flush_amount = fwrite(arr, sizeof(char), ARR_SIZE, log_file);
			if (flush_amount > 0 || !ferror(log_file))  {
				//printf("   >>> %d fwrite LOG_FILE: %p | Ferror:%d | errno: %d | %ld \n", pros::millis(), log_file, ferror(log_file), errno, counter);
			} else {
				if (flush_amount <= 0) printf("\n%d Flushed 1FAIL %d | Ferror:%d | errno: %d %s | %p\n", pros::millis(), flush_amount, ferror(log_file), errno, strerror(errno), log_file);
				if (ferror(log_file)) printf("\n%d Flushed 2FAIL %d | Ferror:%d | errno: %d %s | %p\n", pros::millis(), flush_amount, ferror(log_file), errno, strerror(errno), log_file);
			}
			pros::delay(10);

			//close
			close_log_file();
		}
		counter++;
	}
}
