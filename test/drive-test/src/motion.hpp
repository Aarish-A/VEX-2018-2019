#ifndef MOTION_H
#define MOTION_H

#include <memory>
#include "tracking.hpp"
#include "util.hpp"
#include "angle_target.hpp"

namespace pilons::tracking {
  void setDrive(double x, double y, double a);
  void setDriveVoltage(double x, double y, double a);
  void setDriveYVoltage(double y);
  void setDriveY(double y);
  void moveToTarget(vector target, bool adjust = false, double offset = 0);
  void moveToTargetSimple(vector target, double speed, double breakOffset);

  void turnVel(AngleTarget *target, double offset);
  void turn(AngleTarget *target, double offset = 0, bool fast = true, bool brake = true);
}

#endif
