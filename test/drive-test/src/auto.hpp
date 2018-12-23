#ifndef AUTO_H
#define AUTO_H
#include "tracking.hpp"

namespace pilons::tracking {
  struct vector {
    double x, y;
  };

  vector rotate(vector v, double offset);
  void setDrive(double x, double y, double a);
  void moveToTargetAngle(Tracking &tracking, double x, double y, double a);
}
#endif
