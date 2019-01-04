#include "auto.hpp"
#include "config.hpp"

using namespace pros;

double getGlobalAngle() {
	return (DRIVE_DIA * M_PI * (driveFL.get_position() - driveBR.get_position() + driveBL.get_position() - driveFR.get_position())) / (2 * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH));
}

void setDrive(int x, int y, int a) {
	driveFL.move(y + x + a);
	driveBL.move(y - x + a);
	driveFR.move(y - x - a);
	driveBR.move(y + x - a);
}

void setDrive(int pow) {
  setDrive(pow, pow, pow);
}

void setDriveVel(int x, int y, int a) {
	driveFL.move_velocity(y + x + a);
	driveBL.move_velocity(y - x + a);
	driveFR.move_velocity(y - x - a);
	driveBR.move_velocity(y + x - a);
}

void setDriveVel(int vel) {
  setDriveVel(vel, vel, vel);
}

void moveDrive(double dis, int vel, bool stop) {
  double targetFL = driveFL.get_position() + dis;
  double targetBL = driveBL.get_position() + dis;
  double targetFR = driveFR.get_position() + dis;
  double targetBR = driveBR.get_position() + dis;

  int encLStart = encL.get_value();
  int encRStart = encR.get_value();

  printf("%d Moving to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), targetFL, targetBL, targetFR, targetBR);

  uint32_t lastAccel = 0, lastDecel = 0;
  int i = 10;
  while ((targetFL - driveFL.get_position()) * sgn(dis) > 0.5_in || (targetBL - driveBL.get_position()) * sgn(dis) > 0.5_in || (targetFR - driveFR.get_position()) * sgn(dis) > 0.5_in || (targetBR - driveBR.get_position()) * sgn(dis) > 0.5_in) {
    uint32_t curTime = millis();
    double actVel = ((driveFL.get_actual_velocity() + driveBL.get_actual_velocity() + driveFR.get_actual_velocity() + driveBR.get_actual_velocity()) / 4.0) / 60.0 * 360.0;
    double tkToDecel = (actVel * actVel) / (2 * ((1000.0 / MOVE_DECEL_RATE) / 60.0) * 360.0);
    //printf("%d %f %f %f\n", i, actVel, targetFL - driveFL.get_position(), tkToDecel);
    if (tkToDecel < (targetFL - driveFL.get_position()) * sgn(dis) - 0.5_in) {
      if (curTime - lastAccel > MOVE_ACCEL_RATE && i < vel) {
        i += 1;
        lastAccel = curTime;
      }
    }
    else {
      if (curTime - lastDecel > MOVE_DECEL_RATE && i > 10) {
        i -= 1;
        lastDecel = curTime;
      }
    }
    setDriveVel(0, i * sgn(dis), 0);
    delay(1);
  }
	if (stop) {
	  driveFL.move_absolute(targetFL, 25);
	  driveBL.move_absolute(targetBL, 25);
	  driveFR.move_absolute(targetFR, 25);
	  driveBR.move_absolute(targetBR, 25);
	  printf("%d Stopping from FL: %f, BL: %f, FR: %f, BR %f\n", millis(), driveFL.get_position(), driveBL.get_position(), driveFR.get_position(), driveBR.get_position());
	  while (fabs(driveFL.get_position() - targetFL) > 3 || fabs(driveBL.get_position() - targetBL) > 3 || fabs(driveFR.get_position() - targetFR) > 3 || fabs(driveBR.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	}
  printf("%d Moved to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), driveFL.get_position(), driveBL.get_position(), driveFR.get_position(), driveBR.get_position());
  //printf("%f %f\n", (encL.get_value() - encLStart) / 360.0 * 2.75 * M_PI, (encR.get_value() - encRStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}

void turnDrive(double dis, int vel) {
	double q = (dis * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH)) / (2 * M_PI * DRIVE_DIA);
	//printf("%f\n", q);

	double targetFL = driveFL.get_position() + q;
  double targetBL = driveBL.get_position() + q;
  double targetFR = driveFR.get_position() - q;
  double targetBR = driveBR.get_position() - q;

  int encLStart = encL.get_value();
  int encRStart = encR.get_value();

  printf("%d Turning to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), targetFL, targetBL, targetFR, targetBR);

  uint32_t lastAccel = 0, lastDecel = 0;
  int i = 10;
  while ((targetFL - driveFL.get_position()) * sgn(dis) > 20 || (targetBL - driveBL.get_position()) * sgn(dis) > 20 || (driveFR.get_position() - targetFR) * sgn(dis) > 20 || (driveBR.get_position() - targetBR) * sgn(dis) > 20) {
    uint32_t curTime = millis();
    double actVel = ((driveFL.get_actual_velocity() + driveBL.get_actual_velocity() - driveFR.get_actual_velocity() - driveBR.get_actual_velocity()) / 4.0) / 60.0 * 360.0;
    double tkToDecel = (actVel * actVel) / (2 * ((1000.0 / TURN_DECEL_RATE) / 60.0) * 360.0);
    //printf("%d %f %f %f\n", i, actVel, targetFL - driveFL.get_position(), tkToDecel);
    if (tkToDecel < (targetFL - driveFL.get_position()) * sgn(dis) - 10) {
      if (curTime - lastAccel > TURN_ACCEL_RATE && i < vel) {
        i += 1;
        lastAccel = curTime;
      }
    }
    else {
      if (curTime - lastDecel > TURN_DECEL_RATE && i > 20) {
        i -= 1;
        lastDecel = curTime;
      }
    }
    setDriveVel(0, 0, i * sgn(dis));
    delay(1);
  }
  driveFL.move_absolute(targetFL, 25);
  driveBL.move_absolute(targetBL, 25);
  driveFR.move_absolute(targetFR, 25);
  driveBR.move_absolute(targetBR, 25);
  printf("%d Stopping from FL: %f, BL: %f, FR: %f, BR %f\n", millis(), driveFL.get_position(), driveBL.get_position(), driveFR.get_position(), driveBR.get_position());
  while (fabs(driveFL.get_position() - targetFL) > 3 || fabs(driveBL.get_position() - targetBL) > 3 || fabs(driveFR.get_position() - targetFR) > 3 || fabs(driveBR.get_position() - targetBR) > 3) delay(1);
  delay(100);
  printf("%d Turned to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), driveFL.get_position(), driveBL.get_position(), driveFR.get_position(), driveBR.get_position());
  //printf("%f %f\n", (encL.get_value() - encLStart) / 360.0 * 2.75 * M_PI, (encR.get_value() - encRStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}

double operator "" _tk(long double val) { return val; }
double operator "" _in(long double val) { return IN_TO_TK(val); }
double operator "" _cm(long double val) { return IN_TO_TK(CM_TO_IN(val)); }
double operator "" _deg(long double val) { return DEG_TO_RAD(val); }
