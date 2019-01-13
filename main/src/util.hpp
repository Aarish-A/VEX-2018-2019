#pragma once
#include "main.h"
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

#define LOG(input) \
log_file = fopen("/usd/log.txt", "r+"); \
fprintf(log_file, input); \
printf(input); \
fclose(log_file)
