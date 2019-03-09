#include "angler.hpp"

/* Constructor */
Angler::Angler(std::string subsystem_name, pros::Motor& angler_motor) : angler_motor(angler_motor) {
  this->subsystem_name = subsystem_name;
  this->state_names[STATE_DRIVER_CONTROL] = "Driver Control";
  this->state_names[STATE_AUTO_CONTROL] = "Auto Control";
  this->state_names[STATE_HOLD] = "Hold";
}

/* Private Functions */
void Angler::set_angler_motor_power(double power) {
  this->power = power;
  this->angler_motor.move(this->power);
}

/* Public Functions */
void Angler::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_IDLE:
      this->set_angler_motor_power(0);
      break;
    case STATE_RESET:
      this->set_angler_motor_power(-45);
      break;
    case STATE_DISABLED:
      this->set_angler_motor_power(0);
      break;
    case STATE_DRIVER_CONTROL:
      this->set_angler_motor_power(0);
      break;
    case STATE_AUTO_CONTROL:
      this->angler_motor.move_absolute(this->target, 200);
      break;
    case STATE_HOLD:
      this->target = this->position;
      this->angler_motor.move_relative(0, 100);
      // this->angler_motor.move_absolute(this->target, 50);
      break;
  }
}

void Angler::update() {
  this->velocity = angler_motor.get_actual_velocity();
  this->position = angler_motor.get_position();
  this->error = this->target - this->position;
  int joystick = set_scaled_dz(ctrler.get_analog(JOY_ANGLER), this->DEADZONE);

  switch(this->state) {
    case STATE_IDLE:
      break;
    case STATE_RESET:
      if (this->below_vel_threshold(10, 200)) {
        this->angler_motor.tare_position();
        this->set_state(STATE_DRIVER_CONTROL);
      } else if (this->timed_out(1500)) {
        this->set_state(STATE_DISABLED);
      }
      break;
    case STATE_DISABLED:
      break;
    case STATE_DRIVER_CONTROL:
      if ((joystick > 0 && this->position < this->TOP_LIMIT_POSITION) || (joystick < 0 && this->position > this->BOTTOM_LIMIT_POSITION)) this->set_angler_motor_power(joystick);
      else this->set_state(STATE_HOLD);
      break;
    case STATE_AUTO_CONTROL:
      if (fabs(this->error) < 5) {
        log_ln(LOG_STATES, "Angler move finished at %f, target was %f", this->position, this->target);
        this->set_state(STATE_HOLD);
      } else if (joystick) {
        log_ln(LOG_STATES, "Angler move interrupted by driver");
        this->set_state(STATE_DRIVER_CONTROL);
      }
      break;
    case STATE_HOLD:
      if (joystick) this->set_state(STATE_DRIVER_CONTROL);
      break;
  }

  this->last_velocity = this->velocity;
}

void Angler::set_target(double target) {
  this->target = target;
  this->set_state(STATE_AUTO_CONTROL);
  log_ln(LOG_SUBSYSTEMS, "Moving Angler to %f, started at %f", this->target, this->position);
}

void Angler::enable() {
  this->set_state(STATE_DRIVER_CONTROL);
}
