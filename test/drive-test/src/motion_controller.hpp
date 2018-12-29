#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include <memory>
#include "tracking.hpp"
#include "util.hpp"
#include "angle_target.hpp"

namespace pilons::tracking {
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
