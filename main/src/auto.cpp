#include "auto.hpp"

using namespace pros;

bool blue_team = true;

/* Auto Update Task */
pros::Task* auto_update_task = nullptr;
void auto_update(void* param) {
  pos.reset();
  //if (pun_state != PunState::Loaded) pun_state_change(PunState::Loading);
  while (true)
  {
    //if (is_disabled) printf(" >>> %d IN AUTO_UPDATE IN DISABLED\n", millis());
    //pos.update();
    pun_handle();
    pros::delay(10);
  }
}

void auto_update_stop_task() {
	if(auto_update_task != nullptr)
	{
		log_ln(LOG_SHOTS, "  >>> %d Stop Auto Update Task", pros::millis());
    pun_set(PUN_HOLD_PWR);
    if(auto_update_task->get_state() != pros::E_TASK_STATE_DELETED)
      auto_update_task->remove();
		delete auto_update_task;
		auto_update_task = nullptr;
	}
}

void auto_update_start_task() {
	auto_update_stop_task();
	log_ln(LOG_SHOTS, "  >>> %d Start Auto Update Task", pros::millis());
	auto_update_task = new pros::Task((pros::task_fn_t)auto_update);//, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "auto_update_Task");
}

/* Auto puncher angler setter functions */
void auto_set_angler_target(double target) {
  auto_angler_target = target;
  angler_move(target);
  log_ln(LOG_AUTO, "%d Angler Move to %f. CurPos:%f", millis(), auto_angler_target, angler.get_position());
}

void auto_set_first_shot(double angler_target) {
  shot_req_handled_num = 0;
  auto_set_shot = true;
  auto_set_angler_target(angler_target);
}
void auto_set_second_shot(double angler_target) {
  shot_req_handled_num = 1;
  auto_set_shot = true;
  auto_set_angler_target(angler_target);
}


double getGlobalAngle() {
	//return (DRIVE_DIA * M_PI * (driveFL.get_position() - driveBR.get_position() + driveBL.get_position() - driveFR.get_position())) / (2 * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH));
	return (enc_l.get_value() * SPN_TO_IN_L - enc_r.get_value() * SPN_TO_IN_R) / (WHL_DIS_L + WHL_DIS_R);
	//return pos.a;
}

/* Movement Functions */
void resetGlobalAngle() {
	/*driveFL.tare_position();
  driveBL.tare_position();
  driveFR.tare_position();
  driveBR.tare_position();*/
	enc_l.reset();
	enc_r.reset();
  log_ln(LOG_AUTO, "%d Reset Global Angle. L: %d, R:%d", millis(), enc_l.get_value(), enc_r.get_value());
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
	/*
	pros::delay(150);
	log_ln(LOG_AUTO, "%d Done turn brake Pos:(%f, %f, %f), Vel:(%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, RAD_TO_DEG(pos.aVel));
	setDrive(0);
	*/
}

