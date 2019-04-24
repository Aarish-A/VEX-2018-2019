#include "capper.hpp"
/* Constructor */
Capper::Capper(std::string subsystem_name, uint8_t default_state, pros::Motor& capper_motor) : Subsystem(subsystem_name, default_state), capper_motor(capper_motor) {
  this->state_names[STATE_AUTO_CONTROL] = "Auto Control";
  this->state_names[STATE_HOLD] = "Hold";
  this->state_names[STATE_CAPPING] = "Capping";
  this->state_names[STATE_FINISH_CAPPING] = "State Finish Capping";
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
    case STATE_FINISH_CAPPING:
      this->capper_motor.move(100);
      this->target = Capper::CAPPING_END_POSIITON;
      break;
  }
}

/* Public Functions */
void Capper::update() {
  this->velocity = this->capper_motor.get_actual_velocity();
  this->position = this->capper_motor.get_position();
  this->error = this->target - this->position;

  // printf("%f\n", this->position / GEAR_RATIO);


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
      // if (timed_out(this->move_timeout) || below_vel_threshold(1, 500)) {
      //   this->disable();
      // }
      if (fabs(this->error) < this->error_threshold) {
        log_ln(MOVE, CAPPER, "Capper move finished at %f, target was %f", this->position / this->GEAR_RATIO, this->target / this->GEAR_RATIO);
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
    case STATE_FINISH_CAPPING:
      if (this->below_vel_threshold(1, 25)) {
        this->hold = false;
        this->set_state(STATE_HOLD);
      }
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

void Capper::wait_for_target_reach() {
  while (this->state == STATE_AUTO_CONTROL) pros::delay(2);
}

void Capper::move_to_pickup() {
  this->move_to_velocity(Capper::PICKUP_POSITION, 200);
}

void Capper::move_to_cap_flip(bool holding_cap, double velocity) {
  // this->move_to_velocity(Capper::CAP_FLIP_POSITION, 100);
  if (holding_cap) this->move_to_velocity(Capper::CAP_FLIP_POSITION, velocity);
  // else this->move_to_velocity(33 * this->GEAR_RATIO, 140, true, 2);
  else this->move_to_velocity(35 * this->GEAR_RATIO, 140, true, 2);
}

void Capper::move_to_flag_flip(double velocity) {
  this->move_to_velocity(FLAG_FLIP_POSITION, velocity);
}

void Capper::pickup_cap(bool expansion_zone) {
  if (expansion_zone) this->move_to_velocity(100 * Capper::GEAR_RATIO, 80);
  else this->move_to_velocity(Capper::CARRY_POSITION + 10 * Capper::GEAR_RATIO, 65);
}

void Capper::start_capping() {
  this->set_state(STATE_CAPPING);
}

void Capper::finish_capping() {
  while(this->state == STATE_CAPPING) pros::delay(2);
  this->set_state(STATE_FINISH_CAPPING);
}

bool Capper::capping() {
  return this->state != STATE_HOLD;
}
bool Capper::at_flag_flip_position() {
  return fabs(this->position - Capper::FLAG_FLIP_POSITION) < 10 * Capper::GEAR_RATIO;
}
bool Capper::at_pickup_position() {
  return (fabs(this->position) - Capper::PICKUP_POSITION) < 10 * Capper::GEAR_RATIO;
}
