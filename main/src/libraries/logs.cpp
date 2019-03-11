#include "logs.hpp"
bool sd_logging_enabled = true;
pros::Mutex sd_buffer_mutex;
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
  while (log_file == NULL && open_attempt_count < MAX_ALLOWABLE_OPEN_ATTEMPTS) {
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
    close_log_file();
  }
  printf("%d DONE log_init() \n", pros::millis());
}

void close_log_file() {
  if (log_file != NULL) {
    int f_close_ret = -1;
    int attempt_cnt  = 0;
    while (f_close_ret != 0 && attempt_cnt < MAX_ALLOWABLE_CLOSE_ATTEMPTS) {
      f_close_ret = fclose(log_file);
      attempt_cnt++;
      pros::delay(1);
    }
    if (f_close_ret == 0) log_file = NULL;
    // ELSE KILL BUFF_SD TASK IN FEAR OF MAKING CODE HANG
    printf(" \n \n%d FClose: %d took %d tries\n \n", pros::millis(), f_close_ret, attempt_cnt);
  }
}

void log_ln_internal(const char * str_whole) {

  // Write to console
  printf("%s",str_whole);

  if (sd_logging_enabled)
  {
    // Write to sd_logging buffer
    size_t str_whole_len = strlen(str_whole); // Amount of chars in input-string
    int write_amount = 0;
    if (sd_buffer_mutex.take(LOG_MUTEX_TO))
    {
      //printf("    %d LOG TOOK sd_buffer_mutex \n", pros::millis());
      size_t buf_remaining_len = LOG_BUFFER_SIZE - buffer_write_index; // Amount of unfilled indices left in buffer
      if (buf_remaining_len < str_whole_len) { // If the input-string is about to go out of the bounds of the buffer
                                                // Split the input-string in two. Put whatever fits into the end of the buffer
                                                // and put the remaining characters into the front of the buffer
        //printf(">>%d log_ln() WRITE->BUFFER wrapped around \n", pros::millis());
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
        //printf("  >>%d log_ln() | write_amount = %d | buffer_write_index = %d \n", pros::millis(), write_amount, buffer_write_index);
        // 2) Write second part of string to front of buffer
        write_amount = sprintf(log_buffer, "%s", str_sec.c_str()); // HOW WILL ARGS BE HANDLED? WILL IT USE THE RIGHT ARGS B/W THE TWO FUNC CALLS?
        if (write_amount > 0) buffer_write_index = write_amount;
        //printf("  >>%d log_ln() | write_amount = %d | buffer_write_index = %d \n", pros::millis(), write_amount, buffer_write_index);
      }
      else {
        //printf("  >>%d log_ln() WRITE->BUFFER good around \n", pros::millis());
        write_amount = sprintf(log_buffer+sizeof(log_buffer[0])*buffer_write_index, "%s", str_whole);
        if (write_amount > 0) {
          buffer_write_index += write_amount;
        }
      }
      //printf("    %d buffer: %d \n", pros::millis(), buffer_write_index);
      sd_buffer_mutex.give();
      //printf("%d give sd_buffer_mutex \n", pros::millis());

    } else printf(">>> %d log_ln_internal() sd_buffer_mutex take failed (TO = 50ms) | Err:%d \n", pros::millis(), errno);
  }

}

void buffer_to_sd() {
  printf("  %d PRINT - buffer start \n", pros::millis());
  log_ln(PROGRAM_FLOW, "%d Start buffer_to_sd() task \n", pros::millis());
  int flush_amount = 0;
  while (true) {
    //printf(" \n\n  >>>>>>>>%d WHILE TOP \n \n", pros::millis());
    while (log_file == NULL) {
      log_file = fopen(log_file_name, log_mode);
      printf("  \n>>>>>>>>%d Opening file \n\n", pros::millis());
      pros::delay(2);
    }

    //printf("                  \n  >>>%d Opened file <<< \n\n", pros::millis());
    if (sd_buffer_mutex.take(TIMEOUT_MAX))
    {
      int count = buffer_write_index - buffer_flush_index;
      printf(">>>%d Start flush |cnt:%d \n", pros::millis(), count);

      if (count > 0) { // The last character flushed from buf is behind last character written to it
        // Flush everything b/w buffer_flush_index and buffer_write_index
        flush_amount = -1;
        int attempt_count = 0;
        while (flush_amount < 1 && attempt_count < 10) {
          const char* FLUSH_START_POS = log_buffer+BUF_OBJ_SIZE*buffer_flush_index;
          printf("%d >>>BEFORE FLUSH %d amnt: %d | start: %p | startacc: %p \n", pros::millis(), attempt_count, flush_amount, log_buffer, FLUSH_START_POS);
          flush_amount = fwrite(FLUSH_START_POS, BUF_OBJ_SIZE, count, log_file);
          printf("%d >>>after FLUSH %d amnt: %d | err: %d\n", pros::millis(), attempt_count, flush_amount, ferror(log_file));
          attempt_count++;
          pros::delay(2);
        }
        if (flush_amount >  0) buffer_flush_index += flush_amount;
        printf("%d flush | count: %d | %d %d \n", pros::millis(), count, buffer_write_index, buffer_flush_index);
        close_log_file();
        printf ("\n\n%d Fopen\n\n", pros::millis());

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
        close_log_file();
      }
      //printf(" \n\n  >>>>>>>>%d WHILE 2 BOT \n \n", pros::millis());

      sd_buffer_mutex.give();

    } else printf(">>> %d buffer_to_sd() sd_buffer_mutex Take Failed (TO = TIMEOUT_MAX) | Err:%d \n", pros::millis(), errno);

    //printf(" \n\n  >>>>>>>>%d WHILE BOT \n \n", pros::millis());
    pros::delay(1000);
    //printf(" \n\n\n\n\n\n  >>>>>>>>%d WHILE WAIT \n\n\n\n", pros::millis());
    //pros::delay(1000);
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

  if (info_category.enabled)
  {
    //printf("     L Enter >>>  \n");
    int size = ((int)strlen(format)) * 2 + 50;   // Initial str size
    std::string formatted_str;
    formatted_str.resize(size);
    int formatted_len = -1;
    int copy_attempt_cnt = 0;
    while (formatted_len < 0 && copy_attempt_cnt < 5) {
      formatted_len = vsnprintf((char *)formatted_str.data(), size, format, args);
      copy_attempt_cnt++;
    }
    if (formatted_len > 1) {
      formatted_str.resize(formatted_len);
      std::string new_str;
      new_str.resize(15 + formatted_len + info_category.name.length());

      sprintf((char *)new_str.data(), "%07d | %s | %s\r\n", pros::millis(), info_category.name.c_str(), formatted_str.c_str());
      //printf("\n   %d L >>> %s \n", new_str.length(), new_str.c_str());
      log_ln_internal(new_str.c_str());
      //printf("   L2 >>> %s \n", new_str.c_str());namespace  {
    }
  }

  va_end (args);
}
