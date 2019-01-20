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
  decapper_set(-5);
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
  setDrive(0, 40, 0);
  int enc_last, enc_cur = enc_l.get_value();
  do {
    delay(200);
    enc_last = enc_cur;
    enc_cur = enc_l.get_value();
  } while (abs(enc_cur - enc_last) > 2);
  setDriveVel(0);
  delay(10);
  move_drive_simple(-1.4_in, 35, true);
  decapper_move(DECAPPER_CAPPING, 40);
  while (decapper.get_position() < DECAPPER_CAPPING - 5) delay(10);
  decapper.move(0);
  delay(200);
  setDriveVel(0, -40, 0);
  delay(250);
  setDrive(0, 0, 0);

  set_decapper_state(Decapper_States::Idle);

  // decapper_set(-127);
  // set_decapper_state(Decapper_States::Lowering);
}

void decapper_handle()
{
  switch(decapper_state)
  {
    case Decapper_States::Lowering:
      if(decapper.get_position()<20) {
        decapper.move_relative(0, 200);
        set_decapper_state(Decapper_States::Idle);
      }
    case Decapper_States::Idle:
      if(check_single_press(BTN_DECAPPER_DOWN))
      {
        decapper_move(DECAPPER_PICKUP,40);
        set_decapper_state(Decapper_States::Pickup);
      }
			else if(check_single_press(BTN_DECAPPER_UP))
			{
				decapper_move(DECAPPER_DECAPLOW);
				set_decapper_state(Decapper_States::Decap_Low);
			}
      else if(check_double_press(BTN_DECAPPER_DOWN, BTN_DECAPPER_UP))
      {
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
      }
      break;

    case Decapper_States::Pickup:
      if(check_single_press(BTN_DECAPPER_DOWN))
      {
        decapper_move(DECAPPER_CAPPING_HOLD,70);
        set_decapper_state(Decapper_States::Capping_Hold);
      }
      else if(check_double_press(BTN_DECAPPER_DOWN, BTN_DECAPPER_UP))
      {
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
      }
      break;
    case Decapper_States::Capping_Hold:
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
			// if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))
			// {
			// 	decapper_set(-127);
			// 	set_decapper_state(Decapper_States::Lowering);
			// }
			// else if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT))
			// {
			// 	decapper_move(DECAPPER_DECAPLOW);
			// 	set_decapper_state(Decapper_States::Decap_Low);
			// }
      break;
  }
}
