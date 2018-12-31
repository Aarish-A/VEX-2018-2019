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
  void setDriveVoltage(double x, double y, double a) {

    double fL = (y + x + a);
    double bL = (y - x + a);
    double fR = (y - x - a);
    double bR = (y + x - a);

    double max = fmax(fmax(fabs(fL), fabs(fR)), fmax(fabs(bL), fabs(bR)));
    if (max > 127)
    {
      double scale = fabs(127/max);

      fL *= scale;
      bL *= scale;
      fR *= scale;
      bR *= scale;
    }
    //printf("%f %f %f | %f %f %f %f\n", pos.x, pos.y, RAD_TO_DEG(pos.a), fL, bL, fR, bR);
    driveFL.move(fL);
    driveBL.move(bL);
    driveFR.move(fR);
    driveBR.move(bR);
  }
  void setDriveYVoltage(double y) {
    driveFL.move(y);
    driveBL.move(y);
    driveFR.move(y);
    driveBR.move(y);
  }
  void setDriveY(double y) {
    driveFL.move_velocity(y);
    driveBL.move_velocity(y);
    driveFR.move_velocity(y);
    driveBR.move_velocity(y);
  }
  void moveToTarget(vector target, bool adjust, double offset)
  {
    Slew slewX(10, 10, 0);
    Slew slewY(2, 2, 0);
    Slew slewA(10, 10, 0);

    // P loop constants
    const double kPx = 0;//15.0;
    const double kDx = 0.0;
    const double kPy = 5.0;
    const double kPa = 200 / 90_deg;

    vector dV = target - pos.position();
    vector velLine = {kPx * dV.x, kPy * dV.y};
    vector velRobot = rotate(velLine, -pos.a);

    double velYStart = velRobot.y;

    setDriveY(dV.y > 0? 1 : -1);
    printf("\n %d Move To (%lf, %lf) from (%lf, %lf, %lf). dVYStart: %lf \n", pros::millis(), target.x, target.y, pos.x, pos.y, RAD_TO_DEG(pos.a), velYStart);

    do
    {
      double aTarg = (target - pos.position()).phase() + offset;

      // Calculate offsets in global coordinates
      dV = target - pos.position();
      double dA = aTarg - pos.a;

      // Calculate target velocities (velLine is still line coordinates)
      velLine = {kPx * dV.x, kPy * dV.y};
      double velAngle = kPa * dA;

      //Add cap to x and y velocities
      if (fabs(velLine.y) > 150) velLine.y = 150 * SGN(velLine.y);
      if (fabs(velLine.x) > 150) velLine.x = 150 * SGN(velLine.x);

      // Convert to robot coordinates
      velRobot = rotate(velLine, -pos.a);
      if (fabs(velRobot.y) < 30) velRobot.y  = 30 * SGN(velLine.y);
      velRobot.x *= 2;
      //if (fabs(velLine.x) > 5 && fabs(velLine.x) < 25 && fabs(velLine.y) > 180) velLine.x = 25 * SGN(velLine.x);

      //Use slew for y and a velocities + set motors
      double xSlew = slewX.slewSet(velRobot.x);
      double ySlew = slewY.slewSet(velRobot.y);
      double aSlew = adjust? slewA.slewSet(velAngle) : 0;
      //setDrive(0, ySlew, 0);//aSlew);
      setDrive(0, ySlew, aSlew);

      double avgMtrVel = (driveBL.get_actual_velocity() + driveBR.get_actual_velocity() + driveFL.get_actual_velocity() + driveFR.get_actual_velocity()) / 4.0;
      printf("%d %d:%lf, %d:%lf, %d:%lf, %d:%lf  \n", pros::millis(), driveFL.get_target_velocity(), driveFL.get_actual_velocity(), driveFR.get_target_velocity(), driveFR.get_actual_velocity(), driveBL.get_target_velocity(), driveBL.get_actual_velocity(), driveBR.get_target_velocity(), driveBR.get_actual_velocity());

      //printf("%d %f %f %f | dD:%f dA:%f | (%f: %f) (%f: %f: %f : %f) (%f: %f)\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), dV.magnitude(), dA, velRobot.x, xSlew, velLine.y, velRobot.y, ySlew, avgMtrVel, velAngle, aSlew);
      //controller.print(2, 0, "%.1f %.1f %.1f", pos.x, pos.y, RAD_TO_DEG(pos.a));
      pros::delay(8);
    } while (fabs(dV.y) > 1.9);///while(dV.magnitude() > 1);
    setDrive(0, 0, 0);
    printf("%d Done Move %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));

  }

  void moveToTargetSimple(vector target, double speed, double breakOffset)
  {
    printf("\n %d Move To (%lf, %lf) from (%lf, %lf, %lf) \n", pros::millis(), target.x, target.y, pos.x, pos.y, pos.a);
    vector dV = target - pos.position();
    setDriveY(speed);
    do {
      dV = target - pos.position();
      pros::delay(10);
    } while (fabs(dV.y) > fabs(breakOffset));
    setDriveY(0);
    printf("%d Done Move To (%lf, %lf) from (%lf, %lf, %lf) \n", pros::millis(), target.x, target.y, pos.x, pos.y, pos.a);
  }

  void turnVel(AngleTarget *target, double offset)
  {
    Slew slewA(2, 10, 0);

    const double kPA = (200 / 90_deg);
    double aTarg = target->getTarget();
    double dA = target->getTarget() - pos.a + offset;
    printf("%d Turn %lf \n", pros::millis(), RAD_TO_DEG(dA));

    do {
      dA = target->getTarget() - pos.a + offset;
      double power = dA * kPA;
    //  if (fabs(power) < 30) power = 30 * SGN(power);

      double aSlew = slewA.slewSet(power);

      setDrive(0, 0, aSlew);

      printf("%d %f %f %f  velA:%f| dA:%f | (%f, %f) | %f, %f, %f, %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel, RAD_TO_DEG(dA), power, aSlew, driveFL.get_actual_velocity(), driveFR.get_actual_velocity(), driveBL.get_actual_velocity(), driveBR.get_actual_velocity());

      pros::delay(10);
    } while (fabs(dA) > 0.7_deg);
    setDrive(0, 0, 0);
    printf("%d Done Turn %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
    /*
    pros::delay(1000);
    while(true)
    {
      printf("%d Done Turn Sleep %f %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel);
      pros::delay(100);
    }
    */
  }

  void turn(AngleTarget *target, double offset, bool fast, bool brake)
  {
    Slew slewA(2, 10, 0);

    const double kPA = (127 / 90_deg);
    double aTarg = target->getTarget();
    double dA = target->getTarget() - pos.a + offset;
    printf("%d Turn to %f from %f| DeltaA:%f offset:%f \n", pros::millis(), RAD_TO_DEG(aTarg), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), offset);

    do {
      dA = target->getTarget() - pos.a + offset;
      double power = dA * kPA;

      //Set min lim accordingly
      if (fast)  {
         if (fabs(power) < 30) power = 30 * SGN(power);
       }
      else {
        if (fabs(power) < 15) power = 15 * SGN(power);
      }

      double aSlew = slewA.slewSet(power);

      setDriveVoltage(0, 0, power); //aSlew);

     printf("%d %f %f %f  velA:%f| dA:%f | (%f, %f) | %f, %f, %f, %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel, RAD_TO_DEG(dA), power, aSlew, driveFL.get_actual_velocity(), driveFR.get_actual_velocity(), driveBL.get_actual_velocity(), driveBR.get_actual_velocity());

      pros::delay(10);
    } while (fabs(dA) > (fast? 5.65_deg : 0.5_deg));//5.65_deg);
    if (brake)
    {
      printf("%d Turn Break", pros::millis());
      setDriveVoltage(0, 0, 0);
      driveFL.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
			driveFR.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
			driveBL.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
			driveBR.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    }
    else
    {
      setDriveVoltage(0, 0, 0);
    }
    pros::delay(100);
    printf("%d Done Turn %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
    /*
    pros::delay(1000);
    while(true)
    {
      printf("%d Done Turn Sleep %f %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel);
      pros::delay(100);
    }
    */
  }

}
