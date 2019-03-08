#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "logs.hpp"
#include "config.hpp"

void opcontrol() {
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());

	while (true) {
		update_buttons();
		intake.update();

		if (check_single_press(BTN_SHOT_R_T)) log_ln(LOG_STATES, "P: %f V: %f", intake.get_power(), intake.get_velocity());

		pros::delay(10);
	}
}
