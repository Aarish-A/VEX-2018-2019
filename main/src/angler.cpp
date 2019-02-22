#include "angler.hpp"

using namespace pros;

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
	static int anglerJoystickDZ = 0;
	static int anglerJoystickDZLast = 0;
	static int anglerPow = 0;

	anglerJoystickDZ = set_dz(ctrler.get_analog(JOY_ANGLER), ANGLER_DZ);
	// printf("%d\n", abs(ctrler.get_analog(JOY_DRIVE_STRAFE)));

	if (angler.get_position() < ANGLER_BOT_LIM_POS && anglerJoystickDZ < 0) anglerJoystickDZ = 0;
	else if (angler.get_position() > ANGLER_TOP_LIM_POS && anglerJoystickDZ > 0) anglerJoystickDZ = 0;

	if (anglerJoystickDZ) anglerPow = (int)((abs(anglerJoystickDZ) - ANGLER_DZ) * (127.0 - ANGLER_MIN_VAL) / (127.0 - ANGLER_DZ) + ANGLER_MIN_VAL) * sgn(anglerJoystickDZ);
	else anglerPow = 0;

	if (angler_enabled) {
		if (check_double_press(BTN_SHOT_L_T, BTN_SHOT_L_M))
		{
			angler_move(ANGLER_CAP_FLIP_POS, 100);
			intake_state_set(IntakeState::Back);
			log_ln(LOG_ANGLER, "%d Angler Cap Flip. Pos:%f TPos:%f", pros::millis(), angler.get_position(), angler.get_target_position());
		}
		else if (check_single_press(BTN_SHOT_L_M)) {
			angler_move(ANGLER_PU_POS, 100);
			intake_state_set(IntakeState::Forw);
			log_ln(LOG_ANGLER, "%d Angler PU. Pos:%f TPos:%f", pros::millis(), angler.get_position(), angler.get_target_position());
		}
		else if (check_single_press(BTN_SHOT_L_T)) {
			angler_move(ANGLER_CAP_PU_POS, 100);
			intake_state_set(IntakeState::Forw);
			log_ln(LOG_ANGLER, "%d Angler Cap PU. Pos:%f TPos:%f", pros::millis(), angler.get_position(), angler.get_target_position());
		}
		else if(check_double_press(BTN_SHOT_R_T, BTN_SHOT_R_M))
		{
			angler_move(ANGLER_CAP_PU_POS,100);
			intake_state_set(IntakeState::Back);
		}
	}

	if (anglerPow) {
		angler_set(anglerPow);
		// log_ln(LOG_ANGLER, "%d Angler Set %d. Pos:%f", pros::millis(), anglerPow, angler.get_position());
	}
	else if (!anglerJoystickDZ && anglerJoystickDZLast) {
		angler_set(0);
		angler.move_relative(0, 100);
		log_ln(LOG_ANGLER, "%d Angler Set 0. Pos:%f", pros::millis(), angler.get_position());
	}

	anglerJoystickDZLast = anglerJoystickDZ;
}
