#pragma once
#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

int set_dz(int val, int dz);
int set_scaled_dz(int val, int dz);

/* Vectors */
struct vector {
  double x, y;

  vector operator+(vector other);
  vector operator-(vector other);
  vector operator+();
  vector operator-();

  double phase();
  double magnitude();
};

vector rotate(vector v, double offset);

/* Logging */
extern FILE* log_file;
void log_init(); //Call in initialize before calling log();

void log(const char * format, ...);
void log_ln(const char * format, ...);

extern int log_close_timer;
const int LOG_CLOSE_TIME = 1000;
const char* const log_file_name = "/usd/log.txt";
const char* const log_mode = "a";

extern pros::Mutex mutex;
const int LOG_MUTEX_TO = 50;

/* template<typename... Args> void log(const char * f, Args... args) {
  printf(f, args...);
  if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
		return;
	}
  fprintf(log_file, f, args...);
  fflush(log_file);
} */
