#include "main.h"
#include <stdarg.h>

/* Logging */
const int LOG_CLOSE_TIME = 500;
FILE* log_file = NULL; //fopen("/usd/log.txt", "w+");
const char* log_file_name = "/usd/log3.txt";
const char* log_mode = "a";
int log_close_timer = 0;
void log_init() {
  log_file = fopen(log_file_name, log_mode);
  log_close_timer = 0;
  printf(">>>> %d log_init(): Start Logging for Program \n", pros::millis());
	if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
		return;
	}
  printf(">>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
  fprintf(log_file, ">>>> %d Start Logging for Program \n", pros::millis());

  log_close_timer = pros::millis() + LOG_CLOSE_TIME;
}

void log(const char * format, ...) {
  va_list args;
  va_start(args, format);

  vprintf(format, args); //Print to stdout
  if (log_file == NULL) {
		//printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
	}
  else {
    vfprintf(log_file, format, args); //Print to log_file and flush log_file
    //fflush(log_file);

    if (pros::millis() > log_close_timer) {
      log_close_timer = pros::millis() + LOG_CLOSE_TIME;

      fclose(log_file);
      while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(3);

      printf(">>> %d Close File\n", pros::millis());
      fprintf(log_file, ">>> %d Close File\r\n", pros::millis());
    }
  }

  va_end (args);
}

void log_ln(const char * format, ...) {
  va_list args;
  va_start(args, format);

  vprintf(format, args); //Print to stdout
  printf("\n"); //Print to stdout
  if (log_file == NULL) {
		//printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
	}
  else {
    vfprintf(log_file, format, args); //Print to log_file and flush log_file
    fprintf(log_file, "\r\n");
    //fflush(log_file);

    if (pros::millis() > log_close_timer) {
      log_close_timer = pros::millis() + LOG_CLOSE_TIME;

      fclose(log_file);
      while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(3);

      printf(">>> %d Close File\n", pros::millis());
      fprintf(log_file, ">>> %d Close File\r\n", pros::millis());
    }
  }

  va_end (args);
}

void opcontrol() {
	uint32_t startTime = pros::millis();
	int numLogs = 100;
	log_init();

	log_ln("STARTED LOGGING IN OPCONTROL: %d\n", startTime);

	for(int i = 0; i < numLogs; i++) {
		log_ln("%d TEST LOG %d\r\n", pros::millis(), i);
	}

	log_ln("STOPPED LOGGING... TOOK %d MILLISECONDS TO LOG %d TIMES\n", pros::millis() - startTime, numLogs);
  pros::delay(1000);
  log_ln("%d Second Last print after wait 1sec \n", pros::millis());
  pros::delay(500);
  log_ln("%d Last print after wait 1sec \n", pros::millis());
	// fclose(log_file);
	//printf("FILE CLOSED");

	while (true) {
		pros::delay(10);
	}
}
