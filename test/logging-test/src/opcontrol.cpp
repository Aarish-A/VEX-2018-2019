#include "main.h"
#include <stdarg.h>

FILE* log_file = NULL;

void log_init() {
  log_file = fopen("/usd/log2.txt", "a");
  printf(" >>>> %d log_init(): Start Logging for Program \n", pros::millis());
	if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
		return;
	}
  printf(" >>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
  fprintf(log_file, " >>>> %d Start Logging for Program \n", pros::millis());
}


void log(const char * format, ...) {
  va_list args;
  va_start(args, format);

  //vfprintf(stdout, format, args); //Print to stdout
  if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
	}
  else {
		// vprintf(format, args);
    vfprintf(log_file, format, args); //Print to log_file and flush log_file
    fflush(log_file);
  }

	printf("REACHED END OF LOG FUNCTION\n");

  va_end (args);
}

void opcontrol() {
	uint32_t startTime = pros::millis();
	int numLogs = 100;
	log_init();

	log("STARTED LOGGING IN OPCONTROL: %d\n", startTime);

	for(int i = 0; i < numLogs; i++) {
		log("TEST LOG %d\r\n", i);
	}

	log("STOPPED LOGGING... TOOK %d MILLISECONDS TO LOG %d TIMES\n", pros::millis() - startTime, numLogs);
	// fclose(log_file);
	printf("FILE CLOSED");

	while (true) {
		pros::delay(10);
	}
}
