#include "logs.hpp"

pros::Mutex mutex;

FILE* log_file = NULL;

const char* log_file_name = "/usd/log.txt";
int log_close_timer = 0;
const char* const log_mode = "a";

void log_init() {
  time_t current_time;
  char* c_time_string;
  log_file = fopen(log_file_name, log_mode);
  log_close_timer = 0;
  printf(">>>> %d log_init(): Start Logging for Program \n", pros::millis());
	if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
     //ctrler.rumble("--------");
    //pros::delay(3000);
		return;
	}
  printf(">>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
	// fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
  fprintf(log_file, ">>>> %d Start Logging for Program \n", pros::millis());

  log_close_timer = pros::millis() + LOG_CLOSE_TIME;
  printf("time closed");
}

void _{}//_internal(const char * format, ...) {
  mutex.take(LOG_MUTEX_TO);

  va_list args;
  va_start(args, format);
  // if (log_file == NULL) log_file = fopen(log_file_name, log_mode);
  vprintf(format, args);
  printf("\n");
  if (log_file == NULL) {
    //printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
  }
  else {
    vfprintf(log_file, format, args);
    fprintf(log_file, "\r\n");

    if (pros::millis() > log_close_timer) {
      log_close_timer = pros::millis() + LOG_CLOSE_TIME;

      fclose(log_file);
      while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(3);
    }
  }
  va_end (args);
  mutex.give();
}

void {}//(Log_Info infoA, const char * format, ...) {
  va_list args;
  va_start(args, format);
  if (infoA.enabled) {
    // TODO: Add TimeStamp & Category to args list
    _{}//_internal(format, args);
  }
  va_end (args);

}
