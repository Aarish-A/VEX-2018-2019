#include "main.h"
#include "angle_target.hpp"
#include "tracking.hpp"
#include "config.hpp"
#include <cmath>


  FixedAngleTarget::FixedAngleTarget(double target) : target(target) {}

  double FixedAngleTarget::getTarget() {
    return target;
  }

  PointAngleTarget::PointAngleTarget(vector target) : target(target) {}

  double PointAngleTarget::getTarget() {
    return (target - pos.position()).phase();
  }
