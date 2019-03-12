#include "logs.hpp"
bool sd_logging_enabled = false;
pros::Mutex sd_buffer_mutex;
char log_buffer[LOG_BUFFER_SIZE];
int buffer_write_index = 0;
int buffer_flush_index = 0;

FILE* log_file = NULL;

const char* log_file_name = "/usd/log.txt";
const char* const log_mode = "a";

/* Open and Close Log File */
/* Open and Close Log File */
void open_log_file() {
  int open_attempt_count = 0;
  while (log_file == NULL && open_attempt_count < MAX_ALLOWABLE_OPEN_ATTEMPTS) {
    log_file = fopen(log_file_name, log_mode);
		if (log_file == NULL) {
			printf(" ERR %d log_file fopen failed | errno: %d %s | %p\n", pros::millis(), errno, strerror(errno), log_file);
			pros::delay(5);
		} else printf("\n       >>> %d OPEN LOG_FILE: | errno: %d %s | %p\n", pros::millis(), errno, strerror(errno), log_file);
  }
  pros::delay(FILE_SLEEP);
}

void close_log_file() {
  if (log_file != NULL) {
    int f_close_ret = -1;
    f_close_ret = fclose(log_file);
    if (f_close_ret != 0) {
      printf("  %d ERR log_file fclose failed | errno: %d %s | f_close_ret: %d | %p\n", pros::millis(), errno, strerror(errno), f_close_ret, log_file);
    }
    else printf("  %d CLOSE LOG FILE | errno: %d %s | f_close_ret: %d | %p\n", pros::millis(), errno, strerror(errno), f_close_ret, log_file);
    log_file = NULL;
    pros::delay(FILE_SLEEP);
  }
}

