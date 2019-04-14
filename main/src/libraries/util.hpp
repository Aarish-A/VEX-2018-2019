#pragma once
#include "main.h"
#include "../controls.hpp"

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }
template <typename T> T map(T x, T in_min, T in_max, T out_min, T out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

template <typename T> T operator++ (T enum_type, int) { return static_cast<T>(static_cast<int>(enum_type) + 1); }
template <typename T> T operator-- (T enum_type, int) { return static_cast<T>(static_cast<int>(enum_type) - 1); }


template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, void>::type> T next_enum_value(T enum_type) {
  if (static_cast<int>(enum_type) < static_cast<int>(T::NUM_OF_ELEMENTS) - 1) return enum_type++;
  else return static_cast<T>(0);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, void>::type> T previous_enum_value(T enum_type) {
  if (static_cast<int>(enum_type) > static_cast<int>(static_cast<T>(0))) return enum_type--;
  else return static_cast<T>(static_cast<int>(T::NUM_OF_ELEMENTS) - 1);
}

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
