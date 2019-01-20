#include "decapper.hpp"

using namespace pros;

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

void decapper_handle()
{
  switch(decapper_state)
  {
    case Decapper_States::Lowering:
      if(decapper.get_position()<20) {
        decapper_set(-5);
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
      break;

    case Decapper_States::Pickup:
      if(check_single_press(BTN_DECAPPER_DOWN))
      {
        decapper_move(DECAPPER_CAPPING,90);
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
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
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
