#pragma once
#include "main.h"
#include "controls.hpp"

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

  double phase() const;
  double magnitude() const;
};

vector rotate(vector v, double offset);
