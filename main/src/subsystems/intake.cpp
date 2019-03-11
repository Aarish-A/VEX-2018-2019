#include "intake.hpp"

/* Private Functions */
void Intake::set_power(int8_t power) {
  this->intake_motor.move(power);
}

void Intake::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_DISABLED:
      set_power(0);
      break;
    case STATE_RESET:
      this->set_state(STATE_OFF);
      break;
    case STATE_OFF:
      set_power(0);
      break;
    case STATE_FORWARDS:
      set_power(this->power);
      break;
    case STATE_BACKWARDS:
      set_power(this->power);
      break;
    case STATE_JAM:
      set_power(-80);
      break;
  }
}

/* Constructor */
Intake::Intake(std::string subsystem_name, uint8_t default_state, pros::Motor& intake_motor) : Subsystem(subsystem_name, default_state), intake_motor(intake_motor) {
  state_names[STATE_OFF] = "Off";
  state_names[STATE_FORWARDS] = "Forwards";
  state_names[STATE_BACKWARDS] = "Backwards";
  state_names[STATE_JAM] = "Jam";
}

/* Public Functions */
void Intake::update() {
  this->velocity = intake_motor.get_actual_velocity();
  this->position = intake_motor.get_position();

  switch(this->state) {
    case STATE_DISABLED:
      break;
    case STATE_RESET:
      break;
    case STATE_OFF:
      break;
    case STATE_FORWARDS:
      if (below_vel_threshold(20, 200)) this->set_state(STATE_JAM);
      break;
    case STATE_BACKWARDS:
      break;
    case STATE_JAM:
      if (timed_out(450)) this->set_state(STATE_FORWARDS);
      else if (below_vel_threshold(10, 300)) this->set_state(STATE_OFF);
      break;
  }

  this->last_velocity = this->velocity;
}

void Intake::intake(int8_t power) {
  this->power = power;
  this->set_state(STATE_FORWARDS);
}

void Intake::outtake(int8_t power) {
  this->power = power;
  this->set_state(STATE_BACKWARDS);
}

void Intake::stop() {
  this->set_state(STATE_OFF);
}

bool Intake::off() {
  return this->state == STATE_OFF;
}
