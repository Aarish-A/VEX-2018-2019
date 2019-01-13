#pragma once
#include "main.h"
#include <stdio.h>
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
void log_init();

template<typename... Args> void log(const char * f, Args... args) {
  log_file = fopen("/usd/log.txt", "r+");
  printf(f, args...);
  fprintf(log_file, f, args...);
  fclose(log_file);
}
