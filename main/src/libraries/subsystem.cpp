#include "subsystem.hpp"

uint8_t Subsystem::number_of_subsystems = 0;
Subsystem* Subsystem::subsystems[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

/* Protected Non-Virtual Functions */
bool Subsystem::timed_out(uint32_t timeout) {
  if (pros::millis() - this->state_change_time > timeout) {
    log_ln(LOG_ERROR, "%s timed out in %s state due to taking more than %d ms", (this->subsystem_name).c_str(), (this->state_names[this->state]).c_str(), timeout);
    return true;
  } else return false;
}

bool Subsystem::above_vel_threshold(double velocity, uint32_t timeout) {
  if (fabs(this->velocity) > fabs(velocity) && fabs(this->last_velocity) < fabs(velocity)) this->velocity_exceeded_time = pros::millis();
  else if (fabs(this->velocity) < fabs(velocity) && fabs(this->last_velocity) > fabs(velocity)) this->velocity_exceeded_time = 0;

  if (fabs(this->velocity) > fabs(velocity) && pros::millis() - this->velocity_exceeded_time > timeout && this->velocity_exceeded_time != 0) {
    log_ln(LOG_ERROR, "%s timed out in %s state due to having a velocity greater than %f for %d ms. Velocity Was %f", (this->subsystem_name).c_str(), (this->state_names[this->state]).c_str(), velocity, timeout, this->velocity);
    return true;
  } else return false;
}

bool Subsystem::below_vel_threshold(double velocity, uint32_t timeout) {
  if (fabs(this->velocity) < fabs(velocity) && fabs(this->last_velocity) > fabs(velocity)) this->velocity_exceeded_time = pros::millis();
  else if (fabs(this->velocity) > fabs(velocity) && fabs(this->last_velocity) < fabs(velocity)) this->velocity_exceeded_time = 0;

  if (fabs(this->velocity) < fabs(velocity) && pros::millis() - this->velocity_exceeded_time > timeout && this->velocity_exceeded_time != 0) {
    log_ln(LOG_ERROR, "%s timed out due in %s state to having a velocity less than %f for %d ms. Velocity Was %f", (this->subsystem_name).c_str(), (this->state_names[this->state]).c_str(), velocity, timeout, this->velocity);
    return true;
  } else return false;
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
  this->velocity_exceeded_time = 0;
  // set_timeout_time();
  // set_timeout_velocity();
  log_ln(LOG_STATES, "Switching %s to %s state from %s state", (this->subsystem_name).c_str(), (this->state_names[state]).c_str(), (this->state_names[last_state]).c_str());
}

void Subsystem::set_target(double target) {
}

void Subsystem::set_power(double target) {
}

void Subsystem::set_velocity(double target) {
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
