#include "decapper.hpp"
/* Constructor */
Decapper::Decapper(std::string subsystem_name, uint8_t default_state, pros::Motor& decapper_motor) : Subsystem(subsystem_name, default_state), decapper_motor(decapper_motor) {
  this->state_names[STATE_AUTO_CONTROL] = "Auto Control";
  this->state_names[STATE_HOLD] = "Hold";
  this->state_names[STATE_DECAP] = "Decapping";
}

/* Private Functions */
void Decapper::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_DISABLED:
      this->decapper_motor.move(0);
      break;
    case STATE_RESET:
      this->decapper_motor.move(-45);
      break;
    case STATE_AUTO_CONTROL:
      if (this->target_velocity) this->decapper_motor.move_absolute(this->target, this->target_velocity);
      else if (this->target_power) this->decapper_motor.move(this->target_power);
      break;
    case STATE_HOLD:
      this->target = this->position;
      if (this->hold) this->decapper_motor.move_absolute(this->target, 200);
      else this->decapper_motor.move(0);
      break;
    case STATE_DECAP:
      this->move_to_velocity(Decapper::DECAP_POSITION, 200);
      break;
  }
}

/* Public Functions */
void Decapper::update() {
  this->velocity = this->decapper_motor.get_actual_velocity();
  this->position = this->decapper_motor.get_position();
  this->error = this->target - this->position;

  // printf("%f\n", this->position / GEAR_RATIO);
  // printf("pos: %f\n", this->decapper_motor.get_position() / Decapper::GEAR_RATIO);

  switch(this->state) {
    case STATE_DISABLED:
      break;
    case STATE_RESET:
      if (this->below_vel_threshold(5, 200)) {
        this->decapper_motor.tare_position();
        this->set_state(STATE_HOLD);
      } else if (this->timed_out(3000)) this->set_state(STATE_DISABLED);
      break;
    case STATE_AUTO_CONTROL:
      if (this->below_vel_threshold(5, 500)) {
        this->disable();
      } else if (fabs(this->error) < this->error_threshold) {
        log_ln(MOVE, CAPPER, "Capper move finished at %f, target was %f", this->position / this->GEAR_RATIO, this->target / this->GEAR_RATIO);
        this->set_state(STATE_HOLD);
      }
      break;
    case STATE_HOLD:
      break;
    case STATE_DECAP:
      if (fabs(this->position - Decapper::DECAP_POSITION) < 5 * Decapper::GEAR_RATIO) this->move_to_velocity(Decapper::CARRY_POSITION, 200);
      break;
  }

  this->last_velocity = this->velocity;
}

void Decapper::move_to_power(double target, int8_t power, bool hold, uint8_t error_threshold) {
  this->target_velocity = 0;
  this->target_power = power;
  this->target = target;
  this->error_threshold = error_threshold;
  this->hold = hold;
  this->set_state(STATE_AUTO_CONTROL);
}

void Decapper::move_to_velocity(double target, uint8_t velocity, bool hold, uint8_t error_threshold) {
  this->target_velocity = velocity;
  this->target_power = 0;
  this->target = target;
  this->error_threshold = error_threshold;
  this->hold = hold;
  this->set_state(STATE_AUTO_CONTROL);
}

void Decapper::next_position() {
  if (this->position < (Decapper::CARRY_POSITION - 5 * Decapper::GEAR_RATIO)) this->move_to_velocity(Decapper::CARRY_POSITION, 200);
  else if (this->state != STATE_DECAP) this->set_state(this->STATE_DECAP);
}

void Decapper::previous_position() {
  if (this->position > (Decapper::CARRY_POSITION + 5 * Decapper::GEAR_RATIO)) this->move_to_velocity(Decapper::CARRY_POSITION, 200);
  else this->move_to_velocity(Decapper::BOTTOM_POSITION, 200);
}
