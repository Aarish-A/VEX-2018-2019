#include "auto.hpp"

pilons::Task auto_update_task("Auto Update", auto_update, auto_update_task_stop_function);
pilons::Task drive_move_task("Drive Move", drive_move, drive_task_stop_function);
pilons::Task drive_turn_task("Drive Turn", drive_turn, drive_task_stop_function);

drive_move_params* drive_move_param = nullptr;
drive_turn_params* drive_turn_param = nullptr;

void auto_update(void* _params) {
  uint32_t timer = pros::millis() + 1250;
  while(true) {
    Subsystem::update_all();
    if (pros::millis() > timer) printf("state: %d\n", drive_turn_task.get_state());
    pros::delay(10);
  }
}

/* Shots */
void single_shot(double targ, bool wait) {
  angler.move_to(targ);
  while(angler.moving_to_target()) pros::delay(2);
  puncher.shoot();
  if (wait) while(puncher.shooting()) pros::delay(2);
}

void double_shot(double targ1, double targ2, bool wait) {
  single_shot(targ1);
  single_shot(targ2, wait);
}

/* Capping */
void cap_on_pole() {
}

/* Drive */
void drive_move(void* _params) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);

  drive_move_params* params = (drive_move_params*)_params;
  double dist_target = params->dist_target;
  double angle_target = params->angle_target;
  bool brake = params-brake;
  uint8_t max_power = params->max_power;
  drive.target = dist_target;

  // If the correct angle is default, the correct angle should be the starting angle
  double angle_start = drive.get_global_angle();
  if (angle_target == 1000) angle_target = angle_start;

  uint32_t start_time = pros::millis();
  log_ln(LOG_AUTO, "%d Started moving %f inches straight, Starting angle is %f deg, Correcting to %f deg", pros::millis(), dist_target, RAD_TO_DEG(angle_start), RAD_TO_DEG(angle_target));

  // Distance correction PID Values
  double dist_kP = 127 / 25.0_in;
  double dist_kD = 53.0;
  double dist_kI = 0.015;
  double dist_p_val = 0;
  double dist_i_val = 0;
  double dist_d_val = 0;

  // Angle Correction PID Values
  double angle_kP = 5.0 / 2.0_deg;
  double angle_kD = 69.00;
  double angle_kI = 0.0350;
  double angle_p_val = 0;
  double angle_i_val = 0;
  double angle_d_val = 0;

  // Tracking wheel variables
  double enc_l_start = drive.enc_l.get_value();
  double enc_r_start = drive.enc_r.get_value();
  double delta_enc_l = 0;
  double delta_enc_r = 0;

  // Distance Variables
  double dist_current = 0;
  double dist_error = 0;
  double dist_last_error = 0;

  // Angle variables
  double angle_current = 0;
  double angle_error = 0;
  double angle_last_error = 0;
  bool angle_correcting = false;

  // Power variables
  double power = 0;
  double angle_power = 0;

  do {
    // Calculate how far the robot has moved and get current angle
    delta_enc_l = (drive.enc_l.get_value() - enc_l_start);
    delta_enc_r = (drive.enc_r.get_value() - enc_r_start);
    dist_current = (delta_enc_l * SPN_TO_IN_L + delta_enc_r * SPN_TO_IN_R) / 2.0;
    angle_current = drive.get_global_angle();

    // Calculate distance and angle errors
    dist_error = dist_target - dist_current;
    angle_error = angle_target - angle_current;
    drive.error = dist_error;

    // Ramp up while the robot hasn't moved 8 inches and is not within 2 inches of it's target
    // if (abs(dist_current) < 8_in && dist_error > 2_in) {
    if (fabs(dist_current) < fabs(dist_target) * 0.30) {
      power += 0.40 * sgn(dist_target);
      log_ln(LOG_MOVEMENT_ALGS, "%d Ramping up...", pros::millis(), power, dist_current, dist_error, angle_current, angle_error);
    } else {
      // Calculate PID values for distance
      dist_p_val = dist_error * dist_kP;

      if (fabs(dist_error) < 5.5_in) dist_i_val += dist_error * dist_kI;
      else dist_i_val = 0;

      if (fabs(dist_error) < fabs(dist_target * 0.30) && (dist_error - dist_last_error) < 2.0_in) dist_d_val = (dist_error - dist_last_error) * dist_kD;
      else dist_d_val = 0;

      // Calculate PID Values for angle_error
      angle_p_val = angle_error * angle_kP;

      angle_i_val += angle_error * angle_kI;
      // if (abs(angle_error) < 1.5_deg) angle_i_val += angle_error * angle_kI;
      // else angle_i_val = 0;
      if (sgn(angle_error) != sgn(angle_last_error)) angle_i_val = 0;

      if (fabs(angle_error) < 2.0_deg) angle_d_val = (angle_error - angle_last_error) * angle_kD;
      else angle_d_val = 0;

      // Calculate powers
      power = dist_p_val + dist_i_val + dist_d_val;
      angle_power = angle_p_val + angle_i_val + angle_d_val;

      log_ln(LOG_MOVEMENT_ALGS, "%d In PID...", pros::millis());
    }


    log_ln(LOG_MOVEMENT_ALGS, "%d Distance | Current: %f in, Error: %f in, Power: %f, P: %f, I: %f, D: %f", pros::millis(), dist_current, dist_error, power, dist_p_val, dist_i_val, dist_d_val);
    log_ln(LOG_MOVEMENT_ALGS, "%d Angle    | Current: %f deg, Error: %f deg, Angle Power: %f, P: %f, I: %f, D: %f", pros::millis(), RAD_TO_DEG(angle_current), RAD_TO_DEG(angle_error), angle_power, angle_p_val, angle_i_val, angle_d_val);
    log_ln(LOG_MOVEMENT_ALGS, "----------------------------------------------------------------");

    drive.set_power(0, power, angle_power);

    dist_last_error = dist_error;
    angle_last_error = angle_error;
    pros::delay(1);
  } while (fabs(dist_error) > 0.3_in && sgn(dist_error) == sgn(dist_last_error));

  if (brake) {
    double targetFL = drive.fl_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));
    double targetBL = drive.bl_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));
    double targetFR = drive.fr_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));
    double targetBR = drive.br_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));

    drive.fl_motor.move_absolute(targetFL, 25);
    drive.bl_motor.move_absolute(targetBL, 25);
    drive.fr_motor.move_absolute(targetFR, 25);
    drive.br_motor.move_absolute(targetBR, 25);
    log_ln(LOG_AUTO, "%d Stopping from FL: %f, BL: %f, FR: %f, BR %f", pros::millis(), drive.fl_motor.get_position(), drive.bl_motor.get_position(), drive.fr_motor.get_position(), drive.br_motor.get_position());
    while (fabs(drive.fl_motor.get_position() - targetFL) > 3 || fabs(drive.bl_motor.get_position() - targetBL) > 3 || fabs(drive.fr_motor.get_position() - targetFR) > 3 || fabs(drive.br_motor.get_position() - targetBR) > 3) pros::delay(1);
    pros::delay(100);
  } else drive.set_vel(0, 0, 0);

  dist_current = ((drive.enc_l.get_value() - enc_l_start) * SPN_TO_IN_L + (drive.enc_r.get_value() - enc_r_start) * SPN_TO_IN_R) / 2.0;
  angle_current = drive.get_global_angle();

  dist_error = dist_target - dist_current;
  angle_error = angle_target - angle_current;

  log_ln(LOG_AUTO, "%d FINISHED MOVE >>>> Took %d ms, Ended At: %f, Distance Error: %f, Angle Error: %f", pros::millis(), pros::millis() - start_time, dist_current, dist_error, RAD_TO_DEG(angle_error));
  drive_move_task.stop_task();
}

