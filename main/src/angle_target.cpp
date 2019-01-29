#include "angle_target.hpp"

FixedAngleTarget::FixedAngleTarget(double target) : target(target) {}

double FixedAngleTarget::getTarget() const {
  return target;
}

PointAngleTarget::PointAngleTarget(vector target) : target(target) {}

double PointAngleTarget::getTarget() const {
  vector t = target;
  return (t - pos.position()).phase();
}
