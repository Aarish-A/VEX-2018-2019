#include "angler.hpp"

/* Constructor */
Angler::Angler(std::string subsystem_name, uint8_t default_state, pros::Motor& angler_motor) : Subsystem(subsystem_name, default_state), angler_motor(angler_motor) {
  this->state_names[STATE_DRIVER_CONTROL] = "Driver Control";
  this->state_names[STATE_AUTO_CONTROL] = "Auto Control";
  this->state_names[STATE_HOLD] = "Hold";
}

/* Private Functions */
void Angler::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_DISABLED:
      this->angler_motor.move(0);
      break;
    case STATE_RESET:
      this->angler_motor.move(-45);
      break;
    case STATE_DRIVER_CONTROL:
      break;
    case STATE_AUTO_CONTROL:
      this->angler_motor.move_absolute(this->target, 200);
      break;
    case STATE_HOLD:
      this->target = this->position;
      // this->angler_motor.move_relative(0, 100);
      this->angler_motor.move_absolute(this->target, 200);
      break;
  }
}

/* Public Functions */
void Angler::update() {
  this->velocity = angler_motor.get_actual_velocity();
  this->position = angler_motor.get_position();
  this->error = this->target - this->position;

  switch(this->state) {
    case STATE_DISABLED:
      break;
    case STATE_RESET:
      if (this->below_vel_threshold(10, 200)) {
        this->angler_motor.tare_position();
        this->set_state(STATE_DRIVER_CONTROL);
      } else if (this->timed_out(1500)) {
        this->set_state(STATE_DISABLED);
      }
      break;
    case STATE_DRIVER_CONTROL:
      if (this->power) this->angler_motor.move(this->power);
      else this->set_state(STATE_HOLD);
      break;
    case STATE_AUTO_CONTROL:
      if (timed_out(this->move_timeout) || below_vel_threshold(2, 400)) this->disable();
      else if (this->power) {
        log_ln(LOG_STATES, "Angler move interrupted by driver");
        this->set_state(STATE_DRIVER_CONTROL);
      } else if (fabs(this->error) < this->error_threshold) {
        log_ln(LOG_STATES, "Angler move finished at %f, target was %f", this->position, this->target);
        this->set_state(STATE_HOLD);
      }
      break;
    case STATE_HOLD:
      if (this->power) this->set_state(STATE_DRIVER_CONTROL);
      break;
  }

  this->last_velocity = this->velocity;
}

void Angler::driver_set(int8_t power) {
  if (power) {
    this->power = power;
    if (this->disabled() && last_state != STATE_RESET) this->enable();
  }
  else this->power = 0;
}

void Angler::move_to(double target, uint32_t move_timeout, uint8_t error_threshold) {
  this->target = target;
  this->move_timeout = move_timeout;
  this->error_threshold = error_threshold;
  this->set_state(STATE_AUTO_CONTROL);
  log_ln(LOG_SUBSYSTEMS, "Moving Angler to %f, started at %f", this->target, this->position);
}

bool Angler::moving_to_target() {
  return state == STATE_AUTO_CONTROL;
}
