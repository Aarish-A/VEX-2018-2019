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
  printf(" %d log_init(): Start Logging for Program \n", pros::millis());
  int open_attempt_count = 0;
  while (log_file == NULL && open_attempt_count < MAX_ALLOWABLE_OPEN_ATTEMPTS) {
    log_file = fopen(log_file_name, log_mode);
    printf("     %d log_init() FILE-OPEN ATTEMPT %d\n", pros::millis(), open_attempt_count);
    open_attempt_count++;
    pros::delay(1);
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
  pros::delay(50);
  if (sd_logging_enabled) {
    log_ln(PROGRAM_FLOW, " Construct buffer_to_sd() task from log_init()");
    pros::Task buffer_to_sd_task((pros::task_fn_t) buffer_to_sd);
  }
  pros::delay(50);
  log_ln(PROGRAM_FLOW, "DONE LOG_INIT");
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
      //printf("        >>%d LOG_LN() TOOK sd_buffer_mutex \n", pros::millis());
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
        //printf("  >>%d log_ln() | write_amount = %d | %d %d \n", pros::millis(), write_amount, buffer_write_index, buffer_flush_index);
        // 2) Write second part of string to front of buffer
        write_amount = sprintf(log_buffer, "%s", str_sec.c_str()); // HOW WILL ARGS BE HANDLED? WILL IT USE THE RIGHT ARGS B/W THE TWO FUNC CALLS?
        if (write_amount > 0) buffer_write_index = write_amount;
        //printf("  >>%d log_ln() | write_amount = %d | %d %d \n", pros::millis(), write_amount, buffer_write_index, buffer_flush_index);
      }
      else {
        //printf("  >>%d log_ln() WRITE->BUFFER good around \n", pros::millis());
        write_amount = sprintf(log_buffer+BUF_OBJ_SIZE*buffer_write_index, "%s", str_whole);
        if (write_amount > 0) {
          buffer_write_index += write_amount;
        }
      }
      //printf("    %d buffer: %d \n", pros::millis(), buffer_write_index);
      sd_buffer_mutex.give();
      //printf("        >>%d LOG_LN() GAVE sd_buffer_mutex \n", pros::millis());

    } else printf(">>> %d log_ln_internal() sd_buffer_mutex take failed (TO = 50ms) | Err:%d \n", pros::millis(), errno);
  }

}

void flush_to_sd(int given_amnt_to_flush) {
  //Open file
  while (log_file == NULL) {
    log_file = fopen(log_file_name, log_mode);
    //printf("  \n>>>>>>>>%d Opening file \n\n", pros::millis());
    pros::delay(2);
  }
  //printf("                  \n  >>>%d Opened file <<< \n\n", pros::millis());

  int amnt_to_flush = std::min(given_amnt_to_flush, MAX_AMNT_TO_FLUSH); // Lim num of indices to flush to MAX_AMNT_TO_FLUSH

  const char* FLUSH_START_POS = &log_buffer[buffer_flush_index];
  //printf("%d >>>before ToFLUSH: %d | %d %d | \n", pros::millis(), amnt_to_flush, buffer_write_index, buffer_flush_index);
  int flush_amount = fwrite(FLUSH_START_POS, BUF_OBJ_SIZE, amnt_to_flush, log_file);
  //printf("%d >>>after FLUSH %d | %d %d | err: %d\n", pros::millis(), flush_amount,  buffer_write_index, buffer_flush_index, ferror(log_file));
  int old_bfi = buffer_flush_index;
  if (flush_amount >  0) buffer_flush_index += flush_amount;
  if (buffer_flush_index >= LOG_BUFFER_SIZE) {
    buffer_flush_index = 0; // If the pointer is at the end of the buffer, move it back to the front
    //printf("    >>%d BFI OVERFLOW -> Move to %d \n", pros::millis(), buffer_flush_index);
  }
  //printf("%d flush | count: %d | %d %d | old:%d acc:%d\n", pros::millis(), flush_amount, buffer_write_index, buffer_flush_index, old_bfi, old_bfi+flush_amount);
  close_log_file();
}

void buffer_to_sd() {
  printf("  %d PRINT - buffer start \n", pros::millis());
  log_ln(PROGRAM_FLOW, "%d Start buffer_to_sd() task \n", pros::millis());
  while (true) {
    if (sd_buffer_mutex.take(TIMEOUT_MAX))
    {
      int count = buffer_write_index - buffer_flush_index;
      if (count > 0) { // The last character flushed from buf is behind last character written to it
        // Flush everything b/w buffer_flush_index and buffer_write_index
        flush_to_sd(count);
      }
      else if (count < 0) { // We wrapped around when writing to the buffer (meaning the last character written is behind the last character flushed)
        printf("  >>%d buffer_to_sd() BUFFER->SD wrapped around \n", pros::millis());
        // Flush b/w buffer_flush_index & buffer_end. Move the pointer back to the beginning of the buffer
        int buf_remaining_len = LOG_BUFFER_SIZE - buffer_flush_index; // Amount of unflushed indices left in buffer
        flush_to_sd(buf_remaining_len);
      }

      sd_buffer_mutex.give();

    } else printf(">>> %d buffer_to_sd() sd_buffer_mutex Take Failed (TO = TIMEOUT_MAX) | Err:%d \n", pros::millis(), errno);

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

  if (info_category.enabled)
  {
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

void _test_flush_time() {
  constexpr int BUF_TEST_SIZE = 100;
	char buf_test[BUF_TEST_SIZE];
	constexpr int NUM_TESTS = 500;
	for (int i = 0; i < BUF_TEST_SIZE; i++) {
		buf_test[i] = (char)i;
	}
	buf_test[BUF_TEST_SIZE-1] = '\n';
	int dif_sum = 0;;
	for (int i = 0; i < NUM_TESTS; i++) {
		while (log_file == NULL) {
      log_file = fopen(log_file_name, log_mode);
      printf("  \n>>>>>>>>%d Opening file \n\n", pros::millis());
      pros::delay(2);
    }
		uint32_t s_time = pros::millis();
		printf("BEFORE FLUSH %d | T:%d \n", i, pros::millis());
		int flush_amount = fwrite(buf_test, sizeof(buf_test[0]), BUF_TEST_SIZE, log_file);
		close_log_file();
		int dif = pros::millis() - s_time;
		printf("	>> AFTER FLUSH %d | T:%d | Dif: %d\n", i, pros::millis(), dif);
		dif_sum += dif;
	}
	printf("\n\n\n	>>>dif_avg: %f \n\n\n", (double)dif_sum/NUM_TESTS);
}
