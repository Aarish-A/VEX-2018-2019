#include "main.h"
#include "tracking.hpp"
#include "auto.hpp"
#include "config.hpp"
#include "slew.hpp"
#include <cmath>

namespace pilons::tracking {
  vector rotate(vector v, double offset) {
    double m = sqrt(v.x * v.x + v.y * v.y);
    double a = atan2(v.x, v.y) + offset;
    return {m * sin(a), m * cos(a)};
  }

  void setDrive(double x, double y, double a) {

    driveFL.move_velocity(y + x + a);
    driveBL.move_velocity(y - x + a);
    driveFR.move_velocity(y - x - a);
    driveBR.move_velocity(y + x - a);
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
}
