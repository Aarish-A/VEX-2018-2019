#include "main.h"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "controls.hpp"
#include "macros/shot_queueing.hpp"
#include "menu.hpp"
#include "libraries/util.hpp"
#include "libraries/task.hpp"

void opcontrol() {
	while (true) {
		master.print(2, 0, "L: %d, R: %d   ", enc_l.get_value() % 360, enc_r.get_value() % 360);
		printf("L: %d, R: %d\n", enc_l.get_value() % 360, enc_r.get_value() % 360);

		if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
			enc_r.reset();
			enc_l.reset();
		} else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
			autonomous();
		}

		intake.update();
		drive.update();
		angler.update();
		puncher.update();
		capper.update();

		pros::delay(2);
	}
}