/* Logging Functions */
void log_init() {
  printf(" %d log_init(): Start Logging for Program \n", pros::millis());
  open_log_file(); // Open log_file
	if (log_file == NULL) { // If the log_file could not be oppend
    sd_logging_enabled = false;
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE \n", pros::millis());
	}
  else { // If the log_file could be oppened
    sd_logging_enabled = true;
    printf(">>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
  	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
    fprintf(log_file, ">>>> %d Start Logging for Program \n", pros::millis());
    close_log_file();
  }
  pros::delay(10);
  if (sd_logging_enabled) { // Start buffer -> sd task
    log_ln(PROGRAM_FLOW, " Construct buffer_to_sd() task from log_init()");
    pros::Task buffer_to_sd_task((pros::task_fn_t) buffer_to_sd);
  }
  pros::delay(50); // Give the task time to start up (it is not started instantly)
  log_ln(PROGRAM_FLOW, "DONE LOG_INIT");
}

void log_ln_internal(const char * str_whole) {
  // 1) Write to console
  printf("%s",str_whole);

  // 2) Write to sd
  if (sd_logging_enabled)
  {
    size_t str_whole_len = strlen(str_whole); // Amount of chars in input-string
    int write_amount = 0;
    if (sd_buffer_mutex.take(LOG_MUTEX_TO))
    {
      size_t buf_remaining_len = LOG_BUFFER_SIZE - buffer_write_index; // Amount of unfilled indices left in buffer
      if (buf_remaining_len < str_whole_len) { // If the input-string is about to go out of the bounds of the buffer
                                                // Split the input-string in two. Put whatever fits into the end of the buffer
                                                // and put the remaining characters into the front of the buffer
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
        printf("  >>%d log_ln() | write_amount = %d | %d %d \n", pros::millis(), write_amount, buffer_write_index, buffer_flush_index);
        // 2) Write second part of string to front of buffer
        write_amount = sprintf(log_buffer, "%s", str_sec.c_str()); // HOW WILL ARGS BE HANDLED? WILL IT USE THE RIGHT ARGS B/W THE TWO FUNC CALLS?
        if (write_amount > 0) buffer_write_index = write_amount;
        printf("  >>%d log_ln() | write_amount = %d | %d %d \n", pros::millis(), write_amount, buffer_write_index, buffer_flush_index);
      }
      else {
        write_amount = sprintf(log_buffer+BUF_OBJ_SIZE*buffer_write_index, "%s", str_whole);
        if (write_amount > 0) buffer_write_index += write_amount;
      }
      sd_buffer_mutex.give();
      //printf("        >>%d LOG_LN() GAVE sd_buffer_mutex \n", pros::millis());
    } else printf("\n   ERR>>> %d log_ln_internal() sd_buffer_mutex take failed (TO = 50ms) | Err:%d \n\n", pros::millis(), errno);
  }
}
void log_ln(Log_Info info_category, const char * format, ...) {
  va_list args;
  va_start(args, format);
  //printf("\n\n\n%d START PRINT \n\n", pros::millis());
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
  //printf("\n\n\n%d DONE PRINT \n\n", pros::millis());
  va_end (args);
}

/* Buffer -> SD Functions */
void flush_to_sd(int given_amnt_to_flush, int pos_to_flush_to) {
  int amnt_to_flush = std::min(given_amnt_to_flush, MAX_AMNT_TO_FLUSH); // Lim num of indices to flush to MAX_AMNT_TO_FLUSH

  // 1) Open Log File
  open_log_file();
  printf("\n         >>> %d F_TO_SD() OPEN: | errno: %d %s | %p | %d %d | F: %d->%d\n", pros::millis(), errno, strerror(errno), log_file, buffer_flush_index, pos_to_flush_to, given_amnt_to_flush, amnt_to_flush);

  if (log_file != NULL) { // Only perform flush_to_sd opperations if file is oppened
    // 2) Flush buffer into the SD file
    const char* FLUSH_START_POS = &log_buffer[buffer_flush_index];
    int flush_amount = fwrite(FLUSH_START_POS, BUF_OBJ_SIZE, amnt_to_flush, log_file);
    int err = ferror(log_file);

    // 2a) Print error messages upon failure
    if (err) printf("  %d ERR log_write <1> failed | errno: %d | ferror: %d | flush_amount: %d | %p \n", pros::millis(), errno, err, flush_amount, log_file);
    if (flush_amount <= 0) printf("  %d ERR log_write <2> failed | errno: %d | ferror: %d | flush_amount: %d | %p \n", pros::millis(), errno, err, flush_amount, log_file);
    // 2b) Set buffer_flush_index upon success
    else buffer_flush_index += flush_amount; // Incrememnt blush_flush_index upon successful write
    if (buffer_flush_index >= LOG_BUFFER_SIZE) buffer_flush_index = 0; // If the pointer is at the end of the buffer, move it back to the front
    pros::delay(FILE_SLEEP);
    printf("\n         >>> %d F_TO_SD() FLUSH: | errno: %d %s | %p | %d %d | F: %d->%d\n", pros::millis(), errno, strerror(errno), log_file, buffer_flush_index, pos_to_flush_to, given_amnt_to_flush, amnt_to_flush);

    // 3) Close log_file
    close_log_file();
    printf("\n         >>> %d F_TO_SD() CLOSE: | errno: %d %s | %p | %d %d | F: %d->%d\n", pros::millis(), errno, strerror(errno), log_file, buffer_flush_index, pos_to_flush_to, given_amnt_to_flush, amnt_to_flush);
  }
}

void buffer_to_sd() {
  printf("  %d PRINT - buffer start \n", pros::millis());
  log_ln(PROGRAM_FLOW, "%d Start buffer_to_sd() task \n", pros::millis());

  while (true) {
    int pos_to_flush_to = buffer_write_index; // Take a copy of buffer_write_index (so we task not be affected if log_ln modifies it)

    if (pos_to_flush_to > buffer_flush_index) { // The last character flushed from buf is behind last character written to it
      int amnt_to_flush = pos_to_flush_to-buffer_flush_index;
      flush_to_sd(amnt_to_flush, pos_to_flush_to); // Flush everything b/w buffer_flush_index -> pos_to_flush_to
    }
    else if (pos_to_flush_to < buffer_flush_index) { // We wrapped around when writing to the buffer (meaning the last character written is behind the last character flushed)
      printf("  >>%d buffer_to_sd() BUFFER->SD wrapped around \n", pros::millis());
      int buf_remaining_len = LOG_BUFFER_SIZE - buffer_flush_index; // Amount of unflushed indices left in buffer
      flush_to_sd(buf_remaining_len, pos_to_flush_to); // Flush b/w buffer_flush_index -> buffer_end. Move the pointer back to the beginning of the buffer
    }
    pros::delay(LOG_BUFFER_FLUSH_DELAY);
  }
}

/* Test Functions */
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
