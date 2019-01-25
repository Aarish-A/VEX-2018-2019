#include "decapper.hpp"

using namespace pros;
pros::Task *decapper_cap_task = nullptr;
Decapper_States decapper_state = Decapper_States::Idle;
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
  decapper_set(-9);
  set_decapper_state(Decapper_States::Idle);
}

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

void decapper_handle()
{
  switch(decapper_state)
  {
    case Decapper_States::Lowering:
      if(decapper.get_position()<20) {
        decapper_set(-9);
        set_decapper_state(Decapper_States::Idle);
      }
    case Decapper_States::Idle:
      if(check_single_press(BTN_DECAPPER_DOWN))
      {
        decapper_move(DECAPPER_PICKUP,120);
        set_decapper_state(Decapper_States::Pickup);
      }
			else if(check_single_press(BTN_DECAPPER_UP))
			{
				decapper_move(DECAPPER_DECAPLOW);
				set_decapper_state(Decapper_States::Decap_Low);
			}
      break;

    case Decapper_States::Pickup:
      if(check_single_press(BTN_DECAPPER_DOWN))
      {
        //decapper.set_brake_mode(E_MOTOR_BRAKE_COAST);
        decapper_move(DECAPPER_CAPPING_HOLD,120);
        set_decapper_state(Decapper_States::Capping_Hold);
      }
      else if(check_double_press(BTN_DECAPPER_DOWN, BTN_DECAPPER_UP))
      {
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
      }
      break;
    case Decapper_States::Capping_Hold:
    if(decapper.get_position()>(DECAPPER_CAPPING_HOLD-400) && decapper.get_position()<(DECAPPER_CAPPING_HOLD-200))
    {
      decapper_set(60);
    }
    if(decapper.get_position()>(DECAPPER_CAPPING_HOLD-200))
    {
      decapper_move(DECAPPER_CAPPING_HOLD,70);
    }
    if(check_single_press(BTN_DECAPPER_DOWN))
    {
      decapper_start_cap_task();
      set_decapper_state(Decapper_States::Capping);
    }
    else if(check_double_press(BTN_DECAPPER_DOWN, BTN_DECAPPER_UP))
    {
      decapper_set(-127);
      set_decapper_state(Decapper_States::Lowering);
    }
      break;
    case Decapper_States::Capping:
      if(check_double_press(BTN_DECAPPER_DOWN, BTN_DECAPPER_UP))
      {
        decapper_stop_cap_task();
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
      }
      else if(check_single_press(BTN_DECAPPER_DOWN))
      {
        decapper_stop_cap_task();
        set_decapper_state(Decapper_States::Idle);
      }
      break;

    case Decapper_States::Decap_Low:
			if(check_double_press(BTN_DECAPPER_DOWN, BTN_DECAPPER_UP))
			{
				decapper_set(-127);
				set_decapper_state(Decapper_States::Lowering);
			}
			else if(check_single_press(BTN_DECAPPER_UP))
			{
				decapper_move(DECAPPER_DECAPPING);
				set_decapper_state(Decapper_States::Decapping);
			}
      break;

    case Decapper_States::Decapping:
		if(decapper.get_position()<DECAPPER_DECAPPING+10)
    {
      decapper_move(DECAPPER_DECAPLOW);
  		set_decapper_state(Decapper_States::Decap_Low);
    }
      break;
  }
}
