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

		pros::delay(10);
	}
}
