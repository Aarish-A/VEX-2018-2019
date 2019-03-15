#include "decapper.hpp"

using namespace pros;
pros::Task *decapper_cap_task = nullptr;
Decapper_States decapper_state = Decapper_States::Bot;
Decapper_States decapper_state_last = decapper_state;
double decapper_targ = 0;
int decapper_state_change_time = 0;
bool decapper_cappable = false;
int going_up = 1;
bool slow_bottom = true;
void set_decapper_targ_state(Decapper_States state, double targ, int32_t velocity) {
  printf("Going from %d", decapper_state);
	decapper_state_last = decapper_state;
	decapper_state = state;
	decapper_state_change_time = pros::millis();
  decapper_targ = targ;
  decapper_move(targ, velocity);
  printf(" to %d\n", decapper_state);
}

void set_decapper_pow_state(Decapper_States state, double pow) {
  printf("Going from %d", decapper_state);
	decapper_state_last = decapper_state;
	decapper_state = state;
	decapper_state_change_time = pros::millis();
  decapper_set(pow);
  printf(" to %d\n", decapper_state);
}

void set_decapper_state(Decapper_States state) {
  switch(state) {
    case Decapper_States::Bot:
    if(slow_bottom)
    {
    set_decapper_targ_state(state, DECAPPER_BOT_POS, 55);
    }
    else
    {
    set_decapper_targ_state(state, DECAPPER_BOT_POS, 200);
    }
    break;
    // case Decapper_States::Mid: set_decapper_pow_state(state, 50); break;
    case Decapper_States::Mid:
    if(going_up==1)
    {
    set_decapper_targ_state(state, DECAPPER_MID_POS+100, 55);
    }
    else if(going_up==2)
    {
    set_decapper_targ_state(state, DECAPPER_MID_POS, 200);
    }
    else if(going_up==3)
    {
    set_decapper_targ_state(state, DECAPPER_MID_POS-100, 200);
    }
    break;
    // case Decapper_States::Top_Bot: set_decapper_targ_state(state, DECAPPER_TOP_BOT_POS, 200); break;
    case Decapper_States::Top_Bot: set_decapper_pow_state(state, 100); break;
    case Decapper_States::Top_Mid: set_decapper_targ_state(state, DECAPPER_TOP_MID_POS, 125); break;
    // case Decapper_States::Top: set_decapper_targ_state(state, DECAPPER_TOP_POS, 125); break;
    case Decapper_States::Top: set_decapper_pow_state(state, 90); decapper_targ = DECAPPER_TOP_POS; decapper_cappable = false; break;
    case Decapper_States::Idle: set_decapper_pow_state(state, 0); break;
  }
  log_ln(LOG_DECAPPER, "%d Change Decapper State to %d from %d | Time: %d | Targ: %d", millis(), state, decapper_state_last, decapper_state_change_time, decapper_targ);
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
  // decapper_set(DECAPPER_BOT_HOLD_POW);
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
      //printf("%d Bot vel:%f StateCT:%d pos:%f t:%f \n", millis(), decapper.get_actual_velocity(), decapper_state_change_time, decapper.get_position(), decapper_targ);
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
      // else if (fabs(decapper.get_position() - DECAPPER_MID_POS) < (2*DECAPPER_RATIO)) decapper_set(DECAPPER_MID_HOLD_POW);//decapper.move_relative(0, 200);
      //printf("%d Mid vel:%f StateCT:%d pos:%f t:%f \n", millis(), decapper.get_actual_velocity(), decapper_state_change_time, decapper.get_position(), decapper_targ);
      break;
    }
    case Decapper_States::Top_Bot:
    {
      if (fabs(decapper.get_position() - DECAPPER_TOP_BOT_POS) < (3*DECAPPER_RATIO)) set_decapper_state(Decapper_States::Top_Mid);
      break;
    }
    case Decapper_States::Top_Mid:
    {
      if (fabs(decapper.get_position() - DECAPPER_TOP_MID_POS) < (3*DECAPPER_RATIO)) decapper.move_relative(0, 200);
      break;
    }
    case Decapper_States::Top:
    {
      printf("Decapper at: %f, target: %f\n", decapper.get_position(), DECAPPER_TOP_POS - 5);
      if (decapper.get_position()>DECAPPER_TOP_POS-15 && check_single_press(BTN_DECAPPER_DOWN))
      {
          set_decapper_state(Decapper_States::Mid);
      }
      if (decapper.get_position() > DECAPPER_TOP_POS - 5) {
        decapper_set(DECAPPER_TOP_HOLD_POW);
        printf("HERE!\n");
        decapper_cappable = true;
        //set_decapper_state(Decapper_States::Idle);
      }
      // else if (fabs(decapper.get_position() - DECAPPER_TOP_POS) < (5*DECAPPER_RATIO)) decapper_set(DECAPPER_TOP_HOLD_POW);//decapper.move_relative(0, 200);
      //printf("%d Top vel:%f StateCT:%d pos:%f t:%f \n", millis(), decapper.get_actual_velocity(), decapper_state_change_time, decapper.get_position(), decapper_targ);
    }
    case Decapper_States::Idle:
    {
      if (check_single_press(BTN_DECAPPER_DOWN))
      {
        set_decapper_state(Decapper_States::Bot);
      }
      if (check_single_press(BTN_DECAPPER_UP))
      {
        if (decapper.get_position() < DECAPPER_MID_POS) set_decapper_state(Decapper_States::Mid);
        else set_decapper_state(Decapper_States::Top);
      }
      break;
    }
  }
  // if (pros::millis() > decapper_state_change_time+100 && fabs(decapper.get_actual_velocity()) < 20 && fabs(decapper_targ-decapper.get_position()) > 20)
  //   set_decapper_state(Decapper_States::Idle);
}
