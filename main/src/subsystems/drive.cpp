#include "drive.hpp"

/* Private Functions */
void Drive::set_power(double x, double y, double a) {
  this->fl_motor.move(y + x + a);
  this->bl_motor.move(y - x + a);
  this->fr_motor.move(y - x - a);
  this->br_motor.move(y + x - a);
}

void Drive::set_power(double power) {
  this->set_power(power, power, power);
}

void Drive::set_vel(double x, double y, double a) {
  this->fl_motor.move_velocity(y + x + a);
  this->bl_motor.move_velocity(y - x + a);
  this->fr_motor.move_velocity(y - x - a);
  this->br_motor.move_velocity(y + x - a);
}

void Drive::set_vel(double velocity) {
  this->set_vel(velocity, velocity, velocity);
}

void Drive::set_side_power(double left, double right) {
  this->fl_motor.move(left);
  this->bl_motor.move(left);
  this->fr_motor.move(right);
  this->br_motor.move(right);
}

void Drive::set_side_vel(double left, double right) {
  this->fl_motor.move_velocity(left);
  this->bl_motor.move_velocity(left);
  this->fr_motor.move_velocity(right);
  this->br_motor.move_velocity(right);
}

void Drive::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(this->state) {
    case STATE_DISABLED:
      this->set_power(0);
      this->x = 0;
      this->y = 0;
      this->a = 0;
      break;
    case STATE_RESET:
      this->set_state(STATE_DRIVER_CONTROL);
      break;
    case STATE_DRIVER_CONTROL:
      break;
    case STATE_AUTO_CONTROL:
      break;
  }
}

void Drive::brake() {
  set_power(0);
  this->fl_motor.move_relative(0, 20);
  this->bl_motor.move_relative(0, 20);
  this->fr_motor.move_relative(0, 20);
  this->br_motor.move_relative(0, 20);
}

double Drive::get_global_angle() {
  return (this->enc_l.get_value() * SPN_TO_IN_L - this->enc_r.get_value() * SPN_TO_IN_R) / (WHL_DIS_L + WHL_DIS_R);
}

/* Constructor */
Drive::Drive(std::string subsystem_name, uint8_t default_state, pros::Motor& fl_motor, pros::Motor& fr_motor, pros::Motor& bl_motor, pros::Motor& br_motor, pros::ADIAnalogIn& pole_poti, pros::ADIEncoder& enc_l, pros::ADIEncoder& enc_r) : Subsystem(subsystem_name, default_state), fl_motor(fl_motor), fr_motor(fr_motor), bl_motor(bl_motor), br_motor(br_motor), pole_poti(pole_poti), enc_l(enc_l), enc_r(enc_r) {
  state_names[STATE_DRIVER_CONTROL] = "Driver Control";
}

/* Public Functions */
void Drive::update() {
  switch(this->state) {
    case STATE_RESET:
      this->set_power(0);
      break;
    case STATE_DISABLED:
      break;
    case STATE_DRIVER_CONTROL:
      this->set_power(x, y, a);
      break;
    case STATE_AUTO_CONTROL:
      break;
  }
}

void Drive::driver_set(int8_t x, int8_t y, int8_t a) {
  if (x) this->x = x;
  else this->x = 0;

  if (y) this->y = y;
  else this->y = 0;

  if (a) this->a = a;
  else this->a = 0;

  if ((x || y || a) && (this->state == STATE_AUTO_CONTROL || this->state == STATE_DISABLED)) this->enable();
}

