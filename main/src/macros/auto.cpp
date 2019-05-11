#include "auto.hpp"

pilons::Task auto_update_task("Auto Update", auto_update, auto_update_task_stop_function);
pilons::Task drive_move_task("Drive Move", drive_move, drive_task_stop_function);
pilons::Task drive_turn_task("Drive Turn", drive_turn, drive_task_stop_function);
pilons::Task cap_on_pole_task("Cap on Pole", cap_on_pole_task_function, cap_on_pole_stop_function);

uint8_t capper_count = 0;

drive_move_params drive_move_param;
drive_turn_params* drive_turn_param = nullptr;
FixedAngleTarget fixed_angle_target(0_deg);
PointAngleTarget point_angle_target({0, 0});
bool fixed_target = true;


void auto_update(void* _params) {
  uint32_t timer = pros::millis() + 1250;
  while(true) {
    Subsystem::update_all();
    master.update();
    pos.update();
    pros::delay(5);
  }
}

/* Shots */
void single_shot(double targ, bool wait) {
  angler.move_to(targ);
  while(angler.moving_to_target()) pros::delay(2);
  puncher.shoot();
  if (wait) while(puncher.shooting()) pros::delay(2);
  log_ln(AUTO, "Angle error at shot is %f", RAD_TO_DEG(drive.get_error()));
}

void double_shot(double targ1, double targ2, bool wait) {
  bool intake_on = intake.off() ? false : true;
  if (intake_on) intake.stop();
  single_shot(targ1);
  pros::delay(75);
  single_shot(targ2, wait);
  if (intake_on) intake.intake();
}

/* Capping */
void cap_on_pole() {
  drive.flatten_against_wall(false, true);
  printf("started cap on pole func\n");
  capper.start_capping();
  printf("started capping\n");
  drive.align_with_pole();
  capper.finish_capping();
  drive.flatten_against_wall(false,true);
  drive.reset_global_angle();
  while(capper.capping()) pros::delay(2);
}

void cap_on_pole_task_function(void* _param) {
  if (capper_count == 1) {
    cap_on_pole();
    angler.move_to(Angler::PICKUP_POSITION);

    drive_move_sync(5_in, 0_deg);
    drive_turn_sync(FixedAngleTarget(13.5_deg));
    double_shot(shot_positions[DS_BACK_MID], shot_positions[DS_BACK_TOP]);
    angler.move_to(Angler::CAP_PICKUP_POSITION);
    capper.move_to_flag_flip(200);
    pros::delay(75);
    intake.outtake();
  } else if (capper_count == 3) {
    cap_on_pole();
    drive_move_async(20_in, 0_deg);
    drive.wait_for_distance(5_in);
    drive.wait_for_stop();
    capper.move_to_velocity(Capper::CARRY_POSITION, 200);
    drive_turn_sync(FixedAngleTarget(90_deg));
    double_shot(shot_positions[DS_FINAL_MID], shot_positions[DS_FINAL_TOP]);
  } else {
    cap_on_pole();
    master.rumble("-");
    drive_move_sync(4_in);
  }
  // drive_move_sync(5_in,0_deg);
  // angler.move_to(Angler::CAP_PICKUP_POSITION);
  // intake.intake();
  // drive_move_async(24.5_in, 0_deg);
  // drive.wait_for_distance(7_in);
  // capper.move_to_velocity(30 * Capper::GEAR_RATIO, 200);
  // drive.wait_for_stop();
  // drive_move_sync(-7_in,0_deg);
  // //drive_move_sync(18.5_in,4.5_deg);
  // //  angler.move_to(Angler::CAP_PICKUP_POSITION);
  //
  // drive_turn_sync(FixedAngleTarget(-88_deg));
  //
  // capper.move_to_velocity(56 * Capper::GEAR_RATIO, 200);
  // angler.move_to(Angler::CAP_FLIP_POSITION);
  // intake.intake();
  // drive_move_async(15.5_in, -88_deg);
  // drive.wait_for_distance(14.5_in);
  // drive_move_async(-2.5_in, -88_deg, true, 80);
  // capper.move_to_power(28 * Capper::GEAR_RATIO, -100);
  // drive.wait_for_stop();
  // pros::delay(50);
  // drive_move_sync(-5_in, -88_deg);
  // intake.stop();
  // capper.move_to_pickup();
  // drive_move_async(9.5_in, -88_deg);
  // drive.wait_for_distance(5.0_in);
  // capper.pickup_cap();
  // pros::delay(250);
  // drive.wait_for_stop();
  // intake.intake();
  // drive_turn_sync(FixedAngleTarget(-79_deg));
  // drive_move_sync(-33_in, -82_deg, false);
  // cap_on_pole();
}

