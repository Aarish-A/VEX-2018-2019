#include "auto.hpp"
#include "config.hpp"

using namespace pros;

double getGlobalAngle() {
	//return (DRIVE_DIA * M_PI * (driveFL.get_position() - driveBR.get_position() + driveBL.get_position() - driveFR.get_position())) / (2 * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH));
	return (enc_l.get_value() * SPN_TO_IN_L - enc_r.get_value() * SPN_TO_IN_R) / (WHL_DIS_L + WHL_DIS_R);
}

void resetGlobalAngle() {
	/*driveFL.tare_position();
  driveBL.tare_position();
  driveFR.tare_position();
  driveBR.tare_position();*/
	enc_l.reset();
	enc_r.reset();
}

void setDrive(int x, int y, int a) {
	drive_fl.move(y + x + a);
	drive_bl.move(y - x + a);
	drive_fr.move(y - x - a);
	drive_br.move(y + x - a);
}

void setDrive(int pow) {
  setDrive(pow, pow, pow);
}

void setDriveVel(int x, int y, int a) {
	drive_fl.move_velocity(y + x + a);
	drive_bl.move_velocity(y - x + a);
	drive_fr.move_velocity(y - x - a);
	drive_br.move_velocity(y + x - a);
}

void setDriveVel(int vel) {
  setDriveVel(vel, vel, vel);
}

void moveDrive(double dis, int vel, bool stop) {
  int enc_LStart = enc_l.get_value();
  int enc_RStart = enc_r.get_value();

  printf("%d Moving to %f\n", millis(), dis);

	double posCur;
  uint32_t lastAccel = 0, lastDecel = 0;
  int i = 10;
  while (true) {
    uint32_t curTime = millis();
		posCur = IN_TO_TK(pos.y);// ((enc_l.get_value() - enc_LStart) * SPN_TO_IN_L + (enc_r.get_value() - enc_RStart) * SPN_TO_IN_R) / 2.0;

		//printf("%d %f %f %d %d\n", curTime, pos, dis, enc_L.get_value(), enc_R.get_value());
		if ((dis - posCur) * sgn(dis) <= 0.5_in) break;
		double actVel = ((drive_fl.get_actual_velocity() + drive_bl.get_actual_velocity() + drive_fr.get_actual_velocity() + drive_br.get_actual_velocity()) / 4.0) / 60.0 * 360.0;
    double tkToDecel = (actVel * actVel) / (2 * ((1000.0 / MOVE_DECEL_RATE) / 60.0) * 360.0);
		double inToDecel = tkToDecel * DRIVE_DIA * M_PI / DRIVE_TPR;
    printf("%d %d %f %f\n", curTime, i, inToDecel, dis - posCur);
    if (inToDecel < (dis - posCur) * sgn(dis) - 0.5_in) {
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
	  double targetFL = drive_fl.get_position() + dis - posCur;
	  double targetBL = drive_bl.get_position() + dis - posCur;
	  double targetFR = drive_fr.get_position() + dis - posCur;
	  double targetBR = drive_br.get_position() + dis - posCur;

	  drive_fl.move_absolute(targetFL, 25);
	  drive_bl.move_absolute(targetBL, 25);
	  drive_fr.move_absolute(targetFR, 25);
	  drive_br.move_absolute(targetBR, 25);
	  printf("%d Stopping from FL: %f, BL: %f, FR: %f, BR %f\n", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
	  while (fabs(drive_fl.get_position() - targetFL) > 3 || fabs(drive_bl.get_position() - targetBL) > 3 || fabs(drive_fr.get_position() - targetFR) > 3 || fabs(drive_br.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	}
  printf("%d Moved to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  //printf("%f %f\n", (enc_L.get_value() - enc_LStart) / 360.0 * 2.75 * M_PI, (enc_R.get_value() - enc_RStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}

void turnDrive(double targ, int vel) {
  printf("%d Turning to %f\n", millis(), RAD_TO_DEG(targ));
	while (fabs(getGlobalAngle() - targ) > 0.8_deg) {
		setDriveVel(0, 0, (targ - getGlobalAngle()) * 120);
		delay(1);
	}
  printf("%d Turned to FL: %f, BL: %f, FR: %f, BR %f\n", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);
}

double operator "" _tk(long double val) { return val; }
