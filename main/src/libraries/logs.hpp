#pragma once
#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <cstring>

/* Log Information Structure  - Used by Logging Function */
struct Log_Info {
public:
  std::string name;
  bool enabled;
  Log_Info(std::string name, bool enabled) : name(name), enabled(enabled){};
};
/* Log_Info Categories */
const Log_Info PROGRAM_FLOW("PROGRAM_FLOW", true);
const Log_Info USER("USER", true);
const Log_Info SAFETY("SAFETY", true);
const Log_Info ACCESS("ACCESS", true);
const Log_Info HARDWARE("HARDWARE", true);
const Log_Info FATAL_ERROR("FATAL_ERROR", true);
/* Log_Info Subsystems */
const Log_Info ANGLER("ANGLER", true);
const Log_Info DRIVE("USER", true);
const Log_Info PUNCHER("PUN", true);
const Log_Info DECAPPER("DECAPPER", true);
const Log_Info AUTO("USER", true);

/* Loging Constants */
extern bool sd_logging_enabled; // Boolean that is set during init(). If false, log functions will only log to the console and not the SD
extern const char* log_file_name; // Name of file on SD card that is printed to
extern const char* const log_mode; // Logging mode that is used
extern FILE* log_file; // File on SD card that we print to
extern pros::Mutex sd_buffer_mutex; // sd_buffer_mutex used to ensure only one piece of code touches the buffer at a time
constexpr int LOG_BUFFER_FLUSH_DELAY = 500; // Amount of time between consecutive flushes of the buffer into the SD
constexpr int LOG_MUTEX_TO = 5; // Max amnt of time that log_ln_internal() will wait for sd_buffer_mutex
constexpr int MAX_ALLOWABLE_OPEN_ATTEMPTS = 100; // Max num of attempts that log_init() will make when trying to open the file
constexpr int MAX_ALLOWABLE_CLOSE_ATTEMPTS = 100; // Max num of attempts that close_log_file will make when trying to close the file
constexpr int FILE_SLEEP = 10; // Time (in ms) for sleeps b/w opperations in flush_to_sd() function

/* Logging Buffer */
constexpr int LOG_BUFFER_SIZE = 10000; // Max size of log_buffer
extern char log_buffer[LOG_BUFFER_SIZE]; // If sd_logging_enabled, log functions log to this buffer. Then, buffer_to_sd() task flushes buffer into SD
extern int buffer_write_index; // Last index in buffer that was written to + 1 ; Writes to the buffer start from this index
extern int buffer_flush_index; // Last index in buffer that was flushed onto the SD card ; buffer_to_sd() flushes buffer from this index to buffer_write_index
constexpr size_t BUF_OBJ_SIZE = sizeof(log_buffer[0]); // Size of single element in the buffer

constexpr int MAX_AMNT_TO_FLUSH = LOG_BUFFER_SIZE; // Maximum amount of characters allowed to be flushed to the SD at once. Flushing 1000 takes approx. 13ms

/* Log_File open/close Functions */
void open_log_file(); // Open log_file and sets log_file ptr. Will attempt to open file MAX_ALLOWABLE_OPEN_ATTEMPTS times
void close_log_file(); // Closes log_file and sets log_file to NULL. Will attempt to close file MAX_ALLOWABLE_CLOSE_ATTEMPTS times

/* Logging Functions */
void log_init(); // Initializes sd logging; Will disabling sd_logging if it cannot open the log_file
void log_ln_internal(const char * str_whole); // Used by log_ln function overloads to log to console and log_buffer (for sd)
void log_ln(Log_Info info_category, const char * format, ...); // Logs single line to SD and console (if info_category.enabled)
void log_ln(Log_Info info_category, Log_Info info_subsystem, const char * format, ...);  // Logs single line to SD and console (if info_category.enabled AND info_subsystem.enabled)

/* Buffer -> SD Functions */
void flush_to_sd(int given_amnt_to_flush, int pos_to_flush_to); // Flushes given_amnt_to_flush of indices (capped at MAX_AMNT_TO_FLUSH), to the sd card
void buffer_to_sd(); // Push new data from the buffer into the SD. Is started as a task in log_init()

/* Test Functions */
void _test_flush_time(); // Call to test how long it takes to copy an array of a given size.
                          // Disable buffer_to_sd() when calling this. To modify desired test array size, go to the func. definition
