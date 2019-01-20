#include "decapper.hpp"

using namespace pros;

Decapper_States decapper_state = Decapper_States::Idle;
Decapper_States decapper_state_last = decapper_state;
int decapper_state_change_time = 0;

void set_decapper_state(Decapper_States state) {
	decapper_state_last = decapper_state;
	decapper_state = state;
	decapper_state_change_time = pros::millis();
}

void decapper_set(int power)
{
  decapper.move(power);
}

void decapper_move(double pos, int32_t velocity)
{
  decapper.move_absolute(pos, velocity);
}

// void decapper_reset()
// {
//   uint32_t timeout_time = millis() + 500;
//   bool success = true;
//   decapper_set(-20);
//   while (fabs(decapper.get_actual_velocity()) < 12 && (success = (millis() < timeout_time)))
//   {
//     delay(10);
//   }
//   timeout_time = millis() + 2500;
//   while (fabs(decapper.get_actual_velocity()) > 10 && (success = (millis() < timeout_time)))
// 	{
// 		delay(10);
// 	}
//   delay(100);
//   decapper.tare_position();
//   decapper_set(-5);
//   set_decapper_state(Decapper_States::Idle);
// }
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
      if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT))
      {
        decapper_move(DECAPPER_PICKUP,40);
        set_decapper_state(Decapper_States::Pickup);
      }
			else if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT))
			{
				decapper_move(DECAPPER_DECAPLOW);
				set_decapper_state(Decapper_States::Decap_Low);
			}
      break;

    case Decapper_States::Pickup:
      if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT))
      {
        decapper_move(DECAPPER_CAPPING,90);
        set_decapper_state(Decapper_States::Capping);
      }
      else if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))
      {
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
      }
      break;

    case Decapper_States::Capping:
      if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))
      {
        decapper_set(-127);
        set_decapper_state(Decapper_States::Lowering);
      }
      break;

    case Decapper_States::Decap_Low:
			if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))
			{
				decapper_set(-127);
				set_decapper_state(Decapper_States::Lowering);
			}
			else if(ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT))
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
