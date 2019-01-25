#pragma once
#include "main.h"
// #include "config.hpp"
#include <stdarg.h>
#include <time.h>
#include <stdio.h>

constexpr bool LOG_DRIVE = 0;
constexpr bool LOG_PUNCHER = 0;
constexpr bool LOG_ANGLER = 0;
constexpr bool LOG_DECAPPER = 0;
constexpr bool LOG_JOYSTICK = 0;
constexpr bool LOG_ERROR = 0;
constexpr bool LOG_AUTO = 0;
constexpr bool LOG_SHOTS = 0;

constexpr int LOG_CLOSE_TIME = 1000;
constexpr int LOG_MUTEX_TO = 50;

extern const char* log_file_name;
extern const char* const log_mode;

extern FILE* log_file;

extern int log_close_timer;


extern pros::Mutex mutex;

void log_init();
void log(bool system, const char * format, ...);
void log_ln(bool system, const char * format, ...);
