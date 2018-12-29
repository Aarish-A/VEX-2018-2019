#ifndef ANGLE_TARGET_H
#define ANGLE_TARGET_H

#include <memory>
#include "tracking.hpp"
#include "util.hpp"

namespace pilons::tracking {
  class AngleTarget {
  public:
    virtual double getTarget() = 0;
  };

  class FixedAngleTarget final : public AngleTarget {
    double target;

  public:
    FixedAngleTarget(double target);

    double getTarget() override;
  };

  class PointAngleTarget final : public AngleTarget {
    vector target;

  public:
    PointAngleTarget(vector target);
    double getTarget() override;
  };
}

#endif