/* Drive */

void climb_on_platform() {
  drive.flatten_against_wall(true, true);
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  drive.set_power(55, 15, 0);
  while (right_platform_sensor.get_value() < 2000) pros::delay(5);
  drive.set_power(-55, 15,0);
  while (right_platform_sensor.get_value() > 2500) pros::delay(5);
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

  drive_move_async(8.5_in);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  // drive.set_power(0, -20, 0);
  // pros::delay(150);
  drive.reset_global_angle();
}

void drive_move(void* _params) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);

  // drive_move_params* params = static_cast<drive_move_params*>(_params);
  double dist_target = drive_move_param.dist_target;
  double angle_target = drive_move_param.angle_target;
  bool brake = drive_move_param.brake;
  uint8_t max_power = drive_move_param.max_power;
  int8_t start_power = drive_move_param.start_power;
  bool decel = drive_move_param.decel;
  bool carrying_cap = drive_move_param.carrying_cap;
  drive.target = dist_target;

  // If the correct angle is default, the correct angle should be the starting angle
  double angle_start = drive.get_global_angle();
  if (angle_target == 1000) angle_target = angle_start;

  uint32_t start_time = pros::millis();
  log_ln(MOVE, AUTO, "%d Started moving %f inches straight, Starting angle is %f deg, Correcting to %f deg", pros::millis(), dist_target, RAD_TO_DEG(angle_start), RAD_TO_DEG(angle_target));

  // Distance correction PID Values
  double dist_kP = 127 / 25.0_in;
  double dist_kD = 53.0;
  double dist_kI = 0.015;
  double dist_p_val = 0;
  double dist_i_val = 0;
  double dist_d_val = 0;

  // Angle Correction PID Values
  double angle_kP = carrying_cap ? 2.0 / 2.0_deg : 5.0 / 2.0_deg;
  double angle_kD = carrying_cap ? 80.00 : 69.00;
  double angle_kI = carrying_cap ? 0.525 : 0.0350;
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

  bool safety = false;
  uint32_t safety_timeout = 0;
  uint32_t safety_count = 1;
  double safety_error = 0.0;
  double safety_last_error = 0.0;

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
      log_ln(MOVE_DEBUGGING, "%d Ramping up...", pros::millis(), power, dist_current, dist_error, angle_current, angle_error);
    } else if (decel) {
      // Calculate PID values for distance
      dist_p_val = dist_error * dist_kP;

      if (fabs(dist_error) < 5.5_in) dist_i_val += dist_error * dist_kI;
      else dist_i_val = 0;

      if (fabs(dist_error) < fabs(dist_target * 0.30) && (dist_error - dist_last_error) < 2.0_in) dist_d_val = (dist_error - dist_last_error) * dist_kD;
      else dist_d_val = 0;

      // // Calculate PID Values for angle_error
      // angle_p_val = angle_error * angle_kP;
      //
      // angle_i_val += angle_error * angle_kI;
      // // if (abs(angle_error) < 1.5_deg) angle_i_val += angle_error * angle_kI;
      // // else angle_i_val = 0;
      // if (sgn(angle_error) != sgn(angle_last_error)) angle_i_val = 0;
      //
      // if (fabs(angle_error) < 2.0_deg) angle_d_val = (angle_error - angle_last_error) * angle_kD;
      // else angle_d_val = 0;

      // Calculate powers
      power = dist_p_val + dist_i_val + dist_d_val;
      // if (fabs(dist_current) > fabs(dist_target) * 0.60) angle_power = angle_p_val + angle_i_val + angle_d_val;
      // else angle_power = 0;

      if (safety_count % 10 == 0) {
        safety_error = dist_error;
        if (fabs(safety_last_error - safety_error) < 0.050 && !safety_timeout) safety_timeout = pros::millis();
        else if (fabs(safety_last_error - safety_error) >= 0.050) safety_timeout = 0;

        if (safety_timeout && pros::millis() - safety_timeout >= 200) {
          safety = true;
          break;
        }
        safety_last_error = safety_error;
      }
      safety_count++;

      log_ln(MOVE_DEBUGGING, "%d In PID...", pros::millis());
    }


    log_ln(MOVE_DEBUGGING, "%d Distance | Current: %f in, Error: %f in, Power: %f, P: %f, I: %f, D: %f", pros::millis(), dist_current, dist_error, power, dist_p_val, dist_i_val, dist_d_val);
    log_ln(MOVE_DEBUGGING, "%d Angle    | Current: %f deg, Error: %f deg, Angle Power: %f, P: %f, I: %f, D: %f", pros::millis(), RAD_TO_DEG(angle_current), RAD_TO_DEG(angle_error), angle_power, angle_p_val, angle_i_val, angle_d_val);
    log_ln(MOVE_DEBUGGING, "----------------------------------------------------------------");

    if (fabs(power) > max_power) power = max_power * sgn(power);

    if (carrying_cap ? true : fabs(dist_current) > fabs(dist_target) * 0.30) {
      angle_p_val = angle_error * angle_kP;
      angle_i_val += angle_error * angle_kI;
      if (sgn(angle_error) != sgn(angle_last_error)) angle_i_val = 0;
      if (fabs(angle_error) < 2.0_deg) angle_d_val = (angle_error - angle_last_error) * angle_kD;
      else angle_d_val = 0;
      angle_power = angle_p_val + angle_i_val + angle_d_val;
    } else angle_power = 0;

    // drive.set_power(0, power, angle_power);
    if (carrying_cap) drive.set_side_power(sgn(power + angle_power) == sgn(dist_target) ? power + angle_power : 0, sgn(power - angle_power) == sgn(dist_target) ? power - angle_power : 0);
    else drive.set_power(0, power, angle_power);

    // if (fabs(dist_target) > 15_in) {
    //   if (dist_target > 0 && (drive.fl_motor.get_actual_velocity() < 1 || drive.fr_motor.get_actual_velocity() < 1)) {
    //     safety = true;
    //     break;
    //   } else if (dist_target > 0 && (drive.bl_motor.get_actual_velocity() < 1 || drive.br_motor.get_actual_velocity() < 1)) {
    //     safety = true;
    //     break;
    //   }
    // }

    dist_last_error = dist_error;
    angle_last_error = angle_error;

    pros::delay(1);
  } while (fabs(dist_error) > 0.3_in && sgn(dist_error) == sgn(dist_last_error));

  if (brake && decel && !safety) {
    double targetFL = drive.fl_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));
    double targetBL = drive.bl_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));
    double targetFR = drive.fr_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));
    double targetBR = drive.br_motor.get_position() + (dist_error) * (drive.TPR / (drive.WHEEL_DIAMETER * M_PI));

    drive.fl_motor.move_absolute(targetFL, 25);
    drive.bl_motor.move_absolute(targetBL, 25);
    drive.fr_motor.move_absolute(targetFR, 25);
    drive.br_motor.move_absolute(targetBR, 25);
    log_ln(MOVE_DEBUGGING, "%d Stopping from FL: %f, BL: %f, FR: %f, BR %f", pros::millis(), drive.fl_motor.get_position(), drive.bl_motor.get_position(), drive.fr_motor.get_position(), drive.br_motor.get_position());
    uint32_t temp = pros::millis();
    while (fabs(drive.fl_motor.get_position() - targetFL) > 4 || fabs(drive.bl_motor.get_position() - targetBL) > 4 || fabs(drive.fr_motor.get_position() - targetFR) > 4 || fabs(drive.br_motor.get_position() - targetBR) > 4) {
      // printf("Errors: FL: %f, BL: %f, FR: %f, BR: %f\n", fabs(drive.fl_motor.get_position() - targetFL), fabs(drive.bl_motor.get_position() - targetBL), fabs(drive.fr_motor.get_position() - targetFR), fabs(drive.br_motor.get_position() - targetBR));
      // printf("Velocities: FL: %f, BL: %f, FR: %f, BR: %f\n", drive.fl_motor.get_actual_velocity(), drive.bl_motor.get_actual_velocity(), drive.fr_motor.get_actual_velocity(), drive.br_motor.get_actual_velocity());
      pros::delay(1);
    }
    // printf("Finished the loop at %d millis\n", pros::millis() - temp);
    pros::delay(100);
  } else drive.set_vel(0, 0, 0);

  dist_current = ((drive.enc_l.get_value() - enc_l_start) * SPN_TO_IN_L + (drive.enc_r.get_value() - enc_r_start) * SPN_TO_IN_R) / 2.0;
  angle_current = drive.get_global_angle();

  dist_error = dist_target - dist_current;
  angle_error = angle_target - angle_current;

  log_ln(MOVE, AUTO, "%d FINISHED MOVE >>>> Took %d ms, Ended At: %f, Distance Error: %f, Angle Error: %f", pros::millis(), pros::millis() - start_time, dist_current, dist_error, RAD_TO_DEG(angle_error));
  drive.set_state(Drive::STATE_DRIVER_CONTROL);
  // pros::delay(100);

  // dist_current = ((drive.enc_l.get_value() - enc_l_start) * SPN_TO_IN_L + (drive.enc_r.get_value() - enc_r_start) * SPN_TO_IN_R) / 2.0;
  // angle_current = drive.get_global_angle();

  // dist_error = dist_target - dist_current;
  // angle_error = angle_target - angle_current;

  // log_ln(LOG_AUTO, "%d FINISHED MOVE SECOND ONE >>>> Took %d ms, Ended At: %f, Distance Error: %f, Angle Error: %f", pros::millis(), pros::millis() - start_time, dist_current, dist_error, RAD_TO_DEG(angle_error));
  drive_move_task.stop_task();
}

