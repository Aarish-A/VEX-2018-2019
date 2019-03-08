#include "subsystem.hpp"

/* Protected Non-Virtual Functions */
void Subsystem::set_timeouts(uint32_t time, double velocity) {
  this->state_timeout_time = time;
  this->state_timeout_velocity = velocity;
}

bool Subsystem::timed_out() {
  return (pros::millis() - this->state_change_time > this->state_timeout_time && this->state_timeout_time != 0);
}

bool Subsystem::above_vel_threshold() {
  return (this->velocity > this->state_timeout_velocity && this->state_timeout_velocity != 0);
}

bool Subsystem::below_vel_threshold() {
  return (this->velocity < this->state_timeout_velocity && this->state_timeout_velocity != 0);
}

/* Protected Virtual Functions */
void Subsystem::change_state(uint8_t new_state) {
  this->last_state = state;
  this->state = new_state;
  this->state_change_time = pros::millis();
  log_ln(LOG_STATES, "Switching %s to %s state from %s state", (this->subsystem_name).c_str(), (this->state_names[state]).c_str(), (this->state_names[last_state]).c_str());
}

/* Constructor */
Subsystem::Subsystem() {
  this->state_names[STATE_IDLE] = "Idle";
  this->state_names[STATE_RESET] = "Reset";
  this->state_names[STATE_DISABLED] = "Disabled";
}

/* Public Non-Virtual Functions */
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

void Subsystem::operator= (double test) {
  printf("%f", test);
}

void Subsystem::reset() {
  change_state(STATE_RESET);
}

/* Public Virtual Functions */
void Subsystem::disable() {
  change_state(STATE_DISABLED);
}

void Subsystem::set_target(double target) {}

void Subsystem::set_power(double power) {}

void Subsystem::set_velocity(double velocity) {}
