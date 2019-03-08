#include "intake.hpp"

/* Constructor */
Intake::Intake(std::string subsystem_name, pros::Motor& intake_motor) : intake_motor(intake_motor) {
  this->subsystem_name = subsystem_name;
  state_names[STATE_PICKUP] = "Pickup";
  state_names[STATE_CAP_FLIP] = "Cap Flip";
  state_names[STATE_JAM] = "Jam";
}

/* Private Functions */


/* Public Functions */
void Intake::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_IDLE:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_RESET:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_DISABLED:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_PICKUP:
      this->power = 127;
      set_timeouts(250, 10);
      break;
    case STATE_CAP_FLIP:
      this->power = -80;
      set_timeouts();
      break;
    case STATE_JAM:
      this->power = -80;
      set_timeouts(350);
      break;
  }
}

void Intake::update() {
  this->velocity = intake_motor.get_actual_velocity();
  this->position = intake_motor.get_position();
  intake_motor.move(this->power);

  switch(this->state) {
    case STATE_IDLE:
      if (check_single_press(BTN_INTAKE_UP)) this->set_state(STATE_PICKUP);
      else if (check_single_press(BTN_INTAKE_DOWN)) this->set_state(STATE_CAP_FLIP);
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

void Intake::enable() {
  this->set_state(STATE_IDLE);
}