void move_drive_new(double distance, int max_vel, bool brake) {
  log_ln(LOG_AUTO, "%d Started moving %f inches straight", pros::millis(), distance);
  double enc_l_start = enc_l.get_value();
  double enc_r_start = enc_r.get_value();
  int start_time = pros::millis();

  // double kP = 200.0 / 30.0;
  double kP = 127.0 / 24.0_in;
  double kD = 20.0;
  double kI = 0.01;
  double p_val = 0;
  double d_val = 0;
  double i_val = 0;
  double current_pos;
  double delta_enc_l;
  double delta_enc_r;
  double error;
  double last_error = 0;
  double target_vel;
  double right_side = 0;
  double left_side = 0;
  double start_angle = getGlobalAngle();
  double angle_error;
  bool correcting = false;

  double ramp_voltage = 0;
  do {
    delta_enc_l = (enc_l.get_value() - enc_l_start);
    delta_enc_r = (enc_r.get_value() - enc_r_start);
    current_pos = (delta_enc_l * SPN_TO_IN_L + delta_enc_r * SPN_TO_IN_R) / 2.0;
    ramp_voltage += 0.5 * sgn(distance);
    setDrive(0, ramp_voltage, 0);
    log_ln(LOG_AUTO, "%d Ramping up at %f volts, Current Position: %f", pros::millis(), ramp_voltage, current_pos);
    pros::delay(1);
  } while(abs(current_pos) < abs(distance) * 0.15);

  do {
    delta_enc_l = (enc_l.get_value() - enc_l_start);
    delta_enc_r = (enc_r.get_value() - enc_r_start);
    current_pos = (delta_enc_l * SPN_TO_IN_L + delta_enc_r * SPN_TO_IN_R) / 2.0;
    error = distance - current_pos;
    angle_error = RAD_TO_DEG(getGlobalAngle() - start_angle);

    p_val = error * kP;
    if (last_error && abs(error) < abs(distance * 0.25) && (error - last_error) < 2.0_in) d_val = (error - last_error) * kD;
    else d_val = 0;

    if (abs(error) < 4.0_in) i_val += error * kI;
    else i_val = 0;

    target_vel = p_val + d_val + i_val;
    if (abs(target_vel) > max_vel) target_vel = sgn(target_vel) * max_vel;

    double angle_kP = 1.05 / 3.0;
    if (angle_error > (1 / angle_kP)) {
      if (distance > 0) {
        right_side = target_vel * abs(angle_error) * angle_kP;
        left_side = target_vel;
      } else if (distance < 0) {
        left_side = target_vel * abs(angle_error) * angle_kP;
        right_side = target_vel;
      }
      correcting = true;
    } else if (angle_error < -(1 / angle_kP)) {
      if (distance > 0) {
        right_side = target_vel;
        left_side = target_vel * abs(angle_error) * angle_kP;
      } else if (distance < 0) {
        left_side = target_vel;
        right_side = target_vel * abs(angle_error) * angle_kP;
      }

      correcting = true;
    } else correcting = false;

    log_ln(LOG_AUTO, "%d Left Side: %f, Right Side: %f", pros::millis(), left_side, right_side);
    log_ln(LOG_AUTO, "%d Moving to %f, Cur: %f, Error: %f, Target Vel: %f, P: %f, I: %f, D: %f", pros::millis(), distance, current_pos, error, target_vel, p_val, i_val, d_val);
    log_ln(LOG_AUTO, "%d Actual Velocities are: FR: %f, FL: %f, BL: %f, BR: %f", pros::millis(), drive_fr.get_actual_velocity(), drive_fl.get_actual_velocity(), drive_bl.get_actual_velocity(), drive_br.get_actual_velocity());
    log_ln(LOG_AUTO, "%d Start Angle: %f, Current Angle: %f, Delta Angle: %f", pros::millis(), RAD_TO_DEG(start_angle), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(getGlobalAngle() - start_angle));
    if (correcting) log_ln(LOG_AUTO, "%d CORRECTING", pros::millis());
    log_ln(LOG_AUTO, "----------------------------------------------------------------");

    if (correcting) setDriveTurn(left_side, right_side);
    else setDrive(0, target_vel, 0);

    last_error = error;
    pros::delay(1);
  } while(abs(error) > 0.5_in);

  if (brake) {
		double targetFL = drive_fl.get_position() + (error) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBL = drive_bl.get_position() + (error) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetFR = drive_fr.get_position() + (error) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBR = drive_br.get_position() + (error) * (DRIVE_TPR / (DRIVE_DIA * M_PI));

	  drive_fl.move_absolute(targetFL, 25);
	  drive_bl.move_absolute(targetBL, 25);
	  drive_fr.move_absolute(targetFR, 25);
	  drive_br.move_absolute(targetBR, 25);
	  log_ln(LOG_AUTO, "%d Stopping from FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
	  while (fabs(drive_fl.get_position() - targetFL) > 3 || fabs(drive_bl.get_position() - targetBL) > 3 || fabs(drive_fr.get_position() - targetFR) > 3 || fabs(drive_br.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	} else setDriveVel(0, 0, 0);
  current_pos = ((enc_l.get_value() - enc_l_start) * SPN_TO_IN_L + (enc_r.get_value() - enc_r_start) * SPN_TO_IN_R) / 2.0;
  error = distance - current_pos;
  log_ln(LOG_AUTO, "%d FINISHED TURN >>>> Took %d ms, Ended At: %f, Error: %f, Change in Angle %f", pros::millis(), pros::millis() - start_time, current_pos, error, RAD_TO_DEG(getGlobalAngle() - start_angle));
}


void move_drive(vector targ, int vel, bool stop) {
	double offset = (targ-pos.position()).magnitude();

  log_ln(LOG_AUTO, "%d Moving to (%f, %f) from (%f, %f, %f)", millis(), targ.x, targ.y, pos.x, pos.y, RAD_TO_DEG(pos.a));
  uint32_t lastAccel = 0, lastDecel = 0;
  int i = 10;
  while (true) {
		offset = (targ-pos.position()).magnitude();

    uint32_t curTime = millis();

		//log_ln("%d %f %f %d %d", curTime, pos, dis, enc_L.get_value(), enc_R.get_value());
		if (offset * sgn(offset) <= 0.5_in) break;
		double actVel = ((drive_fl.get_actual_velocity() + drive_bl.get_actual_velocity() + drive_fr.get_actual_velocity() + drive_br.get_actual_velocity()) / 4.0) / 60.0 * 360.0;
    double tkToDecel = (actVel * actVel) / (2 * ((1000.0 / MOVE_DECEL_RATE) / 60.0) * 360.0);
		double inToDecel = tkToDecel * DRIVE_DIA * M_PI / DRIVE_TPR;
    //log_ln(LOG_AUTO, "%d %d %f %f", curTime, i, inToDecel, dis - posCur);
    if (inToDecel < (offset) * sgn(offset) - 0.5_in) {
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
    setDriveVel(0, i * sgn(offset), 0);
    printf("%d VELOCITIES ARE: FR: %f, FL: %f, BL: %f, BR: %f", pros::millis(), drive_fr.get_actual_velocity(), drive_fl.get_actual_velocity(),
                                                                drive_bl.get_actual_velocity(), drive_br.get_actual_velocity());
    delay(1);
  }
	log_ln(LOG_AUTO, "%d Done move to (%f, %f) at (%f, %f, %f) Offs: %f", millis(), targ.x, targ.y, pos.x, pos.y, RAD_TO_DEG(pos.a), offset);
	if (stop) {
		double targetFL = drive_fl.get_position() + (offset) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBL = drive_bl.get_position() + (offset) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetFR = drive_fr.get_position() + (offset) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBR = drive_br.get_position() + (offset) * (DRIVE_TPR / (DRIVE_DIA * M_PI));

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


void move_drive_rel(double dis, int vel, bool stop) {
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
    //printf("  >> %d i:%d inToDecel:%f Dis:%f, A:%f \n", curTime, i, inToDecel, dis - posCur, RAD_TO_DEG(getGlobalAngle()));
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
  log_ln(LOG_AUTO, ">>%d Move done main loop FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));

	//printf("%f %f\n", dis, posCur);
	if (stop) {
		double targetFL = drive_fl.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBL = drive_bl.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetFR = drive_fr.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBR = drive_br.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));

	  drive_fl.move_absolute(targetFL, 25);
	  drive_bl.move_absolute(targetBL, 25);
	  drive_fr.move_absolute(targetFR, 25);
	  drive_br.move_absolute(targetBR, 25);
	  log_ln(LOG_AUTO, "%d Stopping from FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
	  while (fabs(drive_fl.get_position() - targetFL) > 3 || fabs(drive_bl.get_position() - targetBL) > 3 || fabs(drive_fr.get_position() - targetFR) > 3 || fabs(drive_br.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	}
  log_ln(LOG_AUTO, "%d Moved to FL: %f, BL: %f, FR: %f, BR %f |  Angle %f ", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  log_ln(LOG_AUTO, "%d FINISHED TURN >>>> Error: %f", pros::millis(), dis - posCur);
  //log_ln("%f %f", (enc_L.get_value() - enc_LStart) / 360.0 * 2.75 * M_PI, (enc_R.get_value() - enc_RStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}

void move_drive_rel_simple(double dis, int vel, bool stop) {
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
  log_ln(LOG_AUTO, ">>%d Moved simple done main loop FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));

	printf("%f %f\n", dis, posCur);
	if (stop) {
		double targetFL = drive_fl.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBL = drive_bl.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetFR = drive_fr.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));
	  double targetBR = drive_br.get_position() + (dis - posCur) * (DRIVE_TPR / (DRIVE_DIA * M_PI));

	  drive_fl.move_absolute(targetFL, 25);
	  drive_bl.move_absolute(targetBL, 25);
	  drive_fr.move_absolute(targetFR, 25);
	  drive_br.move_absolute(targetBR, 25);
	  log_ln(LOG_AUTO, "%d Stopping simple from FL: %f, BL: %f, FR: %f, BR %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
	  while (fabs(drive_fl.get_position() - targetFL) > 3 || fabs(drive_bl.get_position() - targetBL) > 3 || fabs(drive_fr.get_position() - targetFR) > 3 || fabs(drive_br.get_position() - targetBR) > 3) delay(1);
	  delay(100);
	}
  log_ln(LOG_AUTO, "%d Moved simple to FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  //log_ln("%f %f", (enc_L.get_value() - enc_LStart) / 360.0 * 2.75 * M_PI, (enc_R.get_value() - enc_RStart) / 360.0 * 2.75 * M_PI);
  setDrive(0);
}
void turn_vel_new(const AngleTarget& target)
{
  double dA = target.getTarget() - getGlobalAngle();
  double fixeddA = dA;
  double drive_volt = 0;
  double kP = 200/60;
  while(fabs(dA)> (0.4*fixeddA))
  {
    dA = target.getTarget() - getGlobalAngle();
    drive_volt = RAD_TO_DEG(dA) * kP;
    setDrive(0,0,drive_volt);
    //printf("hello world %f and %f\n", target.getTarget(), getGlobalAngle());
  }
  setDrive(0,0,35);
  while(fabs(dA)>1.3_deg) dA = target.getTarget() - getGlobalAngle();
  setDrive(0);
  drive_brake();
  delay(1000);
  printf("End angle is %f\n", RAD_TO_DEG(getGlobalAngle()));
}
void turn_vel(const AngleTarget& target, double kP, double offset, float drive_turn_handled_offset, short req_handled_num,  double max_vel)
{
	kP = fabs(kP);
	double dA = target.getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f ", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );
	while (fabs(dA) > 0.8_deg) {
		// log_ln(" > %d Turning %f dA: %f| FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
		dA = target.getTarget() - getGlobalAngle() + offset;
		if ((fabs(dA) < DEG_TO_RAD(drive_turn_handled_offset)) && drive_turn_handled_offset != 0) {
			shot_req[req_handled_num].drive_turn_handled = true;
			// log_ln(LOG_AUTO, "%d REACHED DRIVE TURN HANDLED OFFSET THRESHOLD", pros::millis());
		}
    float velocity = dA * kP;
    if (velocity > max_vel) velocity = max_vel;
		setDriveVel(0, 0, velocity);
		delay(2);
	}
  log_ln(LOG_AUTO, "%d Turned to %f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);
	drive_brake();
}

void turn_vel_fast(const AngleTarget& target, double kP, double offset, double correct_amount, bool correct_left)
{
  kP = fabs(kP);
	double dA = target.getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f ", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );

	while (fabs(dA) > correct_amount) {
    dA = target.getTarget() - getGlobalAngle() + offset;
    double pow = kP * dA;
    //if (fabs(pow) < 25) pow = 25 * sgn(pow);
		//printf("  >>> %d Pow:%f | Turning %f CurA:%f, dA: %f| FL: %f, BL: %f, FR: %f, BR %f \n", millis(), pow, target.getTarget(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
    setDrive(0, 0, pow);
		delay(2);
	}
  //log_ln(LOG_AUTO, "%d Turned to %f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);

  //Angle Correct at end

  dA = target.getTarget() - getGlobalAngle() + offset;
  double enc_rollout_targ = dA * (double)(WHL_DIS_L + WHL_DIS_R);
  double whl_tick_targ = IN_TO_TK(enc_rollout_targ)/5;

  printf("%d Print Start Angle Correction | A:%f. DA:%f. T:%f | enc_rollout_targ = %f, whl_tick_targ = %f | FL: %f, BL: %f, FR: %f, BR %f \n", millis(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), RAD_TO_DEG(target.getTarget()), enc_rollout_targ, whl_tick_targ, drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());


	if (dA > 0) {
    printf(">>%d Left Correct dA:%f \n", millis(), dA);
    drive_fl.move_relative(whl_tick_targ, 30);
    drive_bl.move_relative(whl_tick_targ, 30);
    drive_fr.move_relative(0, 30);
    drive_br.move_relative(0, 30);
  }
  else {
    printf(">>%d Right Correct dA:%f \n", millis(), dA);
    whl_tick_targ = -whl_tick_targ;
    drive_fr.move_relative(whl_tick_targ, 30);
    drive_br.move_relative(whl_tick_targ, 30);
    drive_fl.move_relative(0, 30);
    drive_bl.move_relative(0, 30);
  }

  ///drive_brake();
  log_ln(LOG_AUTO, "%d Start Angle Correction | A:%f. DA:%f. T:%f | enc_rollout_targ = %f, whl_tick_targ = %f", millis(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), RAD_TO_DEG(target.getTarget()), enc_rollout_targ, whl_tick_targ);
  u_int32_t start_correction_t = millis();
  while (millis() < start_correction_t + 300)
  {
    dA = target.getTarget() - getGlobalAngle() + offset;
    //printf("  C>>> %d A:%f dA:%f T:%f | %f %f %f %f | %d %d \n", millis(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), RAD_TO_DEG(target.getTarget()), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), enc_l.get_value(), enc_r.get_value());
    pros::delay(10);
  }
  setDriveVel(0);
  setDrive(0);
  log_ln(LOG_AUTO, "%d Stop Angle Correction | A:%f. DA:%f. T:%f | enc_rollout_targ = %f, whl_tick_targ = %f", millis(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), RAD_TO_DEG(target.getTarget()), enc_rollout_targ, whl_tick_targ);
  start_correction_t = millis();
  while (millis() < start_correction_t + 70)
  {
    dA = target.getTarget() - getGlobalAngle() + offset;
    //printf("  Wait>>> %d A:%f dA:%f T:%f | %f %f %f %f | %d %d \n", millis(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), RAD_TO_DEG(target.getTarget()), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), enc_l.get_value(), enc_r.get_value());
    pros::delay(10);
  }
}

void turn_vel_auto(const AngleTarget& target, double kP, double offset, float drive_turn_handled_offset, double angler_target)
{
	kP = fabs(kP);
	double dA = target.getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f ", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );
	while (fabs(dA) > 0.8_deg) {
		//log_ln(" > %d Turning %f dA: %f| FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
		dA = target.getTarget() - getGlobalAngle() + offset;
		//log_ln(LOG_AUTO, "%d dA: %f", pros::millis(), dA);
		if ((fabs(dA) < DEG_TO_RAD(drive_turn_handled_offset)) && drive_turn_handled_offset != 0) {
      shot_req_handled_num = 0;
      auto_set_shot = true;
      auto_set_angler_target(angler_target);
			log_ln(LOG_AUTO, "%d REACHED DRIVE TURN HANDLED OFFSET THRESHOLD", pros::millis());
		}
		setDriveVel(0, 0, (dA * kP));
		delay(2);
	}
  log_ln(LOG_AUTO, "%d Turned to %f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);
	drive_brake();
}

void turn_vel_side(const AngleTarget& target, double kP, double offset, bool f_w)
{
	int t_start = pros::millis();
	double dA = target.getTarget() - getGlobalAngle() + offset;
  drive_fl.tare_position();
  drive_br.tare_position();
  drive_bl.tare_position();
  drive_fr.tare_position();
  double ticks_targ = dA/SPN_TO_IN_L*(WHL_DIS_L+WHL_DIS_R);
  printf("ticks targ: %f", ticks_targ);
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );
  printf("Da is %f", dA);
	if (f_w) {
		while (fabs(dA) > 0.8_deg) {
			dA = target.getTarget() - getGlobalAngle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
			if (dA > 0) {
				drive_bl.move_velocity(kP*fabs(dA));
				drive_fl.move_velocity(kP*fabs(dA));
				drive_br.move_relative(0,200);
				drive_fr.move_relative(0,200);
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
  if(dA<0)
  {
    double left_per = -1;
    double right_per = -0.3;
    double kP = 350;
    double error;

    do {
      dA = target.getTarget() - getGlobalAngle() + offset;
      error = fabs(dA);
      double base_pow = error * kP + 10;
      if (base_pow > 60) base_pow = 60;
      int left_pow = round(base_pow * left_per);
      int right_pow = round(base_pow * right_per);

      drive_fl.move(left_pow);
      drive_bl.move(left_pow);
      drive_fr.move(right_pow);
      drive_br.move(right_pow);

      printf("%f %f\n", RAD_TO_DEG(error), base_pow);

      delay(1);
    } while (fabs(error) > 0.6_deg);

    double left = -15 * left_per;
    if (fabs(left) < 5) left = 5 * sgn(left);
    double right = -15 * right_per;
    if (fabs(right) < 5) right = 5 * sgn(right);
    drive_fl.move(left);
    drive_bl.move(left);
    drive_fr.move(right);
    drive_br.move(right);
    delay(100);
  }
  else
  {
    double left_per = -0.3;
    double right_per = -1;
    double kP = 350;
    double error;

    do {
      dA = target.getTarget() - getGlobalAngle() + offset;
      error = fabs(dA);
      double base_pow = error * kP + 10;
      if (base_pow > 60) base_pow = 60;
      int left_pow = round(base_pow * left_per);
      int right_pow = round(base_pow * right_per);

      drive_fl.move(left_pow);
      drive_bl.move(left_pow);
      drive_fr.move(right_pow);
      drive_br.move(right_pow);

      printf("%f %f\n", RAD_TO_DEG(error), base_pow);

      delay(1);
    } while (fabs(error) > 0.6_deg);

    double left = -15 * left_per;
    if (fabs(left) < 5) left = 5 * sgn(left);
    double right = -15 * right_per;
    if (fabs(right) < 5) right = 5 * sgn(right);
    drive_fl.move(left);
    drive_bl.move(left);
    drive_fr.move(right);
    drive_br.move(right);
    delay(100);
  }
  setDrive(0);
	drive_brake();
  log_ln(LOG_AUTO, "%d Turned to %f in %d Vel:%f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), millis()-t_start, pos.aVel, drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
}

}

void turn_vel_side_simple(const AngleTarget& target, double kP, double offset, bool f_w)
{
	int t_start = pros::millis();
	double dA = target.getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );

	if (f_w) {
		while (fabs(dA) > 0.8_deg) {
			dA = target.getTarget() - getGlobalAngle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
			if (dA > 0) {
				drive_bl.move_velocity(kP*fabs(dA));
				drive_fl.move_velocity(kP*fabs(dA));
				drive_br.move_relative(0, 100);
				drive_fr.move_relative(0, 100);
			} else {
				drive_br.move_velocity(kP*fabs(dA));
				drive_fr.move_velocity(kP*fabs(dA));
				drive_bl.move_relative(0, 100);
				drive_fl.move_relative(0, 100);
			}
			delay(5);
		}
	}
	else {
		while (fabs(dA) > 0.8_deg) {
			dA = target.getTarget() - getGlobalAngle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
			if (dA > 0) {
				drive_br.move_velocity(-kP*fabs(dA));
				drive_fr.move_velocity(-kP*fabs(dA));
				drive_bl.move_relative(0, 100);
				drive_fl.move_relative(0, 100);
			} else {
				drive_bl.move_velocity(-kP*fabs(dA));
				drive_fl.move_velocity(-kP*fabs(dA));
				drive_br.move_relative(0, 100);
				drive_fr.move_relative(0, 100);
			}
			delay(5);
		}
	}
  log_ln(LOG_AUTO, "%d Turned to %f in %d Vel:%f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), millis()-t_start, pos.aVel, drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
  setDrive(0);
	drive_brake();
}

void flatten_against_wall(bool f_w, bool hold, int hold_power) {
	//pos.reset(0,0,0);
	//int pow = 40;
	int hold_pow = hold_power;
	if (f_w) {
		//log_ln("%d FW Start", pros::millis());
		setDrive(0,40, 0);
		pros::delay(200);
		do {
			//log_ln("%d Reset Back Up(%f, %f, %f) Vel(%f, %f, %f) VeelLoc(%f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.aVel, pos.velLocal.x, pos.velLocal.y);
			pros::delay(10);
		} while (abs(drive_bl.get_actual_velocity()) > 1); //aVel < -0.1);
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
		} while (abs(drive_fl.get_actual_velocity()) > 1); //aVel < -0.1);
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
