#include "angler.hpp"
using namespace pros;

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
      this->power = 0;
      set_timeouts();
      break;
    case STATE_RESET:
      this->power = -30;
      set_timeouts(1000, 0);
      break;
    case STATE_DISABLED:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_MOVE_MANUAL:
      this->power = set_dz(ctrler.get_analog(JOY_ANGLER), ANGLER_DZ);
      set_timeouts();
      break;
    case STATE_MOVE_POS:
      set_timeouts();
      break;
    case STATE_HOLD:
      this->power = 0;
      this->target = angler_motor.get_position();
      set_timeouts();
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
      if (velocity > 0 && millis() > state_change_time+100) {
        angler_motor.tare_position();
      }
      break;
    case STATE_DISABLED:
      break;
    case STATE_MOVE_MANUAL:
      angler_motor.move(this->power);
      break;
    case STATE_MOVE_POS:
      //if (lst_joy && !joy) set_state(STATE_HOLD);
      break;
    case STATE_HOLD:
      angler_motor.move(target);
      set_state(STATE_IDLE);
      break;
  }
}

void Angler::enable() {
  this->set_state(STATE_IDLE);
}
