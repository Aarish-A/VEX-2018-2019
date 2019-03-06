#include "subsystem.hpp"

void Subsystem::change_state(State new_state) {
  last_state = state;
  state = new_state;
  state_change_time = pros::millis();
}

void Subsystem::reset() {
  change_state(state_reset);
}

void Subsystem::disable() {
  change_state(state_idle);
}

double Subsystem::get_current() {
  return this->current;
}

double Subsystem::get_target() {
  return this->target;
}

double Subsystem::get_error() {
  return this->error;
}

void Subsystem::operator= (State new_state) {
  change_state(new_state);
}