void drive_move_async(double dist_target, double angle_target, bool brake, uint8_t max_power, int8_t start_power, bool decel, bool carrying_cap) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  // if (drive_move_param != nullptr) {
  //   delete drive_move_param;
  //   drive_move_param = nullptr;
  // }
  drive_move_param = {dist_target, angle_target, brake, max_power, start_power, decel, carrying_cap};
  drive.set_error(drive_move_param.dist_target);
  drive.set_target(drive_move_param.dist_target);
  // drive_move_task.start_task((void*)(drive_move_param));
  drive_move_task.start_task();
}

void drive_move_sync(double dist_target, double angle_target, bool brake, uint8_t max_power, int8_t start_power, bool decel, bool carrying_cap) {
  // drive_move_task.stop_task();
  drive_move_param = {dist_target, angle_target, brake, max_power, start_power, decel, carrying_cap};
  drive_move(nullptr);
}

void drive_turn(void *_params) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  // drive_turn_params* params = static_cast<drive_turn_params*>(_params);

  // const AngleTarget& t = (fixed_target ? fixed_angle_target : point_angle_target);
  uint32_t start_time = pros::millis();

  // Angle Variables
  double target = fixed_target ? fixed_angle_target.getTarget() : point_angle_target.getTarget(); //drive_turn_param->target.getTarget();
  drive.target = target;// drive_turn_param->target.getTarget();
  double start = drive.get_global_angle();
  double current = drive.get_global_angle();
  double error = target - current;
  double last_error = error;
  log_ln(MOVE, AUTO, "Drive Turn Start: Angle is %.3f, Moving to %.3f\n", RAD_TO_DEG(drive.get_global_angle()), RAD_TO_DEG(target));

  // Angle correction PID Values

  // double kP = 127 / 119_deg;
  // fabs(error) > 55_deg ? map(fabs(error), 45_deg, 180_deg, 127 / 85.0_deg, 127 / 125.0_deg) : 127 / 85.0_deg;
  double kP = map_set(fabs(error), 0_deg, 180_deg, 127 / 50_deg, 127 / 119_deg,
                      5_deg,    127 / 50_deg,
                      15_deg,  127 / 93_deg,
                      30_deg,  127 / 106_deg,
                      45_deg,  127 / 107_deg,
                      60_deg,  127 / 110_deg,
                      90_deg,  127 / 119_deg,
                      180_deg, 127 / 119_deg);

  // fabs(error) <= 45_deg ? 700.0 : map(fabs(error), 50_deg, 180_deg, 700.0, 3000.0);
  double kD = map_set(fabs(error), 0_deg, 180_deg, 1100.0, 2650.0,
                      5_deg,   1100.0,
                      15_deg,  1400.0,
                      30_deg,  2000.0,
                      45_deg,  2850.0,
                      60_deg,  3305.0,
                      90_deg,  2650.0,
                      180_deg, 2650.0);

  // fabs(error) > 45_deg ? map(fabs(error), 45_deg, 180_deg, 0.280, 0.010) : fabs(error) >= 22.5_deg ? 0.280 : map(fabs(error), 0_deg, 22.5_deg, 2.450, 0.280);
  double kI = map_set(fabs(error), 0_deg, 180_deg, 1.45, 0.445,
                      5_deg,   1.450,
                      15_deg,  0.310,
                      30_deg,  0.190,
                      45_deg,  0.260,
                      60_deg,  0.345,
                      90_deg,  0.445,
                      180_deg, 0.445);


  double i_limit = 25.0;
  double p_val = 0;
  double i_val = 0;
  double d_val = 0;

  // Power Variables
  double power = 0;
  double ramp_up_power = fabs(error) >= 60_deg ? 0.75 : map(fabs(error), 0_deg, 60_deg, 0.125, 0.75);

  if (fabs(error) < 1.5_deg) goto turn_end;

  do {
    // Get current angle
    current = drive.get_global_angle();

    // Calculate errors
    error = target - current;
    drive.error = error;

    if (fabs(error) > fabs(target - start) * 0.75) {
      power += ramp_up_power * sgn(error);
      // log_ln(MOVE_DEBUGGING, AUTO, "Ramping up...");
      // printf("%d | Ramping up...", pros::millis());
    } else {
      // Calculate PID values for distance
      p_val = error * kP;
      // if (fabs(target) - fabs(start) > 50_deg) p_val = error > 15.0_deg ? error * kP : error * kP * map(fabs(error), 0.0, 15_deg, 0.1, 0.55);
      // else p_val = error > 7.0_deg ? error * kP : error * kP * map(fabs(error), 0.0, 7_deg, 0.3, 0.75);

      if (fabs(i_val) >= i_limit) i_val = i_limit * sgn(i_val);
      else if (fabs(error) < 15_deg) i_val += error * kI * map(fabs(error), 0.0, 15.0, 2.5, 0.5);
      else i_val = 0;

      if (fabs(error) < fabs(target - start) * 0.15 && (error - last_error) < 3.0_deg) d_val = (error - last_error) * kD * 1.75;
      else if (fabs(error) < fabs(target - start) * 0.40 && (error - last_error) < 3.0_deg) d_val = (error - last_error) * kD;
      else d_val = 0;

      // Calculate powers
      power = 2 * p_val + i_val + d_val;

      // log_ln(MOVE_DEBUGGING, AUTO, "In PID...");
      // printf("%d | In PID...    ", pros::millis());
    }

    log_ln(MOVE_DEBUGGING, AUTO, "TURN | Current: %.3f deg, Target: %.3f deg, Error: %.3f deg, Angle Power: %.3f, P: %.3f, I: %.3f, D: %.3f", RAD_TO_DEG(current), RAD_TO_DEG(target), RAD_TO_DEG(error), power, p_val, i_val, d_val);
    // printf(" Angle  | Current: %f deg, Target: %f deg, Error: %f deg, Angle Power: %f, P: %f, I: %f, D: %f\n", RAD_TO_DEG(current), RAD_TO_DEG(target), RAD_TO_DEG(error), power, p_val, i_val, d_val);
    // log_ln(MOVE_DEBUGGING, AUTO, "----------------------------------------------------------------");

    if (sgn(error) != sgn(last_error)) break;

    drive.set_power(0, 0, power);

    last_error = error;
    pros::delay(1);
  } while (fabs(error) > 0.45_deg);

  // if (sgn(error) != sgn(last_error)) drive.set_power(0, 0, fabs(power) * 0.8 * sgn(error));
  // else drive.set_power(0, 0, fabs(power) * -0.8 * sgn(error));
  // pros::delay(50);
  drive.brake();
  pros::delay(25);
  log_ln(MOVE, AUTO, "FINISHED TURN >>>> Took %d ms, Ended At: %f, Angle Error: %f, kP: %.3f, kI: %.3f, kD: %.3f, kP should be %.3f", pros::millis() - start_time, RAD_TO_DEG(current), RAD_TO_DEG(error), kP, kI, kD);

