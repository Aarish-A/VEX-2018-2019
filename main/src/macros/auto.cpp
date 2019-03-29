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
    master.update();
    pros::delay(10);
  }
}

/* Shots */
void single_shot(double targ, bool wait) {
  angler.move_to(targ);
  while(angler.moving_to_target()) pros::delay(2);
  puncher.shoot();
  if (wait) while(puncher.shooting()) pros::delay(2);
  printf("Angle error at shot is %f\n", RAD_TO_DEG(drive.get_error()));
}

void double_shot(double targ1, double targ2, bool wait) {
  single_shot(targ1);
  single_shot(targ2, wait);
}

/* Capping */
void cap_on_pole() {
  drive.flatten_against_wall(false, true);
  capper.start_capping();
  drive.align_with_pole();
  capper.finish_capping();
  drive.flatten_against_wall(false,true);
  drive.reset_global_angle();
  while(capper.capping()) pros::delay(2);
}

/* Drive */
void climb_on_platform() {
  drive.flatten_against_wall(true, true);
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  drive.set_power(55, 15, 0);
  while (right_platform_sensor.get_value() < 2000) pros::delay(5);
  drive.set_power(-55, 15,0);
  while (right_platform_sensor.get_value() > 1900) pros::delay(5);
  drive.set_power(0, 0, 0);
  gyro.reset();
  angler.move_to(0);
  drive.br_motor.tare_position();
  for (int i = 40; i < 120; i++) {
    drive.set_side_power(i, i);
    pros::delay(3);
  }
  while (fabs(gyro.get_value()) < 150) pros::delay(5);
  while (fabs(gyro.get_value()) > 60) pros::delay(5);

  drive_move_sync(10_in);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  drive.set_power(0, -20, 0);
  pros::delay(150);
  drive.set_power(0, 0, 0);
  drive.reset_global_angle();
}

void drive_move(void* _params) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);

  drive_move_params* params = (drive_move_params*)_params;
  double dist_target = params->dist_target;
  double angle_target = params->angle_target;
  bool brake = params->brake;
  uint8_t max_power = params->max_power;
  int8_t start_power = params->start_power;
  bool decel = params->decel;
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
  double power = start_power;
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
    } else if (decel) {
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
    } else { }


    log_ln(LOG_MOVEMENT_ALGS, "%d Distance | Current: %f in, Error: %f in, Power: %f, P: %f, I: %f, D: %f", pros::millis(), dist_current, dist_error, power, dist_p_val, dist_i_val, dist_d_val);
    log_ln(LOG_MOVEMENT_ALGS, "%d Angle    | Current: %f deg, Error: %f deg, Angle Power: %f, P: %f, I: %f, D: %f", pros::millis(), RAD_TO_DEG(angle_current), RAD_TO_DEG(angle_error), angle_power, angle_p_val, angle_i_val, angle_d_val);
    log_ln(LOG_MOVEMENT_ALGS, "----------------------------------------------------------------");

    if (fabs(power) > max_power) power = max_power * sgn(power);
    drive.set_power(0, power, angle_power);

    dist_last_error = dist_error;
    angle_last_error = angle_error;
    pros::delay(1);
  } while (fabs(dist_error) > 0.3_in && sgn(dist_error) == sgn(dist_last_error));

  if (brake && decel) {
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

void drive_move_async(double dist_target, double angle_target, bool brake, uint8_t max_power, int8_t start_power, bool decel) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  if (drive_move_param != nullptr) {
    delete drive_turn_param;
    drive_move_param = nullptr;
  }
  drive_move_param = new drive_move_params{dist_target, angle_target, brake, max_power, start_power, decel};
  drive.set_error(drive_move_param->dist_target);
  drive.set_target(drive_move_param->dist_target);
  drive_move_task.start_task((void*)(drive_move_param));
}

void drive_move_sync(double dist_target, double angle_target, bool brake, uint8_t max_power, int8_t start_power, bool decel) {
  drive_move_params params = {dist_target, angle_target, brake, max_power, start_power, decel};
  drive_move((void*)(&params));
}

void drive_turn(void *_params) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  drive_turn_params* params = (drive_turn_params*)_params;
  const AngleTarget& target = params->target;
  drive.target = target.getTarget();

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

    if (fabs(fixeddA) < 15_deg && fabs(drive_volt) < 30) {
      drive_volt = sgn(drive_volt) * 30;
    }

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

    if (fabs(fixeddA) < 15_deg && fabs(drive_volt) < 25) {
      drive_volt = sgn(drive_volt) * 25;
    }

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
  drive.set_state(Drive::STATE_AUTO_CONTROL);
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

void sweep_turn(const AngleTarget& target, double radius, bool forwards, double post_distance, bool clockwise, bool brake, int max_power) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  double kP = 200/90;
  double kD = 00.0;
  double kI = 0.02;
  double p_val = 0;
  double d_val = 0;
  double i_val = 0;

  uint32_t start_time = pros::millis();
  double start_angle = drive.get_global_angle();
  double target_angle = target.getTarget();
  double angle_error = target_angle - start_angle;
  double dA = target.getTarget() - drive.get_global_angle();
  double target_half = angle_error/2;

  double enc_l_start = enc_l.get_value();
  double enc_r_start = enc_r.get_value();
  double delta_enc_l = 0;
  double delta_enc_r = 0;
  double current_pos = 0;

  double distance;
  double distance_right;
  double distance_left;
  double mecanum_distance_right;
  double mecanum_distance_left;
  double power_ratio;
  double power = 0;

  double error = 0;
  double last_error = 0;

  if (clockwise) {
    distance_right = angle_error * (radius + EDGE_TO_TRACKING_WHEEL);
    distance_left = angle_error * (radius + EDGE_TO_TRACKING_WHEEL + WHL_DIS_R * 2);
    mecanum_distance_left = radius + DRIVE_EDGE_TO_MECANUM + MECANUM_DRIVE_WIDTH;
    mecanum_distance_right = radius + DRIVE_EDGE_TO_MECANUM;
    power_ratio = mecanum_distance_right / mecanum_distance_left;
  } else {
    distance_left = angle_error * (radius + EDGE_TO_TRACKING_WHEEL);
    distance_right = angle_error * (radius + EDGE_TO_TRACKING_WHEEL + WHL_DIS_R * 2);
    mecanum_distance_right = radius + DRIVE_EDGE_TO_MECANUM + MECANUM_DRIVE_WIDTH;
    mecanum_distance_left = radius + DRIVE_EDGE_TO_MECANUM;
    power_ratio = mecanum_distance_left / mecanum_distance_right;
  }
  if(forwards) {
    power = 170;
    for(int i = 20; i < power; i++) {
      drive.set_side_vel(i, i * power_ratio);
      pros::delay(2);
    }
  }
  else {
    power = -170;
    for(int i = -20; i > power; i--) {
      drive.set_side_vel(i, i * power_ratio);
      pros::delay(2);
    }
  }
  while(fabs(dA)>0.8_deg)
  {
    dA = target.getTarget() - drive.get_global_angle();
    drive.set_side_vel(power, power * power_ratio);
    printf("fl: %f, bl: %f, fr: %f, br: %f\n", drive.fl_motor.get_actual_velocity(), drive.bl_motor.get_actual_velocity(), drive.fr_motor.get_actual_velocity(), drive.br_motor.get_actual_velocity());
  }
  drive_move_sync(post_distance, max_power, brake, target_angle);
}

void auto_update_task_stop_function() {
  puncher.set_holding();
  drive.driver_set(0, 0, 0);
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
