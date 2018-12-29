#include "main.h"
#include "tracking.hpp"
#include "motion.hpp"
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
  void setDriveY(double y) {
    driveFL.move_velocity(y);
    driveBL.move_velocity(y);
    driveFR.move_velocity(y);
    driveBR.move_velocity(y);
  }

  void moveToTarget(vector target, bool adjust)
  {
    Slew slewX(10, 10, 0);
    Slew slewY(1, 2, 0);
    Slew slewA(10, 10, 0);

    vector dV = target - pos.position();

    do
    {
      double aTarg = (target - pos.position()).phase();

      // Calculate offsets in global coordinates
      dV = target - pos.position();
      double dA = aTarg - pos.a;

      // P loop constants
      const double kPx = 0;//15.0;
      const double kDx = 0.0;
      const double kPy = 5.0;
      const double kPa = 200 / 90_deg;

      // Calculate target velocities (velLine is still line coordinates)
      vector velLine = {kPx * dV.x, kPy * dV.y};
      double velAngle = kPa * dA;

      //Add cap to x and y velocities
      if (fabs(velLine.y) > 200) velLine.y = 200 * SGN(velLine.y);
      if (fabs(velLine.x) > 200) velLine.x = 200 * SGN(velLine.x);

      // Convert to robot coordinates
      vector velRobot = rotate(velLine, -pos.a);
      velRobot.x *= 2;
      //if (fabs(velLine.x) > 5 && fabs(velLine.x) < 25 && fabs(velLine.y) > 180) velLine.x = 25 * SGN(velLine.x);

      //Use slew for y and a velocities + set motors
      double xSlew = slewX.slewSet(velRobot.x);
      double ySlew = slewY.slewSet(velRobot.y);
      double aSlew = slewA.slewSet(velAngle);
      setDrive(0, ySlew, adjust? aSlew : 0);

      double avgMtrVel = (driveBL.get_actual_velocity() + driveBR.get_actual_velocity() + driveFL.get_actual_velocity() + driveFR.get_actual_velocity()) / 4.0;

      printf("%d %f %f %f | dD:%f dA:%f | (%f: %f) (%f: %f : %f) (%f: %f)\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), dV.magnitude(), dA, velRobot.x, xSlew, velRobot.y, ySlew, avgMtrVel, velAngle, aSlew);
      //controller.print(2, 0, "%.1f %.1f %.1f", pos.x, pos.y, RAD_TO_DEG(pos.a));
      pros::delay(8);
    } while(dV.magnitude() > 1);
    setDrive(0, 0, 0);
    printf("%d Done Move %f %f %f", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));

  }

  void turn(AngleTarget *target)
  {
    const double kPA = (200 / 90_deg);
    double aTarg = target->getTarget();
    double dA = aTarg - pos.a;
    printf("%d Turn %lf \n", pros::millis(), RAD_TO_DEG(dA));

    do {
      dA = aTarg - pos.a;
      double power = dA * kPA;

    //  if (fabs(power) < 30 && fabs(dA) > 3_deg) power = 30 * SGN(power);

      setDrive(0, 0, power);

      printf("%d %f %f %f  velA:%f| dA:%f | (%f)\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel, RAD_TO_DEG(dA), power);

      pros::delay(10);
    } while (fabs(dA) > 0.7_deg);
    setDrive(0, 0, 0);
    printf("%d Done Turn %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
    pros::delay(1000);
    while(true)
    {
      printf("%d Done Turn Sleep %f %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel);
      pros::delay(100);
    }
  }

}
