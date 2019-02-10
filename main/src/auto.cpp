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

void turn_vel(const AngleTarget& target, double kP, double offset, float drive_turn_handled_offset, short req_handled_num)
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
		setDriveVel(0, 0, (dA * kP));
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
		printf("  >>> %d Pow:%f | Turning %f CurA:%f, dA: %f| FL: %f, BL: %f, FR: %f, BR %f \n", millis(), pow, target.getTarget(), RAD_TO_DEG(getGlobalAngle()), RAD_TO_DEG(dA), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
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

void turn_vel_auto(const AngleTarget& target, double kP, double offset, float drive_turn_handled_offset)
{
	kP = fabs(kP);
	double dA = target.getTarget() - getGlobalAngle() + offset;
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f ", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );
	while (fabs(dA) > 0.8_deg) {
		//log_ln(" > %d Turning %f dA: %f| FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());
		dA = target.getTarget() - getGlobalAngle() + offset;
		//log_ln(LOG_AUTO, "%d dA: %f", pros::millis(), dA);
		if ((fabs(dA) < DEG_TO_RAD(drive_turn_handled_offset)) && drive_turn_handled_offset != 0) {
			auto_set_shot = true;
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
  double ticks_targ = dA/SPN_TO_IN_L*(WHL_DIS_L+WHL_DIS_R);
  printf("ticks targ: %f", ticks_targ);
  log_ln(LOG_AUTO, "%d Turning to %f | DeltaA: %f", millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );

	if (f_w) {
		while (fabs(dA) > 0.8_deg) {
			dA = target.getTarget() - getGlobalAngle() + offset;
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
			dA = target.getTarget() - getGlobalAngle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
      double full_pow = -50;//-kP*fabs(dA);
      double frac_pow = (full_pow * 0.25)*127/200;
      printf("global angle: %f\n",RAD_TO_DEG(getGlobalAngle()));
      printf("target angle: %f\n",RAD_TO_DEG(target.getTarget()));
      if (dA > 0) {
        if (dA < 1.8_deg)
        {
          drive_br.move_absolute(ticks_targ,35);
  				drive_fr.move_absolute(ticks_targ,35);
  				drive_bl.move_relative(0,200);
  				drive_fl.move_relative(0,200);
        }
        else
        {
  				drive_br.move_velocity(full_pow);
  				drive_fr.move_velocity(full_pow);
  				drive_bl.move(frac_pow);
  				drive_fl.move(frac_pow);
        }
			} else {
        if(dA>-1.8_deg)
        {
          drive_bl.move_absolute(ticks_targ,35);
  				drive_fl.move_absolute(ticks_targ,35);
  				drive_br.move_relative(0,200);
  				drive_fr.move_relative(0,200);
        }
        else
        {
          drive_bl.move_velocity(full_pow);
  				drive_fl.move_velocity(full_pow);
  				drive_br.move(frac_pow);
  				drive_fr.move(frac_pow);
        }
			}
			delay(5);
		}
	}
  setDrive(0);
	drive_brake();
  log_ln(LOG_AUTO, "%d Turned to %f in %d Vel:%f | FL: %f, BL: %f, FR: %f, BR %f", millis(), RAD_TO_DEG(getGlobalAngle()), millis()-t_start, pos.aVel, drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position());

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
