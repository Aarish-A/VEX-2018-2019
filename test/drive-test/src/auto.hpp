#ifndef AUTO_H
#define AUTO_H

#include <memory>
#include "tracking.hpp"
#include "util.hpp"

namespace pilons::tracking {
  struct vector {
    double x, y;
  };

  vector rotate(vector v, double offset);
  void setDrive(double x, double y, double a);
  void moveToTargetAngle(Tracking &tracking, double x, double y, double a);

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

  class MotionController final : public util::BackgroundTask {
    std::unique_ptr<AngleTarget> angle_target;
    vector start;
    vector end;
    std::unique_ptr<pros::Task> task;

  public:
    void setAngleTarget(AngleTarget *target);
    void setStart(vector start);
    void setEnd(vector end);
  };
}

#endif
