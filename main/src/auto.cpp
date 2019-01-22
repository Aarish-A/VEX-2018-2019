#include "auto.hpp"

using namespace pros;

bool blue_team = true;

double getGlobalAngle() {
	//return (DRIVE_DIA * M_PI * (driveFL.get_position() - driveBR.get_position() + driveBL.get_position() - driveFR.get_position())) / (2 * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH));
	//return (enc_l.get_value() * SPN_TO_IN_L - enc_r.get_value() * SPN_TO_IN_R) / (WHL_DIS_L + WHL_DIS_R);
	return pos.a;
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

void setDriveTurn(int left, int right) {
	drive_fl.move(left);
	drive_bl.move(left);
	drive_fr.move(right);
	drive_br.move(right);
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

void drive_brake() {
	setDrive(0);
	drive_fl.move_relative(0, 100);
	drive_bl.move_relative(0, 100);
	drive_fr.move_relative(0, 100);
	drive_br.move_relative(0, 100);
}

void move_drive(double dis, int vel, bool stop) {
  int enc_LStart = enc_l.get_value();
  int enc_RStart = enc_r.get_value();

  log_ln(LOG_AUTO, "%d Moving to %f", millis(), dis);

	double posCur;
  uint32_t lastAccel = 0, lastDecel = 0;
  int i = 10;
  while (true) {
    uint32_t curTime = millis();
		posCur = ((enc_l.get_value() - enc_LStart) * SPN_TO_IN_L + (enc_r.get_value() - enc_RStart) * SPN_TO_IN_R) / 2.0; // IN_TO_TK(pos.y);

		//log_ln("%d %f %f %d %d", curTime, pos, dis, enc_L.get_value(), enc_R.get_value());
		if ((dis - posCur) * sgn(dis) <= 0.5_in) break;
		double actVel = ((drive_fl.get_actual_velocity() + drive_bl.get_actual_velocity() + drive_fr.get_actual_velocity() + drive_br.get_actual_velocity()) / 4.0) / 60.0 * 360.0;
    double tkToDecel = (actVel * actVel) / (2 * ((1000.0 / MOVE_DECEL_RATE) / 60.0) * 360.0);
		double inToDecel = tkToDecel * DRIVE_DIA * M_PI / DRIVE_TPR;
    log_ln(LOG_AUTO, "%d %d %f %f", curTime, i, inToDecel, dis - posCur);
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
	printf("%f %f\n", dis, posCur);
	if (stop) {
	  double targetFL = drive_fl.get_position() + dis - posCur;
	  double targetBL = drive_bl.get_position() + dis - posCur;
	  double targetFR = drive_fr.get_position() + dis - posCur;
	  double targetBR = drive_br.get_position() + dis - posCur;

	  drive_fl.move_absolute(targetFL, 25);
	  drive_bl.move_absolute(targetBL, 25);
	  drive_fr.move_absolute(targetFR, 25);
	  drive_br.move_absolute(targetBR, 25);
	  log_ln(LOG_AUTO, "%d Stopping from FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
	  while (fabs(drive_fl.get_position() - targetFL) > 3 || fabs(drive_bl.get_position() - targetBL) > 3 || fabs(drive_fr.get_position() - targetFR) > 3 || fabs(drive_br.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	}
  log_ln(LOG_AUTO, "%d Moved to FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  //log_ln("%f %f", (enc_L.get_value() - enc_LStart) / 360.0 * 2.75 * M_PI, (enc_R.get_value() - enc_RStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}

void move_drive_simple(double dis, int vel, bool stop) {
	int enc_LStart = enc_l.get_value();
  int enc_RStart = enc_r.get_value();

  log_ln(LOG_AUTO, "%d Moving simple to %f", millis(), dis);

	setDriveVel(0, vel * sgn(dis), 0);

	double posCur;
  do {
		posCur = ((enc_l.get_value() - enc_LStart) * SPN_TO_IN_L + (enc_r.get_value() - enc_RStart) * SPN_TO_IN_R) / 2.0; // IN_TO_TK(pos.y);
		//log_ln("%d %f %f %d %d", curTime, pos, dis, enc_L.get_value(), enc_R.get_value());
    delay(1);
  } while ((dis - posCur) * sgn(dis) > 0.5_in);
	printf("%f %f\n", dis, posCur);
	if (stop) {
	  double targetFL = drive_fl.get_position() + dis - posCur;
	  double targetBL = drive_bl.get_position() + dis - posCur;
	  double targetFR = drive_fr.get_position() + dis - posCur;
	  double targetBR = drive_br.get_position() + dis - posCur;

	  drive_fl.move_absolute(targetFL, 25);
	  drive_bl.move_absolute(targetBL, 25);
	  drive_fr.move_absolute(targetFR, 25);
	  drive_br.move_absolute(targetBR, 25);
	  log_ln(LOG_AUTO, "%d Stopping simple from FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
	  while (fabs(drive_fl.get_position() - targetFL) > 3 || fabs(drive_bl.get_position() - targetBL) > 3 || fabs(drive_fr.get_position() - targetFR) > 3 || fabs(drive_br.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	}
  log_ln(LOG_AUTO, "%d Moved simple to FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  //log_ln("%f %f", (enc_L.get_value() - enc_LStart) / 360.0 * 2.75 * M_PI, (enc_R.get_value() - enc_RStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}

void turn_vel(AngleTarget *target, double kP, double offset)
{
	kP = fabs(kP);
	double dA = target->getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f ", millis(), RAD_TO_DEG(target->getTarget()), RAD_TO_DEG(dA) );
	while (fabs(dA) > 0.8_deg) {
		//log_ln(" > %d Turning %f dA: %f| FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
		dA = target->getTarget() - getGlobalAngle() + offset;
		setDriveVel(0, 0, (dA * kP));
		delay(2);
	}
  log_ln(LOG_AUTO, "%d Turned to %f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);
	drive_brake();
}

void turn_vel_side(AngleTarget *target, double kP, double offset, bool f_w)
{
	int t_start = pros::millis();
	double dA = target->getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f", millis(), RAD_TO_DEG(target->getTarget()), RAD_TO_DEG(dA) );

	if (f_w) {
		while (fabs(dA) > 0.8_deg) {
			dA = target->getTarget() - getGlobalAngle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
			if (dA > 0) {
				drive_bl.move_velocity(kP*fabs(dA));
				drive_fl.move_velocity(kP*fabs(dA));
				drive_br.move_velocity(0);
				drive_fr.move_velocity(0);
			} else {
				drive_br.move_velocity(kP*fabs(dA));
				drive_fr.move_velocity(kP*fabs(dA));
				drive_bl.move_velocity(0);
				drive_fl.move_velocity(0);
			}
			delay(5);
		}
	}
	else {
		while (fabs(dA) > 0.8_deg) {
			dA = target->getTarget() - getGlobalAngle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
			if (dA > 0) {
				drive_br.move_velocity(-kP*fabs(dA));
				drive_fr.move_velocity(-kP*fabs(dA));
				drive_bl.move_velocity(0);
				drive_fl.move_velocity(0);
			} else {
				drive_bl.move_velocity(-kP*fabs(dA));
				drive_fl.move_velocity(-kP*fabs(dA));
				drive_br.move_velocity(0);
				drive_fr.move_velocity(0);
			}
			delay(5);
		}
	}
  log_ln(LOG_AUTO, "%d Turned to %f in %d Vel:%f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), millis()-t_start, pos.aVel, drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);
	drive_brake();
}

void flatten_against_wall(bool f_w, bool hold) {
	//pos.reset(0,0,0);
	//int pow = 40;
	int hold_pow = 15;
	if (f_w) {
		//log_ln("%d FW Start", pros::millis());
		setDrive(0,40, 0);
		pros::delay(200);
		do {
			//log_ln("%d Reset Back Up(%f, %f, %f) Vel(%f, %f, %f) VeelLoc(%f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.aVel, pos.velLocal.x, pos.velLocal.y);
			pros::delay(10);
		} while (pos.velLocal.y > 1); //aVel < -0.1);
		if (hold) setDrive(0, hold_pow, 0);
		else setDrive(0);
	}
	else {
		//log_ln("%d FW Start", pros::millis());
		setDrive(0,-60, 0);
		pros::delay(200);
		do {
			//log_ln("%d Reset Back Up(%f, %f, %f) Vel(%f, %f, %f) VeelLoc(%f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.aVel, pos.velLocal.x, pos.velLocal.y);
			pros::delay(10);
		} while (pos.velLocal.y < -1); //aVel < -0.1);
		if (hold) setDrive(0, -hold_pow, 0);
		else setDrive(0);
	}
	log_ln(LOG_AUTO, "%d Done flatten_against_wall", pros::millis());
	//setDrive(0, -20, 0);
}

void flatten_angle(bool left, bool right, bool hold) {
	int hold_pow = 10;
	int pow = 60;
	int accel_time = 150;

	//right
	if (right) {
		setDriveTurn(hold_pow, pow);
		int t_start = millis();
		pros::delay(accel_time);
			printf("	>>Start %d %f \n", millis(), RAD_TO_DEG(pos.aVel));
		do {
			printf("%d %f \n", millis(), RAD_TO_DEG(pos.aVel));
			pros::delay(100);
		} while (RAD_TO_DEG(pos.aVel) < -1);
		printf("	>>End %d %f \n", millis(), RAD_TO_DEG(pos.aVel));
	}

	//Left
	if (left) {
		setDriveTurn(pow, hold_pow);
		pros::delay(accel_time);
			printf("	>>Start %d %f \n", millis(), RAD_TO_DEG(pos.aVel));
		do {
			printf("%d %f \n", millis(), RAD_TO_DEG(pos.aVel));
			pros::delay(100);
		} while (RAD_TO_DEG(pos.aVel) > 1);
		printf("	>>End %d %f \n", millis(), RAD_TO_DEG(pos.aVel));

		if (hold) setDrive(0, hold_pow, 0);
		else setDrive(0);
	}
}


double operator "" _tk(long double val) { return val; }
