#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "subsystems/intake.hpp"
#include "libraries/subsystem.hpp"

void opcontrol() {
	printf("%d P - Opcontrol \n", pros::millis());
	log_ln(PROGRAM_FLOW, "   --- START OPCONTROL --- \n");
	long loop_counter = 0;
	while (true) {
		update_buttons();
		intake.update();
		drive.update();
		log_ln(PROGRAM_FLOW, "OPCONTROL TEST TEST TEST | cntr: %ld", loop_counter);
		loop_counter++;
		pros::delay(100);
	}
}
