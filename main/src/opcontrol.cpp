#include "main.h"
#include "logs.hpp"
#include "config.hpp"
#include "libraries/util.hpp"
#include "controls.hpp"
#include "libraries/task.hpp"

void opcontrol() {
	pilons::Task::stop_all_tasks();
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());

	while (true) {
		master.update();
		partner.update();
		intake.update();
		drive.update();
		angler.update();
		puncher.update();

		/* Drive */
		int8_t drive_y = master.get_analog(JOY_DRIVE_THROTTLE, 10);
		int8_t drive_x = master.get_analog(JOY_DRIVE_STRAFE, 10);
		int8_t drive_a = master.get_analog(JOY_DRIVE_TURN, 25);
		drive.driver_set(drive_x, drive_y, drive_a);

		/* Angler */
		int8_t angler_power = master.get_analog(JOY_ANGLER, 40);
		angler.driver_set(angler_power);

		/* Intake */
		if (master.check_single_press(BTN_INTAKE_UP)) {
			intake.off() ? intake.intake() : intake.stop();
		}

		else if (master.check_single_press(BTN_INTAKE_DOWN)) {
			intake.off() ? intake.outtake() : intake.stop();
		}

		/* Puncher */
		if (master.check_single_press(BTN_SHOT_CANCEL)) {
			puncher.cancel_shot();
		}


		/* Macros */
		if (master.check_single_press(BTN_GROUND_PICKUP)) {
			angler.move_to(Angler::CAP_PICKUP_POSITION);
			intake.intake();
		}

		else if (master.check_single_press(BTN_CAP_PICKUP)) {
			angler.move_to(Angler::PICKUP_POSITION);
			intake.intake();
		}

		else if (master.check_double_press(BTN_GROUND_PICKUP, BTN_CAP_PICKUP)) {
			angler.move_to(Angler::CAP_FLIP_POSITION);
			intake.outtake();
		}

		pros::delay(10);
	}
}