turn_end:
  // pros::delay(100);
  // current = drive.get_global_angle();
  // error = target - current;
  // log_ln(MOVE, AUTO, "FINISHED TURN 1 >>>> Took %d ms, Ended At: %f, Angle Error: %f", pros::millis() - start_time, RAD_TO_DEG(current), RAD_TO_DEG(error));
  // pros::delay(100);
  // current = drive.get_global_angle();
  // error = target - current;
  // log_ln(MOVE, AUTO, "FINISHED TURN 2 >>>> Took %d ms, Ended At: %f, Angle Error: %f", pros::millis() - start_time, RAD_TO_DEG(current), RAD_TO_DEG(error));
  // pros::delay(100);
  // current = drive.get_global_angle();
  // error = target - current;
  // log_ln(MOVE, AUTO, "FINISHED TURN 3 >>>> Took %d ms, Ended At: %f, Angle Error: %f", pros::millis() - start_time, RAD_TO_DEG(current), RAD_TO_DEG(error));
  // pros::delay(100);
  // current = drive.get_global_angle();
  // error = target - current;
  // log_ln(MOVE, AUTO, "FINISHED TURN 4 >>>> Took %d ms, Ended At: %f, Angle Error: %f", pros::millis() - start_time, RAD_TO_DEG(current), RAD_TO_DEG(error));

  drive.set_state(Drive::STATE_DRIVER_CONTROL);
  drive_turn_task.stop_task();
}

