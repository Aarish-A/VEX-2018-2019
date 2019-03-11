#include "capper.hpp"

/* Constructor */
Capper::Capper(std::string subsystem_name, uint8_t default_state, pros::Motor& capper_motor) : Subsystem(subsystem_name, default_state), capper_motor(capper_motor) {
  this->state_names[STATE_PICKUP] = "Pickup";
  this->state_names[STATE_CARRY] = "Carry";
  this->state_names[STATE_CAP_START] = "Cap Start";
  this->state_names[STATE_CAP_INTERIM] = "Cap Interim";
  this->state_names[STATE_CAP_END] = "Cap End";
  this->state_names[STATE_CAP_FINISHED] = "Cap Finished";
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
    case STATE_PICKUP:
      this->target = this->PICKUP_POSITION;
      this->capper_motor.move_absolute(this->target, 200);
      break;
    case STATE_CARRY:
      this->target = this->CARRY_POSITION;
      if (this->last_state != this->STATE_CARRY) this->capper_motor.move_absolute(this->target, 200);
      else this->capper_motor.move_absolute(this->target, 50);
      break;
    case STATE_CAP_START:
      this->target = this->CAP_START_POSITION;
      this->capper_motor.move_absolute(this->target, 200);
      break;
    case STATE_CAP_INTERIM:
      this->target = this->CAP_INTERIM_POSITION;
      this->capper_motor.move_absolute(this->target, 75);
      break;
    case STATE_CAP_END:
      this->target = this->CAP_INTERIM_POSITION;
      this->capper_motor.move(90);
      break;
    case STATE_CAP_FINISHED:
      this->capper_motor.move(0);
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
        this->set_state(STATE_PICKUP);
      } else if (this->timed_out(3000)) this->set_state(STATE_DISABLED);
      break;
    case STATE_PICKUP:
      // Wait for pickup routine to begin
      break;
    case STATE_CARRY:
      // Wait for cap routine to beging
      break;
    case STATE_CAP_START:
      if (fabs(this->error) < 5 * this->GEAR_RATIO) this->set_state(this->STATE_CAP_INTERIM);
      break;
    case STATE_CAP_INTERIM:
      if (fabs(this->error) < 5 * this->GEAR_RATIO) this->capper_motor.move_relative(0, 200);
      break;
    case STATE_CAP_END:
      if (fabs(this->error) < 5 * this->GEAR_RATIO) this->set_state(this->STATE_CAP_FINISHED);
      break;
    case STATE_CAP_FINISHED:
      // Wait for move to ground to be called
      break;
  }

  this->last_velocity = this->velocity;
}

void Capper::pickup_cap() {
  if (this->state == STATE_PICKUP) this->set_state(STATE_CARRY);
}

void Capper::start_capping() {
  if (this->state == STATE_CARRY) this->set_state(STATE_CAP_START);
}

void Capper::finish_capping() {
  if (this->state == STATE_CAP_INTERIM && fabs(this->error) < 5 * this->GEAR_RATIO) this->set_state(STATE_CAP_END);
}

void Capper::move_to_bottom() {
  if (this->state != STATE_PICKUP) this->set_state(STATE_PICKUP);
}

bool Capper::ready_to_cap() {
  return (this->state == STATE_CAP_INTERIM && fabs(this-> error) < 5 * this->GEAR_RATIO);
}
