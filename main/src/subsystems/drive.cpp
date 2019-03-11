#include "drive.hpp"

/* Constructor */
Drive::Drive(std::string subsystem_name, uint8_t default_state, pros::Motor& fl_motor, pros::Motor& fr_motor, pros::Motor& bl_motor, pros::Motor& br_motor) : Subsystem(subsystem_name, default_state), fl_motor(fl_motor), fr_motor(fr_motor), bl_motor(bl_motor), br_motor(br_motor) {
  state_names[STATE_DRIVER_CONTROL] = "Driver Control";
}

/* Private Functions */
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

  if ((x || y || a) && this->state == STATE_AUTO_CONTROL) this->enable();
}
