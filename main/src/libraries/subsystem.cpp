#include "subsystem.hpp"

uint8_t Subsystem::number_of_subsystems = 0;
Subsystem* Subsystem::subsystems[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

/* Protected Non-Virtual Functions */
void Subsystem::set_timeout_time(uint32_t time) {
  this->state_timeout_time = 0;
}

void Subsystem::set_timeout_velocity(double velocity, uint32_t time) {
  this->state_timeout_velocity = velocity;
  this->state_timeout_velocity_time = time;
}

bool Subsystem::timed_out() {
  return (pros::millis() - this->state_change_time > this->state_timeout_time && this->state_timeout_time != 0);
}

bool Subsystem::above_vel_threshold() {
  if (fabs(this->velocity) > fabs(this->state_timeout_velocity) && this->last_velocity < fabs(this->state_timeout_velocity)) this->velocity_exceeded_time = pros::millis();

  if (fabs(this->velocity) > fabs(this->state_timeout_velocity) && pros::millis() - this->velocity_exceeded_time > this->state_timeout_velocity_time && this->state_timeout_velocity != 0) {
    log_ln(LOG_ERROR, "%s timed out due to having a velocity greater than %f for %d ms. Velocity Was %f", (this->subsystem_name).c_str(), this->state_timeout_velocity, this->state_timeout_velocity_time, this->velocity);
    return true;
  } else return false;
}

bool Subsystem::below_vel_threshold() {
  if (fabs(this->velocity) < fabs(this->state_timeout_velocity) && this->last_velocity > fabs(this->state_timeout_velocity)) this->velocity_exceeded_time = pros::millis();

  if (fabs(this->velocity) < fabs(this->state_timeout_velocity) && pros::millis() - this->velocity_exceeded_time > this->state_timeout_velocity_time && this->state_timeout_velocity != 0) {
    log_ln(LOG_ERROR, "%s timed out due to having a velocity less than %f for %d ms. Velocity Was %f", (this->subsystem_name).c_str(), this->state_timeout_velocity, this->state_timeout_velocity_time, this->velocity);
    return true;
  } else return false;
}

/* Protected Virtual Functions */

/* Constructor */
Subsystem::Subsystem() {
  this->state_names[STATE_IDLE] = "Idle";
  this->state_names[STATE_RESET] = "Reset";
  this->state_names[STATE_DISABLED] = "Disabled";
  this->state_names[STATE_MOVE_POS] = "Move Pos";
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
  this->power = 0;
  this->target = 0;
  this->velocity_exceeded_time = 0;
  set_timeout_time();
  set_timeout_velocity();
  log_ln(LOG_STATES, "Switching %s to %s state from %s state", (this->subsystem_name).c_str(), (this->state_names[state]).c_str(), (this->state_names[last_state]).c_str());
}

void Subsystem::set_target(double target) {
  set_state_target(STATE_MOVE_POS, target);
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
