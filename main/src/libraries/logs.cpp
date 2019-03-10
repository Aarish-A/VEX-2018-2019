#include "logs.hpp"
bool sd_logging_enabled = true;
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
  printf("%d log_init() \n", pros::millis());
  printf(">>>> %d log_init(): Start Logging for Program \n", pros::millis());
  int open_attempt_count = 0;
  while (log_file == NULL && open_attempt_count < 50) {
    log_file = fopen(log_file_name, log_mode);
    printf("    %d log_init() OPEN ATTEMPT %d\n", pros::millis(), open_attempt_count);
    open_attempt_count++;
    pros::delay(2);
  }
	if (log_file == NULL) {
    sd_logging_enabled = false;
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE \n", pros::millis());
	}
  else {
    sd_logging_enabled = true;
    printf(">>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
  	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
    fprintf(log_file, ">>>> %d Start Logging for Program \n", pros::millis());
    fclose(log_file);
  }
  printf("%d DONE log_init() \n", pros::millis());
}

void _log_ln_internal(const char * str_whole) {

  // Write to console
  printf("%s",str_whole);

  if (sd_logging_enabled)
  {
    // Write to sd_logging buffer
    size_t str_whole_len = strlen(str_whole); // Amount of chars in input-string
    int write_amount = 0;
    if (mutex.take(LOG_MUTEX_TO))
    {
      size_t buf_remaining_len = LOG_BUFFER_SIZE - buffer_write_index; // Amount of unfilled indices left in buffer
      if (buf_remaining_len < str_whole_len) { // If the input-string is about to go out of the bounds of the buffer
                                                // Split the input-string in two. Put whatever fits into the end of the buffer
                                                // and put the remaining characters into the front of the buffer
        printf(">>%d log_ln() WRITE->BUFFER wrapped around \n", pros::millis());
        // Split whole string into two substrings
        const size_t str_first_len = buf_remaining_len;
        const size_t str_sec_len = str_whole_len-str_first_len;
        std::string str_first, str_sec;
        str_first.resize(str_first_len);
        str_sec.resize(str_sec_len);
        strncpy((char*) str_first.data(), str_whole, str_first_len);
        strncpy((char*) str_sec.data(), &str_whole[str_first_len], str_sec_len);
        // 1) Write first part of string to end of buffer
        write_amount = sprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_index, "%s", str_first.c_str());
        if (write_amount > 0) buffer_write_index += write_amount;
        printf("  >>%d log_ln() | write_amount = %d | buffer_write_index = %d \n", pros::millis(), write_amount, buffer_write_index);
        // 2) Write second part of string to front of buffer
        write_amount = sprintf(log_buffer, "%s", str_sec.c_str()); // HOW WILL ARGS BE HANDLED? WILL IT USE THE RIGHT ARGS B/W THE TWO FUNC CALLS?
        if (write_amount > 0) buffer_write_index = write_amount;
        printf("  >>%d log_ln() | write_amount = %d | buffer_write_index = %d \n", pros::millis(), write_amount, buffer_write_index);
      }
      else {
        write_amount = sprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_index, "%s", str_whole);
        if (write_amount > 0) {
          buffer_write_index += write_amount;
        }
      }

      mutex.give();

    } else printf(">>> %d _log_ln_internal() Mutex take failed (TO = 50ms) | Err:%d \n", pros::millis(), errno);
  }


}

void buffer_to_sd() {
  printf("  %d PRINT - buffer start \n", pros::millis());
  log_ln(PROGRAM_FLOW, "%d Start buffer_to_sd() task \n", pros::millis());
  int flush_amount = 0;
  while (true) {
      //printf("   >>>>>>>>%d NULL LOG_FILE \n", pros::millis());
      while (log_file == NULL) {
        log_file = fopen(log_file_name, log_mode);
        printf("  >>>>>>>>%d Opening file \n", pros::millis());
        pros::delay(2);
      }

    //printf("  >>>%d Opened file \n", pros::millis());
    if (mutex.take(LOG_MUTEX_TO))
    {
      printf(">>>%d Start flush \n", pros::millis());
      int count = buffer_write_index - buffer_flush_index;
      if (count > 0) { // The last character flushed from buf is behind last character written to it
        // Flush everything b/w buffer_flush_index and buffer_write_index
        flush_amount = fwrite(log_buffer+BUF_OBJ_SIZE*buffer_flush_index, BUF_OBJ_SIZE, count, log_file);
        if (flush_amount >  0) buffer_flush_index += flush_amount;
        printf("%d flush | count: %d | %d %d \n", pros::millis(), count, buffer_write_index, buffer_flush_index);
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

std::string string_format(const std::string fmt, va_list args) {
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), args);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

void log_ln(Log_Info info_category, const char * format, ...) {
  va_list args;
  va_start(args, format);
  //printf("     L Enter >>>  \n");

  if (info_category.enabled) {
    //printf("     L Enter >>>  \n");
    int size = ((int)strlen(format)) * 2 + 50;   // Initial str size
    std::string formatted_str;
    formatted_str.resize(size);
    int formatted_len = -1;
    int copy_attempt_cnt = 0;
    while (formatted_len < 0 && copy_attempt_cnt < 5) {
      formatted_len = vsnprintf((char *)formatted_str.data(), size, format, args);
      copy_attempt_cnt ++;
    }
    if (formatted_len > 1) {
      formatted_str.resize(formatted_len);
      std::string new_str;
      new_str.resize(15 + formatted_len + info_category.name.length());
      //printf("   %d L >>> %s \n", formatted_len, formatted_str.c_str());
      sprintf((char *)new_str.data(), "%07d | %s | %s\r\n", pros::millis(), info_category.name.c_str(), formatted_str.c_str());
      //printf("   L2 >>> %s \n", new_str.c_str());
      _log_ln_internal(new_str.c_str ());
    }
  }

  va_end (args);
}
