#include "decapper.hpp"

using namespace pros;
pros::Task *decapper_cap_task = nullptr;
Decapper_States decapper_state = Decapper_States::Bot;
Decapper_States decapper_state_last = decapper_state;
double decapper_targ = 0;
int decapper_state_change_time = 0;

void set_decapper_targ_state(Decapper_States state, double targ) {
  printf("Going from %d", decapper_state);
	decapper_state_last = decapper_state;
	decapper_state = state;
	decapper_state_change_time = pros::millis();
  decapper_targ = targ;
  decapper_move(targ);
  printf(" to %d\n", decapper_state);
}

void set_decapper_pow_state(Decapper_States state, double pow) {
  printf("Going from %d", decapper_state);
	decapper_state_last = decapper_state;
	decapper_state = state;
	decapper_state_change_time = pros::millis();
  decapper_set(0);
  printf(" to %d\n", decapper_state);
}

void set_decapper_state(Decapper_States state) {
  switch(state) {
    case Decapper_States::Bot: set_decapper_targ_state(state, DECAPPER_BOT_POS); break;
    case Decapper_States::Mid: set_decapper_targ_state(state, DECAPPER_MID_POS); break;
    case Decapper_States::Top: set_decapper_targ_state(state, DECAPPER_TOP_POS); break;
    case Decapper_States::Idle: set_decapper_pow_state(state, 0); break;
  }
  log_ln(LOG_DECAPPER, "%d Change Decapper State to %d", millis(), state);
}

void decapper_set(int power)
{
  decapper.move(power);
}

void decapper_move(double pos, int32_t velocity)
{
  decapper.move_absolute(pos, velocity);
}

void decapper_cal()
{
  uint32_t timeout_time = millis() + 100;
  bool success = true;
  decapper_set(-50);
  while (fabs(decapper.get_actual_velocity()) < 12 && (success = (millis() < timeout_time)))
  {
    delay(10);
  }
  timeout_time = millis() + 3500;
  while (fabs(decapper.get_actual_velocity()) > 10 && (success = (millis() < timeout_time)))
	{
		delay(10);
	}
  delay(100);
  decapper.tare_position();
  set_decapper_state(Decapper_States::Bot);
  decapper_set(DECAPPER_BOT_HOLD_POW);
}
/*
void decapper_stop_cap_task(bool stop_motor)
{
  if(decapper_cap_task != nullptr)
  {
    decapper_cap_task->remove();
    delete decapper_cap_task;
    decapper_cap_task = nullptr;
  }
  if(stop_motor) decapper_set(0);
}

void decapper_start_cap_task()
{
  decapper_stop_cap_task(false);
  decapper_cap_task = new Task(decapper_cap);
}

void decapper_cap(void *param)
{
  flatten_against_wall(true,true);
  pos.reset();
  move_drive_rel_simple(-2.0_in, 40, true);
  decapper_move(DECAPPER_CAPPING);
  while (decapper.get_position() < DECAPPER_CAPPING - 5) delay(10);
  setDriveVel(0);
  delay(10);
  decapper_set(-10);
  delay(100);
  move_drive_rel_simple(-1.0_in, 40, true);
  decapper_set(-127);
  set_decapper_state(Decapper_States::Lowering);
}
*/
void decapper_handle()
{
  switch(decapper_state)
  {
    case Decapper_States::Bot:
    {
      if (check_single_press(BTN_DECAPPER_UP))
      {
        set_decapper_state(Decapper_States::Mid);
      }
      else if (decapper.get_position() < (DECAPPER_BOT_POS + (2*DECAPPER_RATIO))) decapper_set(DECAPPER_BOT_HOLD_POW);
      printf("%d Bot vel:%f StateCT:%d pos:%f t:%f \n", millis(), decapper.get_actual_velocity(), decapper_state_change_time, decapper.get_position(), decapper_targ);
      //else if (decapper.get_actual_velocity() > -1 && millis() > (decapper_state_change_time+200))
      break;
    }
    case Decapper_States::Mid:
    {
      if (check_single_press(BTN_DECAPPER_UP))
      {
        set_decapper_state(Decapper_States::Top);
      }
      else if (check_single_press(BTN_DECAPPER_DOWN))
      {
        set_decapper_state(Decapper_States::Bot);
      }
      else if (fabs(decapper.get_position() - DECAPPER_MID_POS) < (4*DECAPPER_RATIO)) decapper.move_relative(0, 200);
      printf("%d Mid vel:%f StateCT:%d pos:%f t:%f \n", millis(), decapper.get_actual_velocity(), decapper_state_change_time, decapper.get_position(), decapper_targ);
      break;
    }
    case Decapper_States::Top:
    {
      if (decapper.get_position()>DECAPPER_TOP_POS-15 && check_single_press(BTN_DECAPPER_DOWN))
      {
          set_decapper_state(Decapper_States::Mid);
      }
      printf("%d Top vel:%f StateCT:%d pos:%f t:%f \n", millis(), decapper.get_actual_velocity(), decapper_state_change_time, decapper.get_position(), decapper_targ);
    }
    if (pros::millis() > decapper_state_change_time+200 && fabs(decapper.get_actual_velocity()) < 20 && fabs(decapper_targ-decapper.get_position()) > 50)
      set_decapper_state(Decapper_States::Idle);
    case Decapper_States::Idle:
    {
      if (check_single_press(BTN_DECAPPER_DOWN))
      {
        set_decapper_state(Decapper_States::Bot);
      }
      break;
    }
  }
}