void Drive::move(double dist_target, uint8_t max_power, bool brake, double angle_target) {
    this->set_state(STATE_AUTO_CONTROL);
    // If the correct angle is default, the correct angle should be the starting angle
    double angle_start = this->get_global_angle();
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
    double enc_l_start = this->enc_l.get_value();
    double enc_r_start = this->enc_r.get_value();
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
      delta_enc_l = (this->enc_l.get_value() - enc_l_start);
      delta_enc_r = (this->enc_r.get_value() - enc_r_start);
      dist_current = (delta_enc_l * SPN_TO_IN_L + delta_enc_r * SPN_TO_IN_R) / 2.0;
      angle_current = this->get_global_angle();

      // Calculate distance and angle errors
      dist_error = dist_target - dist_current;
      angle_error = angle_target - angle_current;
      this->error = dist_error;

      // Ramp up while the robot hasn't moved 8 inches and is not within 2 inches of it's target
      // if (abs(dist_current) < 8_in && dist_error > 2_in) {
      if (fabs(dist_current) < fabs(dist_target) * 0.30) {
        power += 0.40 * sgn(dist_target);
        // log_ln(LOG_AUTO, "%d Ramping up...", pros::millis(), power, dist_current, dist_error, angle_current, angle_error);
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
        }

        // Calculate powers
        power = dist_p_val + dist_i_val + dist_d_val;
        angle_power = angle_p_val + angle_i_val + angle_d_val;
        // log_ln(LOG_AUTO, "%d Distance | Current: %f in, Error: %f in, Power: %f, P: %f, I: %f, D: %f", pros::millis(), dist_current, dist_error, power, dist_p_val, dist_i_val, dist_d_val);
        // log_ln(LOG_AUTO, "%d Angle    | Current: %f deg, Error: %f deg, Angle Power: %f, P: %f, I: %f, D: %f", pros::millis(), RAD_TO_DEG(angle_current), RAD_TO_DEG(angle_error), angle_power, angle_p_val, angle_i_val, angle_d_val);
        // log_ln(LOG_AUTO, "----------------------------------------------------------------");

        set_power(0, power, angle_power);

        dist_last_error = dist_error;
        angle_last_error = angle_error;
        pros::delay(1);
    } while (fabs(dist_error) > 0.3_in && sgn(dist_error) == sgn(dist_last_error));

    if (brake) {
      double targetFL = this->fl_motor.get_position() + (dist_error) * (this->TPR / (this->WHEEL_DIAMETER * M_PI));
      double targetBL = this->bl_motor.get_position() + (dist_error) * (this->TPR / (this->WHEEL_DIAMETER * M_PI));
      double targetFR = this->fr_motor.get_position() + (dist_error) * (this->TPR / (this->WHEEL_DIAMETER * M_PI));
      double targetBR = this->br_motor.get_position() + (dist_error) * (this->TPR / (this->WHEEL_DIAMETER * M_PI));

      this->fl_motor.move_absolute(targetFL, 25);
      this->bl_motor.move_absolute(targetBL, 25);
      this->fr_motor.move_absolute(targetFR, 25);
      this->br_motor.move_absolute(targetBR, 25);
      log_ln(LOG_AUTO, "%d Stopping from FL: %f, BL: %f, FR: %f, BR %f", pros::millis(), this->fl_motor.get_position(), this->bl_motor.get_position(), this->fr_motor.get_position(), this->br_motor.get_position());
      while (fabs(this->fl_motor.get_position() - targetFL) > 3 || fabs(this->bl_motor.get_position() - targetBL) > 3 || fabs(this->fr_motor.get_position() - targetFR) > 3 || fabs(this->br_motor.get_position() - targetBR) > 3) pros::delay(1);
      pros::delay(100);
    } else set_vel(0, 0, 0);

    dist_current = ((this->enc_l.get_value() - enc_l_start) * SPN_TO_IN_L + (this->enc_r.get_value() - enc_r_start) * SPN_TO_IN_R) / 2.0;
    angle_current = get_global_angle();

    dist_error = dist_target - dist_current;
    angle_error = angle_target - angle_current;

    log_ln(LOG_AUTO, "%d FINISHED MOVE >>>> Took %d ms, Ended At: %f, Distance Error: %f, Angle Error: %f", pros::millis(), pros::millis() - start_time, dist_current, dist_error, RAD_TO_DEG(angle_error));
    this->set_state(STATE_DRIVER_CONTROL);
}

