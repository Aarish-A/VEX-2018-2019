#include "auto.hpp"
#include "config.hpp"

using namespace pros;

double getGlobalAngle() {
	//return (DRIVE_DIA * M_PI * (driveFL.get_position() - driveBR.get_position() + driveBL.get_position() - driveFR.get_position())) / (2 * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH));
	return (encL.get_value() * SPN_TO_IN_L - encR.get_value() * SPN_TO_IN_R) / (WHL_DIS_L + WHL_DIS_R);
}

void resetGlobalAngle() {
	/*driveFL.tare_position();
  driveBL.tare_position();
  driveFR.tare_position();
  driveBR.tare_position();*/
	encL.reset();
	encR.reset();
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
  int encLStart = encL.get_value();
  int encRStart = encR.get_value();

  printf("%d Moving to %f\n", millis(), dis);

	double pos;
  uint32_t lastAccel = 0, lastDecel = 0;
  int i = 10;
  while (true) {
    uint32_t curTime = millis();
		pos = ((encL.get_value() - encLStart) * SPN_TO_IN_L + (encR.get_value() - encRStart) * SPN_TO_IN_R) / 2.0;
		//printf("%d %f %f %d %d\n", curTime, pos, dis, encL.get_value(), encR.get_value());
		if ((dis - pos) * sgn(dis) <= 0.5_in) break;
		double actVel = ((driveFL.get_actual_velocity() + driveBL.get_actual_velocity() + driveFR.get_actual_velocity() + driveBR.get_actual_velocity()) / 4.0) / 60.0 * 360.0;
    double tkToDecel = (actVel * actVel) / (2 * ((1000.0 / MOVE_DECEL_RATE) / 60.0) * 360.0);
		double inToDecel = tkToDecel * DRIVE_DIA * M_PI / DRIVE_TPR;
    printf("%d %d %f %f\n", curTime, i, inToDecel, dis - pos);
    if (inToDecel < (dis - pos) * sgn(dis) - 0.5_in) {
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
	  double targetFL = driveFL.get_position() + dis - pos;
	  double targetBL = driveBL.get_position() + dis - pos;
	  double targetFR = driveFR.get_position() + dis - pos;
	  double targetBR = driveBR.get_position() + dis - pos;

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

void turnDrive(double targ, int vel) {
  printf("%d Turning to %f\n", millis(), RAD_TO_DEG(targ));
	while (fabs(getGlobalAngle() - targ) > 0.8_deg) {
		setDriveVel(0, 0, (targ - getGlobalAngle()) * 120);
		delay(1);
	}
  printf("%d Turned to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), driveFL.get_position(), driveBL.get_position(), driveFR.get_position(), driveBR.get_position());
  setDrive(0);
}

double operator "" _tk(long double val) { return val; }
double operator "" _in(long double val) { return val; }
double operator "" _cm(long double val) { return CM_TO_IN(val); }
double operator "" _deg(long double val) { return DEG_TO_RAD(val); }
