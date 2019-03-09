#include "subsystem.hpp"

uint8_t Subsystem::number_of_subsystems = 0;
Subsystem* Subsystem::subsystems[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

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

/* Constructor */
Subsystem::Subsystem() {
  this->state_names[STATE_IDLE] = "Idle";
  this->state_names[STATE_RESET] = "Reset";
  this->state_names[STATE_DISABLED] = "Disabled";
  subsystems[number_of_subsystems] = this;
  Subsystem::number_of_subsystems++;
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
  set_state(new_state);
}

void Subsystem::reset() {
  set_state(STATE_RESET);
}

/* Public Virtual Functions */
void Subsystem::disable() {
  set_state(STATE_DISABLED);
}

void Subsystem::set_state(uint8_t new_state) {
  this->last_state = state;
  this->state = new_state;
  this->state_change_time = pros::millis();
  log_ln(LOG_STATES, "Switching %s to %s state from %s state", (this->subsystem_name).c_str(), (this->state_names[state]).c_str(), (this->state_names[last_state]).c_str());
}

/* Private */
void Subsystem::set_state_target(uint8_t new_state, double target) {
  set_state(new_state);
  this->target = target;
}

void Subsystem::set_state_power(uint8_t new_state, double power) {
  set_state(new_state);
  this->power = power;
}

void Subsystem::set_state_velocity(uint8_t new_state, double velocity) {
  set_state(new_state);
  this->velocity = velocity;
}

/* Static Functions */
void Subsystem::disable_all() {
  for(int i = 0; i < Subsystem::number_of_subsystems; i++) {
    if (subsystems[i] != nullptr) {
      subsystems[i]->disable();
    } else log_ln(LOG_ERROR, "Could not disable subsystem %d", i);
  }
  log_ln(LOG_SUBSYSTEMS, "Disabled all subsystems!");
}

void Subsystem::reset_all() {
  for(int i = 0; i < Subsystem::number_of_subsystems; i++) {
    if (subsystems[i] != nullptr) {
      subsystems[i]->reset();
    } else log_ln(LOG_ERROR, "Could not reset subsystem %d", i);
  }
  log_ln(LOG_SUBSYSTEMS, "Reset all subsystems!");
}