void drive_move_async(double dist_target, double angle_target, bool brake, uint8_t max_power) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  if (drive_move_param != nullptr) {
    delete drive_turn_param;
    drive_move_param = nullptr;
  }
  drive_move_param = new drive_move_params{dist_target, angle_target, brake, max_power};
  drive.set_error(drive_move_param->dist_target);
  drive.set_target(drive_move_param->dist_target);
  drive_move_task.start_task((void*)(drive_move_param));
  // pros::delay(100);
  // delete params;
}

void drive_move_sync(double dist_target, double angle_target, bool brake, uint8_t max_power) {
  drive_move_params params = {dist_target, angle_target, brake, max_power};
  drive_move((void*)(&params));
}

void drive_turn(void *_params) {
  printf("Made it to drive turn func\n");
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  drive_turn_params* params = (drive_turn_params*)_params;
  const AngleTarget& target = params->target;
  drive.target = target.getTarget();
  printf("made it to further in the drive turnfunc\n");

  double dA = target.getTarget() - drive.get_global_angle();
  double fixeddA = dA;
  double drive_volt = 0;
  double kP = 200/50;
  double kI = 0.02;
  double kD = 70;
  double iVal = 0;
  double last_error = 0;
  double dVal = 0;

  while (fabs(dA) > fabs(0.4 * fixeddA)) {
    dA = target.getTarget() - drive.get_global_angle();
    drive.error = dA;
    drive_volt = RAD_TO_DEG(dA) * kP;
    drive.set_power(0, 0, drive_volt);
    pros::delay(1);
  }

  kP = 200/100;
  while (fabs(dA) > 0.5_deg) {
    if (fabs(fixeddA)<25_deg) {
      iVal += RAD_TO_DEG(dA) * kI;
      if (fabs(iVal) > 25) iVal = 25 * sgn(iVal);
    } else {
      if (fabs(dA) < (0.17 * fabs(fixeddA))) {
        iVal += RAD_TO_DEG(dA)*kI;
        if (fabs(iVal) > 25) iVal = 25 * sgn(iVal);
      } else iVal = 0;
    }

    dA = target.getTarget() - drive.get_global_angle();
    drive.error = dA;
    drive_volt = RAD_TO_DEG(dA) * kP + iVal;
    drive.set_power(0, 0, drive_volt);
    log_ln(LOG_MOVEMENT_ALGS, "%d Drive voltage is: %f and iVal is %f dA is %f", pros::millis(),drive_volt, iVal, RAD_TO_DEG(dA));
    pros::delay(1);
  }
  drive.set_power(0);
  drive.brake();
  pros::delay(50);
  log_ln(LOG_AUTO, "End angle is %f\n", RAD_TO_DEG(drive.get_global_angle()));
  drive_turn_task.stop_task();
}

void drive_turn_async(const AngleTarget& target) {
  if (drive_turn_param != nullptr) {
    delete drive_turn_param;
    drive_turn_param = nullptr;
  }
  drive_turn_param = new drive_turn_params{target};
  drive.set_error(drive_turn_param->target.getTarget());
  drive.set_target(drive_turn_param->target.getTarget());
  drive_turn_task.start_task((void*)(drive_turn_param));
}

void drive_turn_sync(const AngleTarget& target) {
  drive_turn_params params = {target};
  drive_turn((void*)(&params));
}

void auto_update_task_stop_function() {
  // Subsystem::disable_all();
  puncher.set_holding();
}

void drive_task_stop_function() {
  drive.set_state(Drive::STATE_DRIVER_CONTROL);
  drive.set_power(0);
  if (drive_turn_param != nullptr) {
    delete drive_turn_param;
    drive_turn_param = nullptr;
  }
  if (drive_move_param != nullptr) {
    delete drive_move_param;
    drive_move_param = nullptr;
  }
}
