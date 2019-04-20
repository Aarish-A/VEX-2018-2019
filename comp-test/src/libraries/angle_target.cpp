#include "angle_target.hpp"

FixedAngleTarget::FixedAngleTarget(double target) : target(target) {}

double FixedAngleTarget::getTarget() const {
  return target;
}

PointAngleTarget::PointAngleTarget(vector target) : target(target) {}

double PointAngleTarget::getTarget() const {
  vector t = target;
  //printf("target is: %f\n", RAD_TO_DEG(((t - pos.position()).phase())));
  //printf("pos: x: %f, y: %f, a: %f\n", pos.x, pos.y, RAD_TO_DEG(pos.a));
  return (t - pos.position()).phase();
}
