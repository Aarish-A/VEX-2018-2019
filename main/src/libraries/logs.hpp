#pragma once
#include "main.h"
#include <stdarg.h>
#include <time.h>
#include <stdio.h>

/* Log Information Structure  - Used by Logging Function */
struct Log_Info {
public:
  std::string name;
  bool enabled;
  Log_Info(std::string name, bool enabled) : name(name), enabled(enabled){};
};
const Log_Info PROGRAM_FLOW("PROGRAM_FLOW", true);
const Log_Info USER("USER", true);
const Log_Info SENSOR("SENSOR", true);
const Log_Info STATE_MACHINE("STATE_MACHINE", true);
const Log_Info SAFETY("SAFETY", true);
const Log_Info ACCESS("ACCESS", true);
const Log_Info HARDWARE("HARDWARE", true);
const Log_Info AUTO_MOVE("AUTO_MOVE", true);
const Log_Info FATAL_ERROR("FATAL_ERROR", true);

/* Loging Constants */
extern const char* log_file_name;
extern const char* const log_mode;

extern FILE* log_file;

extern int log_close_timer;


extern pros::Mutex mutex;

void log_init();
void log(bool system, const char * format, ...);
void log_ln(bool system, const char * format, ...);
