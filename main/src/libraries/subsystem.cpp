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
void Subsystem::set_state(uint8_t new_state) {
  this->last_state = state;
  this->state = new_state;
  this->state_change_time = pros::millis();
  this->velocity_exceeded_time = 0;
  if (this->last_state != this->state) log_ln(LOG_STATES, "Switching %s to %s state from %s state", (this->subsystem_name).c_str(), (this->state_names[state]).c_str(), (this->state_names[last_state]).c_str());
}

/* Constructor */
Subsystem::Subsystem(std::string subsystem_name, uint8_t default_state) : subsystem_name(subsystem_name), DEFAULT_STATE(default_state) {
  this->state_names[STATE_DISABLED] = "Disabled";
  this->state_names[STATE_RESET] = "Reset";
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

double Subsystem::get_velocity() {
  return this->velocity;
}

void Subsystem::enable() {
  if (this->state == STATE_DISABLED) {
    log_ln(LOG_SUBSYSTEMS, "Enabled %s subsystem", (this->subsystem_name).c_str());
    set_state(this->DEFAULT_STATE);
  }
}

void Subsystem::disable() {
  if (this->state != STATE_DISABLED) {
    log_ln(LOG_ERROR, "Disabled %s subsystem", (this->subsystem_name).c_str());
    set_state(STATE_DISABLED);
  }
}

bool Subsystem::enabled() {
  return this->state != this->STATE_DISABLED;
}

bool Subsystem::disabled() {
  return this->state == this->STATE_DISABLED;
}

void Subsystem::reset() {
  log_ln(LOG_ERROR, "Resetting %s subsystem", (this->subsystem_name).c_str());
  set_state(STATE_RESET);
}

/* Public Virtual Functions */


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

bool Subsystem::any_resetting() {
  for(int i = 0; i < Subsystem::number_of_subsystems; i++) {
    if (subsystems[i] != nullptr && subsystems[i]->state == STATE_RESET) return true;
  }
  return false;
}
