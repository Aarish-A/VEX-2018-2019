#include "main.h"
#include "button.hpp"
#include "logs.hpp"
#include "config.hpp"

void opcontrol() {
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());
	Subsystem::reset_all();

	while (true) {
		update_buttons();
		intake.update();
		drive.update();
		angler.update();


		if (check_single_press(BTN_SHOT_L_T)) {
			angler.set_target(Angler::CAP_PICKUP_POSITION);
			intake.set_state(Intake::STATE_PICKUP);
		} else if (check_single_press(BTN_SHOT_L_M)) {
			angler.set_target(Angler::PICKUP_POSITION);
			intake.set_state(Intake::STATE_PICKUP);
		} else if (check_double_press(BTN_SHOT_L_T, BTN_SHOT_L_M)) {
			angler.set_target(Angler::CAP_FLIP_POSITION);
			intake.set_state(Intake::STATE_CAP_FLIP);
		}

		pros::delay(10);
	}
}
