#include "angler.hpp"

/* Constructor */
Angler::Angler(std::string subsystem_name, pros::Motor& angler_motor) : angler_motor(angler_motor) {
  this->subsystem_name = subsystem_name;
  state_names[STATE_MOVE_MANUAL] = "Move Manual";
  state_names[STATE_MOVE_POS] = "Move To Pos";
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
      set_timeouts();
      break;
    case STATE_DISABLED:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_MOVE_MANUAL:
      this->power = set_dz(ctrler.get_analog(JOY_ANGLER), ANGLER_DZ);
      set_timeouts(1500, 0);
      break;
    case STATE_MOVE_POS:
      set_timeouts();
      break;
    case STATE_HOLD:
      this->power = 0;
      this->target = angler_motor.get_position();
      set_timeouts(350);
      break;
  }
}

void Angler::update() {
  this->velocity = angler_motor.get_actual_velocity();
  this->position = angler_motor.get_position();
  angler_motor.move(this->power);

  switch(this->state) {
    case STATE_IDLE:
      break;
    case STATE_RESET:
      this->set_state(STATE_IDLE);
      break;
    case STATE_DISABLED:
      break;
    case STATE_PICKUP:
      if (check_single_press(BTN_INTAKE_UP) || check_single_press(BTN_INTAKE_DOWN)) this->set_state(STATE_IDLE);
      else if (timed_out() && below_vel_threshold()) set_state(STATE_JAM);
      break;
    case STATE_CAP_FLIP:
      if (check_single_press(BTN_INTAKE_UP) || check_single_press(BTN_INTAKE_DOWN)) this->set_state(STATE_IDLE);
      break;
    case STATE_JAM:
      if (timed_out()) this->set_state(STATE_PICKUP);
      break;
  }
}

void Angler::enable() {
  this->set_state(STATE_IDLE);
}
