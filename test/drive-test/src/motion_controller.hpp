/*
#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include <memory>
#include "tracking.hpp"
#include "util.hpp"

namespace pilons::tracking {
  void setDrive(double x, double y, double a);
  void setDriveY(double y);
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

  class PointAngleTarget final : public AngleTarget {
    vector target;

  public:
    PointAngleTarget(vector target);
    double getTarget() override;
  };

  class MotionController final : public util::BackgroundTask {
    std::unique_ptr<AngleTarget> angle_target;
    vector start;
    vector end;

  protected:
    void taskImpl() override;

  public:
    void setAngleTarget(AngleTarget *target);
    void setStartToCurrent();
    void setStart(vector start);
    void setEnd(vector end);
    void setEndAndAngle(vector end);

    void setFull(vector start, vector end, AngleTarget* target = nullptr);

    double dDistance();
    double dAngle();
  };
}

#endif
*/
