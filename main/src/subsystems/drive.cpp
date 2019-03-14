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

void Drive::reset_global_angle() {
  this->enc_l.reset();
  this->enc_r.reset();
}

void Drive::flatten_against_wall(bool forward, bool hold, uint8_t hold_power) {
  bool right_done = false;
  bool left_done = false;

  if (forward) {
    this->set_power(0, 40, 0);
    pros::delay(200);
    do {
      pros::delay(2);
    } while (fabs(this->bl_motor.get_actual_velocity()) > 2);
    if (hold) this->set_power(0, hold_power, 0);
  } else {
    this->set_power(0, -60, 0);
    pros::delay(200);
    do {
      if (fabs(this->fl_motor.get_actual_velocity()) < 1) left_done = true;
      if (fabs(this->fr_motor.get_actual_velocity()) < 1) right_done = true;

      if (left_done && !right_done) this->set_side_power(-15, -60);
      else if (!left_done && right_done) this->set_side_power(-60, -15);
      else if (left_done && right_done) break;
      pros::delay(2);
    } while(true);
    if (hold) this->set_power(0, -hold_power, 0);
    else this->set_power(0);
  }
  log_ln(LOG_AUTO, "Done flatten against wall");
}

void Drive::align_with_pole(uint16_t poti_zero) {
  this->flatten_against_wall(false, true);
  if (this->pole_poti.get_value() > poti_zero + 80) this->set_power(55, -5, 0);
  else if (this->pole_poti.get_value() < poti_zero - 80) this->set_power(-55, -5, 0);
  while (abs(this->pole_poti.get_value() - poti_zero) > 125) pros::delay(5);
  this->flatten_against_wall(false, true);
}

bool Drive::moving() {
  return this->state == STATE_AUTO_CONTROL;
}

void Drive::set_error(double error) {
  this->error = error;
}
