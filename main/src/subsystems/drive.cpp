#include "drive.hpp"

int8_t Drive::turn_curve[256];
double Drive::drive_turn_coefficient = 3.50;

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
      this->x = 0;
      this->y = 0;
      this->a = 0;
      break;
    case STATE_DRIVE_LOCK:
      this->x = 0;
      this->y = 0;
      this->a = 0;
      this->fr_motor.tare_position();
      this->fl_motor.tare_position();
      this->br_motor.tare_position();
      this->bl_motor.tare_position();
      this->fl_motor.move_absolute(0,200);
      this->bl_motor.move_absolute(0,200);
      this->fr_motor.move_absolute(0,200);
      this->br_motor.move_absolute(0,200);
      // this->fl_motor.move(-30);
      // this->bl_motor.move(30);
      // this->fr_motor.move(-30);
      // this->br_motor.move(30);
      break;
    case STATE_TURN_BRAKE:
      this->set_power(0, 0, sgn(this->last_a) * -1 * this->DRIVE_TURN_BRAKE_POWER);
      break;
  }
}

void Drive::brake() {
  set_power(0);
  this->fl_motor.move_relative(0, 30);
  this->bl_motor.move_relative(0, 30);
  this->fr_motor.move_relative(0, 30);
  this->br_motor.move_relative(0, 30);
}

double Drive::get_global_angle() {
  return (this->enc_l.get_value() * SPN_TO_IN_L - this->enc_r.get_value() * SPN_TO_IN_R) / (WHL_DIS_L + WHL_DIS_R);
}

/* Constructor */
Drive::Drive(std::string subsystem_name, uint8_t default_state, pros::Motor& fl_motor, pros::Motor& fr_motor, pros::Motor& bl_motor, pros::Motor& br_motor, pros::ADIAnalogIn& pole_poti, pros::ADIEncoder& enc_l, pros::ADIEncoder& enc_r) : Subsystem(subsystem_name, default_state), fl_motor(fl_motor), fr_motor(fr_motor), bl_motor(bl_motor), br_motor(br_motor), pole_poti(pole_poti), enc_l(enc_l), enc_r(enc_r) {
  state_names[STATE_DRIVER_CONTROL] = "Driver Control";
  state_names[STATE_AUTO_CONTROL] = "Auto Control";
  state_names[STATE_DRIVE_LOCK] = "Drive Lock";
  state_names[STATE_TURN_BRAKE] = "Drive Brake";


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
      this->set_power(this->x, this->y, this->a);
      // if (this->x == 0 && this->y == 0 && this->a == 0 && pros::millis() - this->above_turn_brake_threshold < 400) {
      //   this->disable_state_change_log();
      //   this->set_state(STATE_TURN_BRAKE);
      // }
      break;
    case STATE_AUTO_CONTROL:
      if (this->x || this->a || this->y) {
        printf("Drive move interrupted by driver\n");
        this->set_state(STATE_DRIVER_CONTROL);
      }
      break;
    case STATE_DRIVE_LOCK:
      if (this->x || this->a || this->y) {
        printf("Drive lock interrupted by driver\n");
        this->set_state(STATE_DRIVER_CONTROL);
      } else if (timed_out(1000)) {
        this->set_state(STATE_DRIVER_CONTROL);
      } else if (pros::millis() - this->state_change_time > 200) {
        // this->fl_motor.move(-25);
        // this->bl_motor.move(25);
        // this->fr_motor.move(-25);
        // this->br_motor.move(25);
      }
      break;
    case STATE_TURN_BRAKE:
      this->disable_state_change_log();
      if (pros::millis() - this->state_change_time > 100) this->set_state(STATE_DRIVER_CONTROL);
      else if (this->x || this->a || this->y) this->set_state(STATE_DRIVER_CONTROL);
      break;
  }
  if (abs(this->a) > 20) this->above_turn_brake_threshold = pros::millis();
  this->last_a = this->a;
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

void Drive::flatten_against_wall(bool forward, bool hold, int8_t hold_power) {
  flatten_against_wall_base(forward, forward? 40 : 60, hold, hold_power);
}

void Drive::flatten_against_wall_base(bool forward, int8_t power, bool hold, int8_t hold_power) {
  power = abs(power);
  hold_power = abs(hold_power);
  this->set_state(STATE_AUTO_CONTROL);
  bool right_done = false;
  bool left_done = false;

  if (forward) {
    this->set_power(0, power, 0);
    pros::delay(200);
    do {
      pros::delay(2);
    } while (fabs(this->br_motor.get_actual_velocity()) > 1);
    if (hold) this->set_power(0, hold_power, 0);
  } else {
    this->set_power(0, -power, 0);
    pros::delay(200);
    do {
      if (fabs(this->fl_motor.get_actual_velocity()) < 2) left_done = true;
      if (fabs(this->fr_motor.get_actual_velocity()) < 2) right_done = true;

      if (left_done && !right_done) this->set_side_power(-15, -power);
      else if (!left_done && right_done) this->set_side_power(-power, -15);
      else if (left_done && right_done) break;
      pros::delay(2);
    } while(true);
    if (hold) this->set_power(0, -power, 0);
    else this->set_power(0);
  }
  log_ln(MOVE, AUTO, "Done flatten against wall");
}

void Drive::align_with_pole(uint16_t poti_zero) {
  //2820
  uint8_t left = -1;
  uint8_t deadzone = 80;
  this->flatten_against_wall(false, true);

  int16_t poti_val = this->pole_poti.get_value();
  int16_t poti_val_last = poti_val;

  if (poti_val > poti_zero + deadzone)
  {
    this->set_power(55, -15, 0);
    left = -1;
  }
  else if (poti_val < poti_zero - deadzone)
  {
    this->set_power(-55, -15, 0);
    left = 1;
  }

  while (abs(poti_val - poti_zero) > deadzone) {
    poti_val_last = poti_val;
    poti_val = this->pole_poti.get_value();
    pros::delay(3);
  }
  this->set_power(15*sgn(left),0,0);
  this->flatten_against_wall(false, true);
  this->reset_global_angle();
  printf("err: %d", this->pole_poti.get_value() - poti_zero);
  this->set_state(STATE_DRIVER_CONTROL);
}

bool Drive::moving() {
  return this->state == STATE_AUTO_CONTROL;
}

void Drive::set_error(double error) {
  this->error = error;
}

void Drive::set_target(double target) {
  this->target = target;
}

void Drive::wait_for_stop() {
  while (this->state == STATE_AUTO_CONTROL) pros::delay(2);
}

void Drive::wait_for_distance(double target_distance) {
  bool forwards;
  target_distance > 0 ? forwards = true : forwards = false;
  if (forwards) while (this->target - this->error < target_distance) pros::delay(2);
  else while(this->target - this->error > target_distance) pros::delay(2);
}

void Drive::wait_for_angle(double target_angle) {
  bool clockwise = (this->target - this->get_global_angle() > 0 ? true : false);
  if (clockwise) while (this->get_global_angle() < target_angle) pros::delay(1);
  else while (this->get_global_angle() > target_angle) pros::delay(1);
}

void Drive::lock() {
  this->set_state(STATE_DRIVE_LOCK);
}

void Drive::unlock() {
  if (this->state == STATE_DRIVE_LOCK) this->set_state(STATE_DRIVER_CONTROL);
}
