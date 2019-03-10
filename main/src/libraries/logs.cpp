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
  char str_end[] = "\r\n";
  size_t str_whole_len = strlen(format)+strlen(str_end); // Amount of chars in input-string
  char str_whole[str_whole_len];
  std::strcpy(str_whole, format);
  std::strcat(str_whole, str_end);

  int write_amount = 0;
  if (mutex.take(LOG_MUTEX_TO)) {
    size_t buf_remaining_len = LOG_BUFFER_SIZE - buffer_write_index; // Amount of unfilled indices left in buffer
    if (buf_remaining_len < str_whole_len) { // If the input-string is about to go out of the bounds of the buffer
                                              // Split the input-string in two. Put whatever fits into the end of the buffer
                                              // and put the remaining characters into the front of the buffer
      printf(">>%d log_ln() WRITE->BUFFER wrapped around \n", pros::millis());
      // Split whole string into two substrings
      const size_t str_first_len = buf_remaining_len;
      const size_t str_sec_len = str_whole_len-str_first_len;
      char str_first[str_first_len];
      char str_sec[str_sec_len];
      strncpy(str_first, str_whole, str_first_len);
      strncpy(str_sec, &str_whole[str_first_len], str_sec_len);
      // 1) Write first part of string to end of buffer
      write_amount = vsprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_index, str_first, args);
      if (write_amount > 0) buffer_write_index += write_amount;
      printf("  >>%d log_ln() | write_amount = %d | buffer_write_index = %d \n", pros::millis(), write_amount, buffer_write_index);
      // 2) Write sec part of string to front of buffer
      write_amount = vsprintf(log_buffer, str_sec, args); // HOW WILL ARGS BE HANDLED? WILL IT USE THE RIGHT ARGS B/W THE TWO FUNC CALLS?
      if (write_amount > 0) buffer_write_index = write_amount;
      printf("  >>%d log_ln() | write_amount = %d | buffer_write_index = %d \n", pros::millis(), write_amount, buffer_write_index);
    }
    else {
      int write_amount = vsprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_index, str_whole, args);
      if (write_amount > 0) {
        buffer_write_index += write_amount;
      }
    }

    mutex.give();

  } else printf(">>> %d _log_ln_internal() Mutex take failed (TO = 50ms) | Err:%d \n", pros::millis(), errno);


}

void buffer_to_sd() {
  log_ln(PROGRAM_FLOW, "%d Start buffer_to_sd() task \n", pros::millis());
  int flush_amount = 0;
  while (true) {
      //printf("   >>>>>>>>%d NULL LOG_FILE \n", pros::millis());
      while (log_file == NULL) {
        log_file = fopen(log_file_name, log_mode);
        //printf("  >>>>>>>>%d Opening file \n", pros::millis());
        pros::delay(2);
      }

    printf("  >>>%d Opened file \n", pros::millis());
    if (mutex.take(LOG_MUTEX_TO))
    {
      //printf(">>>%d Start flush \n", pros::millis());
      int count = buffer_write_index - buffer_flush_index;
      if (count > 0) { // The last character flushed from buf is behind last character written to it
        // Flush everything b/w buffer_flush_index and buffer_write_index
        flush_amount = fwrite(log_buffer+BUF_OBJ_SIZE*buffer_flush_index, BUF_OBJ_SIZE, count, log_file);
        if (flush_amount >  0) buffer_flush_index += flush_amount;
        printf("%d flush %d %d\n", pros::millis(), buffer_write_index, buffer_flush_index);
        fclose(log_file);
        log_file = fopen(log_file_name, log_mode);
      }
      else if (count < 0) { // We wrapped around when writing to the buffer (meaning the last character written is behind the last character flushed)
        printf(">>%d buffer_to_sd() BUFFER->SD wrapped around \n", pros::millis());
        // 1) Flush b/w buffer_flush_index & buffer_end
        size_t buf_remaining_len = LOG_BUFFER_SIZE - buffer_flush_index; // Amount of unflushed indices left in buffer
        flush_amount = fwrite(log_buffer+BUF_OBJ_SIZE*buffer_flush_index, BUF_OBJ_SIZE, buf_remaining_len, log_file);
        if (flush_amount >  0) buffer_flush_index += flush_amount;
        printf("  >>%d buffer_to_sd() | flush_amount = %d | buffer_flush_index = %d \n", pros::millis(), flush_amount, buffer_flush_index);
        // 2) Flush b/w buffer_front and buffer_write_index
        flush_amount = fwrite(log_buffer, BUF_OBJ_SIZE, buffer_write_index, log_file);
        if (flush_amount >  0) buffer_flush_index = flush_amount;
        printf("  >>%d buffer_to_sd() | flush_amount = %d | buffer_flush_index = %d \n", pros::millis(), flush_amount, buffer_flush_index);
      }

      mutex.give();

    } else printf(">>> %d buffer_to_sd() Mutex Take Failed (TO = TIMEOUT_MAX) | Err:%d \n", pros::millis(), errno);

    pros::delay(LOG_BUFFER_FLUSH_DELAY);
  }
}

void log_ln(Log_Info info_category, const char * format, ...) {
  va_list args;
  va_start(args, format);

  char new_str[12+info_category.name.length()+strlen(format)];
  sprintf(new_str, "%07d | %s | %s", pros::millis(), info_category.name.c_str(), format);

  if (info_category.enabled) {
    // TODO: Add TimeStamp & Category to args list
    _log_ln_internal(new_str, args);
  }
  va_end (args);

}
