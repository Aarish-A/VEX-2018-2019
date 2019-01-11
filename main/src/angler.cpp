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
			if (btn[BTN_SHOT_L_M-6].pressed) {
				angler_move(ANGLER_PU_POS, 100);
				intake_state_set(127, IntakeState::Forw);
				printf("%d Angler PU. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
			}
			else if (btn[BTN_SHOT_L_T-6].pressed) {
				angler_move(ANGLER_CAP_PU_POS, 100);
				intake_state_set(127, IntakeState::Forw);
				printf("%d Angler Cap PU. Pos:%f TPos:%f\n", pros::millis(), angler.get_position(), angler.get_target_position());
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
