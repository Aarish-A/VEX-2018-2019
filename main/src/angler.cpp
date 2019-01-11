#include "angler.hpp"

using namespace pros;

//anglerState angler_state;

void angler_init() {
	angler.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void angler_set(int power) {
  angler.move(power);
}

void angler_move(double position, int32_t velocity) {
  angler.move_absolute(position, velocity);
}

void angler_cal() {
  angler.move(-30);
	delay(100);
	while (angler.get_actual_velocity() < -10) delay(10);
	angler.tare_position();
	angler.move_absolute(ANGLER_PU_POS, 100);
	delay(1000);
	printf("%d Done Angler Cal \n", pros::millis());

	/*
  delay(2000);
	printf("%d Done Angler Cal Wait \n", pros::millis());
	angler_set(0);
	printf("%d Angler 0 pow %f \n", pros::millis(), angler.get_position());
	delay(2000);
	printf("%d Angler 0 pow wait done %f \n", pros::millis(), angler.get_position());
	*/
}

btn_dp_detector angler_back_dp(pros::E_CONTROLLER_DIGITAL_L1, pros::E_CONTROLLER_DIGITAL_L2);

void angler_handle() {
	static int anglerPow = 0;
	static int anglerPowLst = 0;

	anglerPow = set_dz(ctrler.get_analog(JOY_ANGLER), ANGLER_DZ);

	if (angler.get_position() < 0 && anglerPow < 0) anglerPow = 0;
	else if (angler.get_position() > 320 && anglerPow > 0) anglerPow = 0;
/*
	if (shot_req_num > 0) { //(shot_req[shot_req_handled_num].drive_turn_handled) {
		angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
	}
	else
	*/
	if (shot_req_num == 0) {
		if (shot_req[0].field_pos == FieldPos_Back)
		{
			angler_back_dp.reset_timer();
			if (btn[BTN_ANGLER_PU-6].pressed) {
			 	angler_move(ANGLER_PU_POS, 100);
				intake_state_set(127, IntakeState::Forw);
			 	printf("%d Angler PU. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
		 	}
			else if (btn[BTN_ANGLER_CAP_PU-6].pressed) {
			 	angler_move(ANGLER_CAP_PU_POS, 100);
				intake_state_set(127, IntakeState::Forw);
			 	printf("%d Angler Cap PU. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
		 	}
		}
		else
		{
			angler_back_dp.set_first_pressed();
			if (pros::millis() < angler_back_dp.get_timer())
			{
				if ( (angler_back_dp.get_first_pressed() == BTN_SHOT_L_T && btn[BTN_SHOT_L_M-6].pressed) || (angler_back_dp.get_first_pressed() == BTN_SHOT_L_M && btn[BTN_SHOT_L_T-6].pressed) )
				{
					angler_move(ANGLER_CAP_FLIP_POS, 100);
					intake_state_set(-80, IntakeState::Back);
					printf("%d Angler Cap Flip. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
					angler_back_dp.reset_timer();
				}
			}
			else if (angler_back_dp.get_timer())
			{
				if (btn[BTN_SHOT_L_M-6].pressed) {
					angler_move(ANGLER_PU_POS, 100);
					intake_state_set(127, IntakeState::Forw);
					printf("%d Angler PU. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
					angler_back_dp.reset_timer();
				}
				else if (btn[BTN_SHOT_L_T-6].pressed) {
					angler_move(ANGLER_CAP_PU_POS, 100);
					intake_state_set(127, IntakeState::Forw);
					printf("%d Angler Cap PU. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
					angler_back_dp.reset_timer();
				}
			}
		}

		if (anglerPow) {
			angler_set(anglerPow);
			printf("%d Angler Set %d. Pos:%f\n", pros::millis(), anglerPow, angler.get_position());
		}
		else if (!anglerPow && anglerPowLst) {
			angler_set(0);
			angler.move_relative(0, 100);
			printf("%d Angler Set 0. Pos:%f\n", pros::millis(), angler.get_position());
		}
	}

	anglerPowLst = anglerPow;
}
