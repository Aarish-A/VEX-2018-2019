#include "puncher.hpp"

Puncher::Puncher(std::string subsystem_name, uint8_t default_state, pros::Motor& puncher_motor, pros::ADILineSensor& ball_sensor) : Subsystem(subsystem_name, default_state), puncher_motor(puncher_motor), ball_sensor(ball_sensor) {
  state_names[STATE_CANCEL] = "Cancel";
  state_names[STATE_LOADING] = "Loading";
  state_names[STATE_LOADED] = "Loaded";
  state_names[STATE_PULLBACK] = "Pullback";
  state_names[STATE_BOLT_WAIT] = "Bolt Wait";
}

void Puncher::set_power(int8_t power) {
  this->puncher_motor.move(power);
}

void Puncher::move_to(double target, double velocity) {
  this->puncher_motor.move_absolute(target, 200);
}

void Puncher::set_state(uint8_t new_state) {
  switch(new_state) {
    case STATE_DISABLED:
      this->set_power(0);
      break;
    case STATE_RESET:
      this->shot_number = 0;
      this->cancelling_shot = false;
      this->set_power(-20);
      break;
    case STATE_CANCEL:
      this->move_to(this->OFFSET + (--(this->shot_number) * this->TPR) + this->HOLD);
      this->cancelling_shot = false;
      break;
    case STATE_LOADING:
      this->set_power(127);
      this->target = (this->OFFSET + (this->shot_number * this->TPR) + this->HOLD);
      break;
    case STATE_LOADED:
      this->set_power(this->HOLD_POWER);
      break;
    case STATE_PULLBACK:
      this->set_power(127);
      ++(this->shot_number);
      this->point_of_no_return = this->OFFSET + (this->shot_number * this->TPR) - this->NO_RETURN;
      this->ball_check_target = this->OFFSET + (this->shot_number * this->TPR) - this->BALL_CHECK_START;
      this->target = this->OFFSET + (this->shot_number * this->TPR) - (15.0 * this->GEAR_RATIO);
      break;
    case STATE_BOLT_WAIT:
      this->move_to(this->OFFSET + (this->shot_number * this->TPR));
      break;
  }
}

void Puncher::update() {
  this->position = puncher_motor.get_position();
  this->velocity = puncher_motor.get_actual_velocity();
  this->error = this->target - this->error;

  if (ball_sensor.get_value() < BALL_THRESHOLD) {
    this->ball_on_time = pros::millis();
    if (!ball_on) log_ln(LOG_PUNCHER, "Ball has gotten on the puncher");
    ball_on = true;
  }

  if (pros::millis() >= this->ball_on_time + BALL_OFF_TIME) {
    if (ball_on) log_ln(LOG_PUNCHER, "Ball has gotten off the puncher");
    ball_on = false;
  }

  switch(this->state) {
    case STATE_DISABLED:
      break;
    case STATE_RESET:
      if (timed_out(2250)) this->set_state(STATE_DISABLED);
      else if (below_vel_threshold(5, 150)) {
        this->puncher_motor.tare_position();
        this->set_state(STATE_LOADING);
      }
      break;
    case STATE_CANCEL:
      if (timed_out(2000)) this->set_state(STATE_DISABLED);
      else if (this->error >= -(4.0 * this->GEAR_RATIO)) this->set_state(STATE_LOADED);
      break;
    case STATE_LOADING:
      if (timed_out(1000)) this->set_state(STATE_DISABLED);
      else if (this->error <= 4.0 * this->GEAR_RATIO) this->set_state(STATE_LOADED);
      break;
    case STATE_LOADED:
      // Wait for Puncher::shoot() to be called
      break;
    case STATE_PULLBACK:
      if (this->position < this->point_of_no_return && !ball_on && this->position > this->ball_check_target) this->set_state(STATE_CANCEL);
      else if (this->cancelling_shot && this->position < this->point_of_no_return) this->set_state(STATE_CANCEL);
      else if (timed_out(800)) this->set_state(STATE_DISABLED);
      else if (this->position > this->target) this->set_state(STATE_BOLT_WAIT);
      break;
    case STATE_BOLT_WAIT:
      if (timed_out(this->BOLT_WAIT_TIME)) this->set_state(STATE_LOADING);
      break;
  }

  this->last_velocity = this->velocity;
}

void Puncher::shoot() {
  if (this->state == STATE_LOADED) this->set_state(STATE_PULLBACK);
}

void Puncher::cancel_shot() {
  this->cancelling_shot = true;
}

bool Puncher::shooting() {
  return !(this->state == STATE_LOADED || this->state == STATE_CANCEL);
}
