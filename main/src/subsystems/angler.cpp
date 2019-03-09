#include "angler.hpp"

/* Constructor */
Angler::Angler(std::string subsystem_name, pros::Motor& angler_motor) : angler_motor(angler_motor) {
  this->subsystem_name = subsystem_name;
  state_names[STATE_MOVE_MANUAL] = "Move Manual";
  state_names[STATE_HOLD] = "Hold";
}

/* Private Functions */

/* Public Functions */
void Angler::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_IDLE:
      break;
    case STATE_RESET:
      this->power = -30;
      this->angler_motor.move(this->power);
      set_timeout_time(1000);
      set_timeout_velocity(10, 100);
      break;
    case STATE_DISABLED:
      break;
    case STATE_MOVE_MANUAL:
      break;
    case STATE_MOVE_POS:
      break;
    case STATE_HOLD:
      break;
  }
}

void Angler::update() {
  this->velocity = angler_motor.get_actual_velocity();
  this->position = angler_motor.get_position();

  switch(this->state) {
    case STATE_IDLE:
      break;
    case STATE_RESET:
      log_ln(LOG_STATES, "here %f", this->velocity);
      if (this->below_vel_threshold()) {
        this->angler_motor.tare_position();
        this->set_state(STATE_MOVE_MANUAL);
      } else if (this->timed_out()) this->set_state(STATE_IDLE);
      break;
    case STATE_DISABLED:
      break;
    case STATE_MOVE_MANUAL:
      this->power = set_dz(ctrler.get_analog(JOY_ANGLER), this->DEADZONE);
      this->angler_motor.move(this->power);
      break;
    case STATE_MOVE_POS:
      //if (lst_joy && !joy) set_state(STATE_HOLD);
      break;
    case STATE_HOLD:
      angler_motor.move(target);
      set_state(STATE_IDLE);
      break;
  }
  this->last_velocity = this->velocity;
}

void Angler::enable() {
  this->set_state(STATE_MOVE_MANUAL);
}
