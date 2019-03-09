#include "logs.hpp"

pros::Mutex mutex;
char log_buffer[LOG_BUFFER_SIZE];
int buffer_write_index = 0;
int buffer_flush_index = 0;
int buffer_head = 0;
int buffer_tail = 0;

FILE* log_file = NULL;

const char* log_file_name = "/usd/log.txt";
const char* const log_mode = "a";

void log_init() {
  log_file = fopen(log_file_name, log_mode);
  printf(">>>> %d log_init(): Start Logging for Program \n", pros::millis());
	if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
		return;
	}
  printf(">>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
  fprintf(log_file, ">>>> %d Start Logging for Program \n", pros::millis());

  fclose(log_file);
}

void _log_ln_internal(const char * format, va_list args) {

  // Write to console
  vprintf(format, args);
  printf("\n");

  // Write to file from buffer
  if (mutex.take(LOG_MUTEX_TO)) {
    char sd_end[] = "\r\n";
    char sd_new[strlen(format)+strlen(sd_end)];
    std::strcpy(sd_new, format);
    std::strcat(sd_new, sd_end);
    int write_amount = vsprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_index, sd_new, args);
    if (write_amount > 0) {
      buffer_write_index += write_amount;
    }

    mutex.give();

  } else printf(">>> %d _log_ln_internal() Mutex take failed (TO = 50ms) | Err:%d \n", pros::millis(), errno);


}

void buffer_to_sd() {
  size_t size = sizeof(log_buffer[0]);
  log_ln(PROGRAM_FLOW, "%d Start buffer_to_sd() task \n", pros::millis());
  while (true) {
    while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(2);

    if (mutex.take(TIMEOUT_MAX))
    {
      int count = buffer_write_index - buffer_flush_index; // only works if buffer_write_index is greater than buffer_flush_lox TODO: FIX!!
      if (count > 0) {
        int flush_amount = fwrite(log_buffer+size*buffer_flush_index, size, count, log_file);
        if (flush_amount > 0) buffer_flush_index += flush_amount;
        printf("%d flush \n", pros::millis());
        fclose(log_file);
      }

      mutex.give();

    } else printf(">>> %d buffer_to_sd() Mutex Take Failed (TO = TIMEOUT_MAX) | Err:%d \n", pros::millis(), errno);

    pros::delay(LOG_BUFFER_FLUSH_DELAY);
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