void Drive::turn(const AngleTarget& target) {
  this->set_state(STATE_AUTO_CONTROL);
  double dA = target.getTarget() - this->get_global_angle();
  double fixeddA = dA;
  double drive_volt = 0;
  double kP = 200/50;
  double kI = 0.02;
  double kD = 70;
  double iVal = 0;
  double last_error = 0;
  double dVal = 0;

  while(fabs(dA) > fabs((0.4 * fixeddA))) {
    dA = target.getTarget() - this->get_global_angle();
    this->error = dA;
    drive_volt = RAD_TO_DEG(dA) * kP;
    this->set_power(0, 0, drive_volt);
    pros::delay(1);
    //printf("hello world %f and %f\n", target.getTarget(), getGlobalAngle());
  }

  kP = 200/100;

  while(fabs(dA) > 0.5_deg) {
    if (fabs(dA) < 5_deg) iVal += RAD_TO_DEG(dA)*kI;
    else iVal = 0;
    dA = target.getTarget() - this->get_global_angle();
    this->error = dA;
    drive_volt = RAD_TO_DEG(dA) * kP + iVal;
    this->set_power(0, 0, drive_volt);
    // printf("%d Drive voltage is: %f and iVal is %f dA is %f\n", pros::millis(),drive_volt, iVal, RAD_TO_DEG(dA));
    pros::delay(1);
  }
  this->set_power(0);
  this->brake();
  printf("End angle is %f\n", RAD_TO_DEG(this->get_global_angle()));
  this->set_state(STATE_DRIVER_CONTROL);
}

void Drive::flatten_against_wall(bool forward, bool hold, uint8_t hold_power) {
  this->set_state(STATE_AUTO_CONTROL);
  bool right_done = false;
  bool left_done = false;

	if (forward) {
		//log_ln("%d FW Start", pros::millis());
		this->set_power(0, 40, 0);
		pros::delay(200);
		do {
			//log_ln("%d Reset Back Up(%f, %f, %f) Vel(%f, %f, %f) VeelLoc(%f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.aVel, pos.velLocal.x, pos.velLocal.y);
			pros::delay(5);
		} while (fabs(this->bl_motor.get_actual_velocity()) > 2); //aVel < -0.1);
		if (hold) this->set_power(0, hold_power, 0);
		else this->set_power(0);
	} else {
		//log_ln("%d FW Start", pros::millis());
		this->set_power(0,-60, 0);
		pros::delay(200);
		do {
			//log_ln("%d Reset Back Up(%f, %f, %f) Vel(%f, %f, %f) VeelLoc(%f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.aVel, pos.velLocal.x, pos.velLocal.y);
      // if (abs(drive_fl.get_actual_velocity()) < abs(drive_fr.get_actual_velocity()) - 5) setDrive(0, -60, 7);
      // else if (abs(drive_fr.get_actual_velocity()) < abs(drive_fl.get_actual_velocity()) - 5) setDrive(0, -60, -7);
      // else if (abs(drive_fl.get_actual_velocity()) < abs(drive_fr.get_actual_velocity()) - 2) setDrive(0, -60, 12);
      // else if (abs(drive_fr.get_actual_velocity()) < abs(drive_fl.get_actual_velocity()) - 2) setDrive(0, -60, -12);
      // else setDrive(0, -60, 0);
      if (fabs(this->fl_motor.get_actual_velocity()) < 1) left_done = true;
      if (fabs(this->fr_motor.get_actual_velocity()) < 1) right_done = true;
      if (left_done && !right_done) this->set_side_power(0, -60);
      else if (!left_done && right_done) this->set_side_power(-60, 0);
      else if (left_done && right_done) break;
			pros::delay(10);
		} while (true); //aVel < -0.1);
		if (hold) this->set_power(0, -hold_power, 0);
		else this->set_power(0);
	}
	log_ln(LOG_AUTO, "%d Done flatten_against_wall", pros::millis());
  this->set_state(STATE_AUTO_CONTROL);
}
