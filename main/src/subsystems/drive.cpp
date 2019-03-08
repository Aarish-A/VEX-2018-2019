#include "drive.hpp"

/* Constructor */
Drive::Drive(std::string subsystem_name, pros::Motor& fl_motor, pros::Motor& fr_motor, pros::Motor& bl_motor, pros::Motor& br_motor) : fl_motor(fl_motor), fr_motor(fr_motor), bl_motor(bl_motor), br_motor(br_motor) {
  this->subsystem_name = subsystem_name;
  state_names[STATE_DRIVER_CONTROL] = "Driver Control";
}

/* Private Functions */
void Drive::change_state(uint8_t new_state) {
  Subsystem::change_state(new_state);
  switch(this->state) {
    case STATE_DRIVER_CONTROL:
      break;
    case STATE_DISABLED:
      this->state_timeout_velocity = 1;
      this->state_timeout_time = 1000;
      break;
  }
}

void Drive::set(double x, double y, double a) {
  this->fl_motor.move(y + x + a);
  this->bl_motor.move(y - x + a);
  this->fr_motor.move(y - x - a);
  this->br_motor.move(y + x - a);
}

void Drive::set(double power) {
  this->set(power, power, power);
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

void Drive::set_side(double left, double right) {
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
    case STATE_IDLE:
      this->set(0);
      break;
    case STATE_RESET:
      this->change_state(STATE_DRIVER_CONTROL);
      break;
    case STATE_DISABLED:
      // NEEDS TO KILL ANY AUTO MOVEMENT TASK RUNNING AT THIS POINT HOW DO ??
      break;
    case STATE_DRIVER_CONTROL:
      int strafe = set_dz(ctrler.get_analog(JOY_DRIVE_STRAFE), this->STRAFE_DEADZONE);
      int throttle = set_dz(ctrler.get_analog(JOY_DRIVE_FW), this->THROTTLE_DEADZONE);
      int turn = set_scaled_dz(ctrler.get_analog(JOY_DRIVE_TURN), this->TURN_DEADZONE);
      this->set(strafe, throttle, turn);
      break;
  }
}

void Drive::enable() {
  this->change_state(STATE_DRIVER_CONTROL);
}