void drive_turn_async(FixedAngleTarget target) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  fixed_target = true;
  fixed_angle_target = target;
  drive.set_error(fixed_angle_target.getTarget() - drive.get_global_angle());
  drive.set_target(fixed_angle_target.getTarget());
  drive_turn_task.start_task();
}

void drive_turn_sync(FixedAngleTarget target) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  fixed_target = true;
  fixed_angle_target = target;
  drive_turn(nullptr);
}

void drive_turn_async(PointAngleTarget target) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  fixed_target = false;
  point_angle_target = target;
  drive.set_error(point_angle_target.getTarget() - drive.get_global_angle());
  drive.set_target(point_angle_target.getTarget());
  drive_turn_task.start_task();
}

void drive_turn_sync(PointAngleTarget target) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  fixed_target = false;
  point_angle_target = target;
  drive_turn(nullptr);
}

// void drive_turn_async(const AngleTarget& target) {
//   drive.set_state(Drive::STATE_AUTO_CONTROL);
//   // if (drive_turn_param != nullptr) {
//   //   delete drive_turn_param;
//   //   drive_turn_param = nullptr;
//   // }
//   // log_ln(IO, "YEEEEEEAAA");
//   //drive_turn_param = new drive_turn_params{target};
//   drive.set_error(5_deg);
//   drive.set_target(5_deg);
//   drive_turn_task.start_task();
// }

