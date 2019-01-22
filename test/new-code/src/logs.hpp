#pragma once
#include "main.h"
#include "config.hpp"
#include <stdarg.h>
#include <time.h>
#include <stdio.h>

extern constexpr bool LOG_DRIVE = 1;
extern constexpr bool LOG_PUNCHER = 1;
extern constexpr bool LOG_ANGLER = 1;
extern constexpr bool LOG_DECAPPER = 1;
extern constexpr bool LOG_JOYSTICK = 1;
extern constexpr bool LOG_ERROR = 1;
extern constexpr bool LOG_AUTO = 1;

const int LOG_CLOSE_TIME = 1000;
const int LOG_MUTEX_TO = 50;

extern const char* log_file_name;
const char* const log_mode = "a";

extern FILE* log_file;

extern int log_close_timer;


extern pros::Mutex mutex;

void log_init();
void log(const char * format, ...);
void log_ln(const char * format, ...);