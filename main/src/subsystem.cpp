#include "subsystem.hpp"

Subsystem::Subsystem() {
  state_names[STATE_IDLE] = "Idle";
  state_names[STATE_RESET] = "Reset";
}

void Subsystem::change_state(uint8_t new_state) {
  last_state = state;
  state = new_state;
  state_change_time = pros::millis();

  log_ln(LOG_STATES, "Switching %s to %s state from %s state", subsystem_name.c_str(), (state_names[state]).c_str(), (state_names[last_state]).c_str());
}

void Subsystem::reset() {
  change_state(STATE_RESET);
}

void Subsystem::disable() {
  change_state(STATE_IDLE);
}

double Subsystem::get_position() {
  return this->position;
}

double Subsystem::get_target() {
  return this->target;
}

double Subsystem::get_error() {
  return this->error;
}

double Subsystem::get_power() {
  return this->power;
}

double Subsystem::get_velocity() {
  return this->velocity;
}

void Subsystem::operator= (uint8_t new_state) {
  change_state(new_state);
}