// void drive_turn_sync(const AngleTarget& target) {
//   // log_ln(IO, "YEEEEEEAAA NUMBER 5");
//   //drive_turn_param = new drive_turn_params{target};
//   drive_turn(nullptr);
// }

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

void drive_turn_side(const AngleTarget& target, double kP, double offset, bool forwards) {
  drive.set_state(Drive::STATE_AUTO_CONTROL);
  int t_start = pros::millis();
	double dA = target.getTarget() - drive.get_global_angle() + offset;
  drive.fl_motor.tare_position();
  drive.bl_motor.tare_position();
  drive.br_motor.tare_position();
  drive.fr_motor.tare_position();
  double ticks_targ = dA/SPN_TO_IN_L*(WHL_DIS_L+WHL_DIS_R);
  // printf("ticks targ: %f", ticks_targ);
  log_ln(MOVE, AUTO, "%d Turning to %f | DeltaA: %f", pros::millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );
  // printf("Da is %f", dA);
  printf("ticks targ: %f", ticks_targ);
  log_ln(MOVE, AUTO, "%d Turning to %f | DeltaA: %f", pros::millis(), RAD_TO_DEG(target.getTarget()), RAD_TO_DEG(dA) );
  printf("Da is %f", dA);
	if (forwards) {
		while (fabs(dA) > 0.8_deg) {
			dA = target.getTarget() - drive.get_global_angle() + offset;
			//log("%d Pos:%f DeltaA:%f Pow:%f \n", pros::millis(), RAD_TO_DEG(pos.a), RAD_TO_DEG(dA), kP*fabs(dA));
			if (dA > 0) {
				drive.bl_motor.move_velocity(kP*fabs(dA));
				drive.fl_motor.move_velocity(kP*fabs(dA));
				drive.br_motor.move_relative(0,200);
				drive.fr_motor.move_relative(0,200);
			} else {
				drive.br_motor.move_velocity(kP*fabs(dA));
				drive.fr_motor.move_velocity(kP*fabs(dA));
				drive.bl_motor.move_velocity(0);
				drive.fl_motor.move_velocity(0);
			}
			pros::delay(5);
		}
	} else {
    if(dA < 0_deg) {
      double left_per = -1;
      double right_per = -0.3;
      double kP = 350;
      double error;

      do {
        dA = target.getTarget() - drive.get_global_angle() + offset;
        error = fabs(dA);
        double base_pow = error * kP + 10;
        if (base_pow > 60) base_pow = 60;
        int left_pow = round(base_pow * left_per);
        int right_pow = round(base_pow * right_per);

        drive.fl_motor.move(left_pow);
        drive.bl_motor.move(left_pow);
        drive.fr_motor.move(right_pow);
        drive.br_motor.move(right_pow);

        // printf("%f %f\n", RAD_TO_DEG(error), base_pow);

        pros::delay(1);
      } while (fabs(error) > 0.6_deg);

      double left = -15 * left_per;
      if (fabs(left) < 5) left = 5 * sgn(left);
      double right = -15 * right_per;
      if (fabs(right) < 5) right = 5 * sgn(right);
      drive.fl_motor.move(left);
      drive.bl_motor.move(left);
      drive.fr_motor.move(right);
      drive.br_motor.move(right);
      pros::delay(100);
    } else {
      double left_per = -0.3;
      double right_per = -1;
      double kP = 350;
      double error;

      do {
        dA = target.getTarget() - drive.get_global_angle() + offset;
        error = fabs(dA);
        double base_pow = error * kP + 10;
        if (base_pow > 60) base_pow = 60;
        int left_pow = round(base_pow * left_per);
        int right_pow = round(base_pow * right_per);

        drive.fl_motor.move(left_pow);
        drive.bl_motor.move(left_pow);
        drive.fr_motor.move(right_pow);
        drive.br_motor.move(right_pow);

        // printf("%f %f\n", RAD_TO_DEG(error), base_pow);

        pros::delay(1);
      } while (fabs(error) > 0.6_deg);

      double left = -15 * left_per;
      if (fabs(left) < 5) left = 5 * sgn(left);
      double right = -15 * right_per;
      if (fabs(right) < 5) right = 5 * sgn(right);
      drive.fl_motor.move(left);
      drive.bl_motor.move(left);
      drive.fr_motor.move(right);
      drive.br_motor.move(right);
      pros::delay(100);
  }
  drive.set_power(0);
	drive.brake();
  log_ln(MOVE, AUTO, "%d Turned to %f in %d Vel:%f | FL: %f, BL: %f, FR: %f, BR %f", pros::millis(), RAD_TO_DEG(drive.get_global_angle()), pros::millis()-t_start, pos.aVel, drive.fl_motor.get_position(), drive.bl_motor.get_position(), drive.fr_motor.get_position(), drive.br_motor.get_position());
  }
  drive.set_state(Drive::STATE_DRIVER_CONTROL);
}

void auto_update_task_stop_function() {
  puncher.set_holding();
  drive.driver_set(0, 0, 0);
}

void drive_task_stop_function() {
  drive.set_state(Drive::STATE_DRIVER_CONTROL);
  drive.set_power(0);/*
  if (drive_turn_param != nullptr) {
    delete drive_turn_param;
    drive_turn_param = nullptr;
  }*/
}

void cap_on_pole_stop_function() {
  drive.set_power(0);
  drive.set_state(Drive::STATE_DRIVER_CONTROL);
}
