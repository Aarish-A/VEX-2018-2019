#include "main.h"
#include "tracking.hpp"
#include "auto.hpp"
#include "config.hpp"
#include "slew.hpp"
#include <cmath>

namespace pilons::tracking {
  void setDrive(double x, double y, double a) {

    double fL = (y + x + a);
    double bL = (y - x + a);
    double fR = (y - x - a);
    double bR = (y + x - a);

    double max = fmax(fmax(fabs(fL), fabs(fR)), fmax(fabs(bL), fabs(bR)));
    if (max > 200)
    {
      double scale = fabs(200/max);

      fL *= scale;
      bL *= scale;
      fR *= scale;
      bR *= scale;
    }

    //printf("%f %f %f | %f %f %f %f\n", pos.x, pos.y, RAD_TO_DEG(pos.a), fL, bL, fR, bR);
    driveFL.move_velocity(fL);
    driveBL.move_velocity(bL);
    driveFR.move_velocity(fR);
    driveBR.move_velocity(bR);
  }

  void moveToTargetAngle(Tracking &tracking, double x, double y, double a) {
    controller.clear();
    Slew slewX(10, 0, 0);
    Slew slewY(10, 0, 0);
    Slew slewA(10, 0, 0);
    double lineAngle = atan2(x - tracking.x, y - tracking.y);
    double d2;
    double da;
    bool angle_end;
    uint32_t tStart = pros::c::millis();

    do {
      // Calculate offsets in global coordinates
      double dx = x - tracking.x;
      double dy = y - tracking.y;
      double da = a - tracking.a;
      d2 = dx * dx + dy * dy;

      // Calculate position in line coordinates
      vector linePos = rotate({dx, dy}, -lineAngle);

      // P loop constants
      const double kPx = 10.0;
      const double kPy = 10.0;
      const double kPa = 200 / 90_deg;

      // Calculate target velocities (velLine is still line coordinates)
      vector velLine = {kPx * linePos.x, kPy * linePos.y};
      double velAngle = kPa * da;

      // Convert to robot coordinates
      vector velRobot = rotate(velLine, lineAngle - tracking.a);
      velRobot.x *= 2;

      // Write to motors
      double ySlew = slewY.slewSet(velRobot.y);
      double aSlew = slewA.slewSet(velAngle);

      setDrive((velRobot.x), ySlew, aSlew);

      angle_end = fabs(da) > 1_deg;

      printf("%f %f %f | %f (%f: %f) (%f: %f)\n", tracking.x, tracking.y, RAD_TO_DEG(tracking.a), velRobot.x, velRobot.y, ySlew, velAngle, aSlew);
      controller.print(2, 0, "%.1f %.1f %.1f", tracking.x, tracking.y, RAD_TO_DEG(tracking.a));

      pros::delay(10);
    } while (d2 > 1 || angle_end);//d2 > 1 ||
    setDrive(0, 0, 0);
    uint32_t tElpsd = pros::c::millis() - tStart;
    printf("Done Move in %d %f %f %f \n", tElpsd, tracking.x, tracking.y, RAD_TO_DEG(tracking.a));
  }
  FixedAngleTarget::FixedAngleTarget(double target) : target(target) {}

  double FixedAngleTarget::getTarget() {
    return target;
  }

  PointAngleTarget::PointAngleTarget(vector target) : target(target) {}

  double PointAngleTarget::getTarget() {
    return (target - pos.position()).phase();
  }

  void MotionController::taskImpl() {
    Slew slewX(10, 0, 0);
    Slew slewY(10, 0, 0);
    Slew slewA(10, 0, 0);

    printf("MC PTR %x\n", this->angle_target.get());

    while (true) {
      if (!angle_target) {
        pros::delay(10);
        continue;
      }

      // Calculate offsets in global coordinates
      vector dV = end - pos.position();
      double da = angle_target->getTarget() - pos.a;
      double lineAngle = (end - start).phase();

      // Calculate position in line coordinates
      vector linePos = rotate(dV, -lineAngle);

      // P loop constants
      const double kPx = 10.0;
      const double kPy = 10.0;
      const double kPa = 200 / 90_deg;

      // Calculate target velocities (velLine is still line coordinates)
      vector velLine = {kPx * linePos.x, kPy * linePos.y};
      double velAngle = kPa * da;

      double velXRaw = velLine.x;

      //Add cap to x and y velocities
      if (fabs(velLine.y) > 200) velLine.y = 200 * SGN(velLine.y);

      if (fabs(velLine.x) > 200) velLine.x = 200 * SGN(velLine.x);

      // Convert to robot coordinates
      vector velRobot = rotate(velLine, lineAngle - pos.a);
      velRobot.x *= 2;
      //if (fabs(velLine.x) > 5 && fabs(velLine.x) < 25 && fabs(velLine.y) > 180) velLine.x = 25 * SGN(velLine.x);

      //Use slew for y and a velocities + set motors
      double ySlew = slewY.slewSet(velRobot.y);
      double aSlew = slewA.slewSet(velAngle);
      setDrive(velRobot.x, ySlew, aSlew);

      printf("%f %f %f | dD:%f dA:%f | (%f %f %f) (%f: %f) (%f: %f)\n", pos.x, pos.y, RAD_TO_DEG(pos.a), this->dDistance(), this->dAngle(), dV.x, velXRaw, velRobot.x, velRobot.y, ySlew, velAngle, aSlew);
      controller.print(2, 0, "%.1f %.1f %.1f", pos.x, pos.y, RAD_TO_DEG(pos.a));
      pros::delay(10);
    }
  }

  void MotionController::setAngleTarget(AngleTarget *target) {
    angle_target.reset(target);
  }

  void MotionController::setStartToCurrent() {
    setStart(pos.position());
  }

  void MotionController::setStart(vector start) {
    this->start = start;
  }

  void MotionController::setEnd(vector end) {
    this->end = end;
  }

  double MotionController::dDistance() {
    return (this->end - pos.position()).magnitude();
  }

  double MotionController::dAngle() {
    return (this->angle_target->getTarget() - pos.a);
  }
}
