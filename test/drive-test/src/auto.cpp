#include "auto.hpp"
#include "config.hpp"
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
    double xLst
    double lineAngle = atan2(x - tracking.x, y - tracking.y);
    double d2;
    double da;
    do {
      // Calculate offsets in global coordinates
      double dx = x - tracking.x;
      double dy = y - tracking.y;
      double da = a - tracking.a;
      d2 = dx * dx + dy * dy;

      // Calculate position in line coordinates
      vector linePos = rotate({dx, dy}, -lineAngle);

      // P loop constants
      const double kPx = 2.0;
      const double kPy = 5.0;
      const double kPa = 0 / 90_deg;

      // Calculate target velocities (velLine is still line coordinates)
      vector velLine = {kPx * linePos.x, kPy * linePos.y};
      double velAngle = kPa * da;

      // Convert to robot coordinates
      vector velRobot = rotate(velLine, lineAngle - tracking.a);
      velRobot.x *= 2;

      // Write to motors

      setDrive(velRobot.x, velRobot.y, velAngle);

      printf("%f %f %f | %f %f %f\n", tracking.x, tracking.y, RAD_TO_DEG(tracking.a), velRobot.x, velRobot.y, velAngle);

      pros::delay(10);
    } while (d2 > 1 || da > 1_deg);
    setDrive(0, 0, 0);
  }
}
