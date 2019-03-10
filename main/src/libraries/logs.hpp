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
extern const char* log_file_name;
extern const char* const log_mode;
extern FILE* log_file;
extern pros::Mutex mutex;
constexpr int LOG_BUFFER_FLUSH_DELAY = 1000;
constexpr int LOG_MUTEX_TO = 50;

/* Logging Buffer */
constexpr int LOG_BUFFER_SIZE = 10000; // DO NOT ADD MORE 0S - WILL NOT COMPILE
extern char log_buffer[LOG_BUFFER_SIZE];
extern int buffer_write_index;
extern int buffer_flush_index;
extern int buffer_head;
extern int buffer_tail;

/* Logging Functions */
void log_init(); // Call in initialize to initialize log
void _log_ln_internal(const char * format, va_list args); // Used by log_ln function overloads to log
void buffer_to_sd(); // Push new data from the buffer into the sd

void log_ln(Log_Info info_category, const char * format, ...); // Logs single line to SD and console (if info_category.enabled)
void log_ln(Log_Info info_category, Log_Info info_subsystem, const char * format, ...);  // Logs single line to SD and console (if info_category.enabled AND info_subsystem.enabled)
