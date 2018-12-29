#ifndef MOTION_H
#define MOTION_H

#include <memory>
#include "tracking.hpp"
#include "util.hpp"
#include "angle_target.hpp"

namespace pilons::tracking {
  void setDrive(double x, double y, double a);
  void setDriveY(double y);
  void moveToTarget(vector target, bool adjust = true);

  void turn(AngleTarget *target);
}

#endif
