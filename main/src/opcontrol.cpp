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

		pros::delay(10);
	}
}
