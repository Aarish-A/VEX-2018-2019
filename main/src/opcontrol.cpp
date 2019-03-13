#include "main.h"
#include "button.hpp"
#include "logs.hpp"
#include "config.hpp"
#include "libraries/util.hpp"

void opcontrol() {
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());
	
	while (true) {
		update_buttons();
		intake.update();
		drive.update();
		angler.update();
		puncher.update();

		int8_t drive_x = set_dz(ctrler.get_analog(ANALOG_RIGHT_X), Drive::STRAFE_DEADZONE);
		int8_t drive_y = set_dz(ctrler.get_analog(ANALOG_LEFT_Y), Drive::THROTTLE_DEADZONE);
		int8_t drive_a = set_dz(ctrler.get_analog(ANALOG_LEFT_X), Drive::TURN_DEADZONE);
		int8_t angler_power = set_scaled_dz(ctrler.get_analog(JOY_ANGLER), Angler::DEADZONE);

		drive.driver_set(drive_x, drive_y, drive_a);
		angler.driver_set(angler_power);

		// if (check_single_press(BTN_SHOT_R_T)) Subsystem::disable_all();

		if (check_single_press(BTN_INTAKE_UP)) intake.off() ? intake.intake() : intake.stop();
		else if (check_single_press(BTN_INTAKE_DOWN)) intake.off() ? intake.outtake() : intake.stop();
		else if (check_single_press(BTN_SHOT_L_T)) {
			angler.move_to(Angler::CAP_PICKUP_POSITION);
			intake.intake();
		} else if (check_single_press(BTN_SHOT_L_M)) {
			angler.move_to(Angler::PICKUP_POSITION);
			intake.intake();
		} else if (check_double_press(BTN_SHOT_L_T, BTN_SHOT_L_M)) {
			angler.move_to(Angler::CAP_FLIP_POSITION);
			intake.outtake();
		}

		pros::delay(10);
	}
}
