#include "capper.hpp"

/* Constructor */
Capper::Capper(std::string subsystem_name, uint8_t default_state, pros::Motor& capper_motor) : Subsystem(subsystem_name, default_state), capper_motor(capper_motor) {
  this->state_names[STATE_AUTO_CONTROL] = "Auto Control";
  this->state_names[STATE_HOLD] = "Hold";
  this->state_names[STATE_CAPPING] = "Capping";
}

/* Private Functions */
void Capper::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_DISABLED:
      this->capper_motor.move(0);
      break;
    case STATE_RESET:
      this->capper_motor.move(-45);
      break;
    case STATE_AUTO_CONTROL:
      if (this->target_velocity) this->capper_motor.move_absolute(this->target, this->target_velocity);
      else if (this->target_power) this->capper_motor.move(this->target_power);
      break;
    case STATE_HOLD:
      this->target = this->position;
      if (this->hold) this->capper_motor.move_absolute(this->target, 200);
      else this->capper_motor.move(0);
      break;
    case STATE_CAPPING:
      this->target = 130 * Capper::GEAR_RATIO;
      break;
  }
}

/* Public Functions */
void Capper::update() {
  this->velocity = this->capper_motor.get_actual_velocity();
  this->position = this->capper_motor.get_position();
  this->error = this->target - this->position;

  switch(this->state) {
    case STATE_DISABLED:
      break;
    case STATE_RESET:
      if (this->below_vel_threshold(5, 200)) {
        this->capper_motor.tare_position();
        this->set_state(STATE_HOLD);
      } else if (this->timed_out(3000)) this->set_state(STATE_DISABLED);
      break;
    case STATE_AUTO_CONTROL:
      if (timed_out(this->move_timeout) || below_vel_threshold(1, 500)) {
        this->disable();
      }
      else if (fabs(this->error) < this->error_threshold) {
        log_ln(LOG_STATES, "Capper move finished at %f, target was %f", this->position, this->target);
        this->set_state(STATE_HOLD);
      }
      break;
    case STATE_HOLD:
      break;
    case STATE_CAPPING:
      if (this->position < 90 * Capper::GEAR_RATIO) this->capper_motor.move_velocity(200);
      else if (this->position < 115 * Capper::GEAR_RATIO) this->capper_motor.move_velocity(110);
      else if (this->position < 145 * Capper::GEAR_RATIO) this->capper_motor.move_velocity(85);
      else if (this->position < 165 * Capper::GEAR_RATIO) this->capper_motor.move_velocity(70);
      else if (this->position < Capper::CAPPING_HOLD_POSITION) this->capper_motor.move_velocity(45);
      else this->set_state(STATE_HOLD);
      break;
  }

  this->last_velocity = this->velocity;
}

void Capper::move_to_power(double target, int8_t power, bool hold, uint8_t error_threshold) {
  this->target_velocity = 0;
  this->target_power = power;
  this->target = target;
  this->error_threshold = error_threshold;
  this->hold = hold;
  this->set_state(STATE_AUTO_CONTROL);
}

void Capper::move_to_velocity(double target, uint8_t velocity, bool hold, uint8_t error_threshold) {
  this->target_velocity = velocity;
  this->target_power = 0;
  this->target = target;
  this->error_threshold = error_threshold;
  this->hold = hold;
  this->set_state(STATE_AUTO_CONTROL);
}

void Capper::move_to_pickup() {
  this->move_to_velocity(Capper::PICKUP_POSITION, 200);
}

void Capper::move_to_cap_flip() {
  // this->move_to_velocity(Capper::CAP_FLIP_POSITION, 100);
  this->move_to_velocity(Capper::CAP_FLIP_POSITION, 125);
}

void Capper::move_to_flag_flip() {
  this->move_to_velocity(FLAG_FLIP_POSITION, 65);
}

void Capper::pickup_cap() {
  this->move_to_velocity(Capper::CARRY_POSITION, 65);
}

void Capper::start_capping() {
  this->set_state(STATE_CAPPING);
}

void Capper::finish_capping() {
  while(this->state == STATE_CAPPING) pros::delay(2);
  this->move_to_power(CAPPING_END_POSIITON, 105, false);
}

bool Capper::capping() {
  return this->state != STATE_HOLD;
}
