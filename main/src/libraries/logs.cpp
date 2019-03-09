#include "logs.hpp"

pros::Mutex mutex;
char log_buffer[LOG_BUFFER_SIZE];
int buffer_write_loc = 0;
int buffer_flush_loc = 0;
int buffer_head = 0;
int buffer_tail = 0;

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

void _log_ln_internal(const char * format, ...) {
  mutex.take(LOG_MUTEX_TO);

  va_list args;
  va_start(args, format);

  // Write to console
  vprintf(format, args);
  printf("\n");

  // Write to file from buffer
  int write_amount = vsprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_loc, format, args);
  if (write_amount > 0) buffer_write_loc += sizeof(log_buffer[0])*write_amount;

  va_end (args);
  mutex.give();
}

void buffer_to_sd() {
  size_t size = sizeof(log_buffer[0]);
  while (true) {
    while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(3);

    mutex.take(LOG_MUTEX_TO);

    int count = buffer_write_loc - buffer_flush_loc; // only works if buffer_write_loc is greater than buffer_flush_lox TODO: FIX!!
    if (count > 0) {
      int flush_amount = fwrite(log_buffer+size*buffer_flush_loc, size, count, log_file);
      if (flush_amount > 0) buffer_flush_loc += size*flush_amount;
      fclose(log_file);
    }

    mutex.give();

    pros::delay(LOG_CLOSE_TIME);
  }
}

void log_ln(Log_Info infoA, const char * format, ...) {
  va_list args;
  va_start(args, format);
  if (infoA.enabled) {
    // TODO: Add TimeStamp & Category to args list
    _log_ln_internal(format, args);
  }
  va_end (args);

}
