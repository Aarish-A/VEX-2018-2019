#include "decapper.hpp"

using namespace pros;
pros::Task *decapper_cap_task = nullptr;
Decapper_States decapper_state = Decapper_States::Bot;
Decapper_States decapper_state_last = decapper_state;
int decapper_state_change_time = 0;

void set_decapper_state(Decapper_States state) {
  printf("Going from %d", decapper_state);
	decapper_state_last = decapper_state;
	decapper_state = state;
	decapper_state_change_time = pros::millis();
  printf(" to %d\n", decapper_state);
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
  decapper_set(DECAPPER_BOT_HOLD_POW);
  set_decapper_state(Decapper_States::Bot);
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
        decapper_move(DECAPPER_MID_POS);
        decapper_state = Decapper_States::Mid;
      }
      else if (decapper.get_position() < (DECAPPER_BOT_POS + (2*DECAPPER_RATIO))) decapper_set(DECAPPER_BOT_HOLD_POW);
      break;
    }
    case Decapper_States::Mid:
    {
      if (check_single_press(BTN_DECAPPER_UP))
      {
        decapper_move(DECAPPER_TOP_POS);
        decapper_state = Decapper_States::Top;
      }
      else if (check_single_press(BTN_DECAPPER_DOWN))
      {
        decapper_move(DECAPPER_BOT_POS);
        decapper_state = Decapper_States::Bot;
      }
      else if (fabs(decapper.get_position() - DECAPPER_MID_POS) < (4*DECAPPER_RATIO)) decapper_set(DECAPPER_MID_HOLD_POW);
      break;
    }
    case Decapper_States::Top:
    {
      if (decapper.get_position() > (DECAPPER_TOP_POS - (1*DECAPPER_RATIO)))
      {
        decapper_move(DECAPPER_BOT_POS);
        decapper_state = Decapper_States::Bot;
      }
      break;
    }
  }
}
