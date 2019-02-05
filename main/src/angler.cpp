#include "angler.hpp"

using namespace pros;

//anglerState angler_state;

void angler_init() {
	angler.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	log_ln(LOG_ANGLER, "%d Finished angler init", pros::millis());
}

void angler_set(int power) {
  angler.move(power);
}

void angler_move(double position, int32_t velocity) {
  angler.move_absolute(position, velocity);
}

void angler_cal() {
	log_ln(LOG_ANGLER, "%d Started angler cal", pros::millis());
  angler.move(-30);
	delay(100);
	while (angler.get_actual_velocity() < -10) {
		log_ln(LOG_ANGLER, "%d Waiting for angler to stop moving", pros::millis());
		delay(10);
	}
	angler.tare_position();
	angler.move_absolute(ANGLER_PU_POS, 100);
	uint32_t timeout = pros::millis() + 1000;
	while(ANGLER_PU_POS - angler.get_position() < 5 && timeout >= pros::millis()) pros::delay(10);
	log_ln(LOG_ANGLER, "%d Done Angler Cal ", pros::millis());

	/*
  delay(2000);
	log_ln("%d Done Angler Cal Wait ", pros::millis());
	angler_set(0);
	log_ln("%d Angler 0 pow %f ", pros::millis(), angler.get_position());
	delay(2000);
	log_ln("%d Angler 0 pow wait done %f ", pros::millis(), angler.get_position());
	*/
}

void angler_handle() {
	static int anglerPow = 0;
	static int anglerPowLst = 0;

	anglerPow = set_dz(ctrler.get_analog(JOY_ANGLER), ANGLER_DZ);

	if (angler.get_position() < ANGLER_BOT_LIM_POS && anglerPow < 0) anglerPow = 0;
	else if (angler.get_position() > ANGLER_TOP_LIM_POS && anglerPow > 0) anglerPow = 0;
/*
	if (shot_req_num > 0) { //(shot_req[shot_req_handled_num].drive_turn_handled) {
		angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
	}
	else
	*/
	if (shot_req_num == 0) {
		if (shot_req[0].field_pos != FieldPos_Back && shot_req[0].field_pos != FieldPos_PF_Back_Blue && shot_req[0].field_pos != FieldPos_PF_Back_Red)
		{

				if (check_double_press(BTN_SHOT_L_T, BTN_SHOT_L_M))
				{
					angler_move(ANGLER_CAP_FLIP_POS, 100);
					intake_state_set(-80, IntakeState::Back);
					log_ln(LOG_ANGLER, "%d Angler Cap Flip. Pos:%f TPos:%f", pros::millis(), angler.get_position(), angler.get_target_position());
				}
				else if (check_single_press(BTN_SHOT_L_M)) {
					angler_move(ANGLER_PU_POS, 100);
					intake_state_set(127, IntakeState::Forw);
					log_ln(LOG_ANGLER, "%d Angler PU. Pos:%f TPos:%f", pros::millis(), angler.get_position(), angler.get_target_position());
				}
				else if (check_single_press(BTN_SHOT_L_T)) {
					angler_move(ANGLER_CAP_PU_POS, 100);
					intake_state_set(127, IntakeState::Forw);
					log_ln(LOG_ANGLER, "%d Angler Cap PU. Pos:%f TPos:%f", pros::millis(), angler.get_position(), angler.get_target_position());
				}
		}

		if (anglerPow) {
			angler_set(anglerPow);
			log_ln(LOG_ANGLER, "%d Angler Set %d. Pos:%f", pros::millis(), anglerPow, angler.get_position());
		}
		else if (!anglerPow && anglerPowLst) {
			angler_set(0);
			angler.move_relative(0, 100);
			log_ln(LOG_ANGLER, "%d Angler Set 0. Pos:%f", pros::millis(), angler.get_position());
		}
	}

	anglerPowLst = anglerPow;
}
