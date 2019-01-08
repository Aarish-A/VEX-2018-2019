#include "util.hpp"

int set_dz(int val, int dz) {
  dz = abs(dz);
  return (abs(val) < dz ? 0 : val);
}

int set_scaled_dz(int val, int dz) {
  dz = abs(dz);
  val = set_dz(val, dz);
  if (val != 0) {
    val = (127/(127-dz)) * (abs(val)-dz) * sgn(val);
  }

  return val;
}

/* Vectors */

vector vector::operator+(vector other) {
  return {x + other.x, y + other.y};
}

vector vector::operator-(vector other) {
  return *this + -other;
}

vector vector::operator+() {
  return *this;
}

vector vector::operator-() {
  return {-x, -y};
}

double vector::phase() {
  return atan2(x, y);
}

double vector::magnitude() {
  return sqrt(x * x + y * y);
}

vector rotate(vector v, double offset) {
  double m = v.magnitude();
  double a = v.phase() + offset;
  return {m * sin(a), m * cos(a)};
}
