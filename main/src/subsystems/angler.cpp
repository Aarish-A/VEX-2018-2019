#include "angler.hpp"
using namespace pros;

/* Constructor */
Angler::Angler(std::string subsystem_name, pros::Motor& angler_motor) : angler_motor(angler_motor) {
  this->subsystem_name = subsystem_name;
  state_names[STATE_MOVE_MANUAL] = "Move_Manual";
  state_names[STATE_HOLD] = "Hold";

  //Init variables
  angler_joy = 0;
  angler_joy_lst = 0;
  angler_enabled = false;
}

/* Private Functions */

/* Public Functions */
void Angler::set_state(uint8_t new_state) {
  Subsystem::set_state(new_state);
  switch(new_state) {
    case STATE_IDLE:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_RESET:
      angler_enabled = false;
      this->power = -30;
      set_timeouts(1000, 0);
      break;
    case STATE_DISABLED:
      this->power = 0;
      set_timeouts();
      break;
    case STATE_MOVE_MANUAL:
      this->power = angler_joy;
      set_timeouts();
      break;
    case STATE_MOVE_POS:
      this->power = 0;
      this->target = this->target;// Every time this state is entered, use set_target(double target) to make sure target is always set
      angler_motor.move_absolute(this->target, 200);
      set_timeouts(1500, 0);
      break;
    case STATE_HOLD:
      this->power = 0;
      this->target = angler_motor.get_position();
      angler_motor.move_absolute(this->target, 50);
      set_timeouts();
      break;
  }
}

void Angler::update() {
  this->velocity = angler_motor.get_actual_velocity();
  this->position = angler_motor.get_position();
  this->error = this->target - this->position;

  //Read joy vals
  angler_joy = set_scaled_dz(ctrler.get_analog(JOY_ANGLER), ANGLER_DZ);
  if (angler_joy < 0 && position < ANGLER_BOT_LIM_POS) angler_joy = 0;
  else if (angler_joy > 0 && position > ANGLER_TOP_LIM_POS) angler_joy = 0;

  switch(this->state) {
    case STATE_IDLE:
      break;
    case STATE_RESET:
      if (velocity > 0 && millis() > state_change_time+100) {
        angler_motor.tare_position();
        angler_enabled = true;
        set_state(STATE_MOVE_MANUAL);
      }
      break;
    case STATE_DISABLED:
      break;
    case STATE_MOVE_MANUAL:
      this->power = angler_joy;
      angler_motor.move(this->power);
      if (angler_joy_lst && !angler_joy) set_state(STATE_HOLD);
      break;
    case STATE_MOVE_POS:
      if (timed_out()) {
        log_ln(LOG_ANGLER, "%d Angler State_Move_Pos T_O -> Enter State_Hold | Pos:%f Targ:%f Err:%f Vel:%f", millis(), position, target, error, velocity);
        set_state(STATE_HOLD);
      }
      if (fabs(error) < 10) set_state(STATE_HOLD);
      break;
    case STATE_HOLD:
      break;
  }
  if (state != STATE_MOVE_MANUAL && angler_enabled && angler_joy) set_state(STATE_MOVE_MANUAL);

  angler_joy_lst = angler_joy;
}

void Angler::enable() {
  this->set_state(STATE_IDLE);
}
