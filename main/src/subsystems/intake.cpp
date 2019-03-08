#include "intake.hpp"

/* Constructor */
Intake::Intake(std::string subsystem_name, pros::Motor& intake_motor) : intake_motor(intake_motor) {
  this->subsystem_name = subsystem_name;
  state_names[STATE_PICKUP] = "Pickup";
  state_names[STATE_CAP_FLIP] = "Cap Flip";
  state_names[STATE_JAM] = "Jam";
}

/* Private Functions */
void Intake::change_state(uint8_t new_state) {
  Subsystem::change_state(new_state);
  switch(new_state) {
    case STATE_IDLE:
      this->power = 0;
      break;
    case STATE_RESET:
      this->power = 0;
      break;
    case STATE_DISABLED:
      this->power = 0;
      break;
    case STATE_PICKUP:
      this->power = 127;
      this->state_timeout_velocity = 10;
      this->state_timeout_time = 250;
      break;
    case STATE_CAP_FLIP:
      this->power = -80;
      break;
    case STATE_JAM:
      this->power = -80;
      this->state_timeout_time = 350;
      break;
  }
  intake_motor.move(power);
}

/* Public Functions */
void Intake::update() {
  this->velocity = intake_motor.get_actual_velocity();
  this->position = intake_motor.get_position();

  switch(this->state) {
    case STATE_IDLE:
      if (check_single_press(BTN_INTAKE_UP)) this->change_state(STATE_PICKUP);
      else if (check_single_press(BTN_INTAKE_DOWN)) this->change_state(STATE_CAP_FLIP);
      break;
    case STATE_RESET:
      this->change_state(STATE_IDLE);
      break;
    case STATE_DISABLED:
      break;
    case STATE_PICKUP:
      if (check_single_press(BTN_INTAKE_UP) || check_single_press(BTN_INTAKE_DOWN)) this->change_state(STATE_IDLE);
      else if (timed_out() && below_vel_threshold()) change_state(STATE_JAM);
      break;
    case STATE_CAP_FLIP:
      if (check_single_press(BTN_INTAKE_UP) || check_single_press(BTN_INTAKE_DOWN)) this->change_state(STATE_IDLE);
      break;
    case STATE_JAM:
      if (timed_out()) this->change_state(STATE_PICKUP);
      break;
  }
}

void Intake::enable() {
  this->change_state(STATE_IDLE);
}

void Intake::set_power(double power) {
  this->power = power;
  if (power > 0) change_state(STATE_PICKUP);
  else if (power < 0) change_state(STATE_CAP_FLIP);
  else if (power == 0) change_state(STATE_IDLE);
}
