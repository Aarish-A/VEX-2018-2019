#include "main.h"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "controls.hpp"
#include "macros/shot_queueing.hpp"
#include "libraries/util.hpp"
#include "libraries/task.hpp"

void opcontrol() {
	log_ln(PROGRAM_FLOW, "   --- %d START OPCONTROL --- \n", pros::millis());
	pilons::Task::stop_all_tasks();
	Subsystem::enable_all();

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

		else if (master.check_single_press(BTN_SHOT_R_T)) {
			puncher.shoot();
		}


		/* Macros */
		if (master.check_single_press(BTN_GROUND_PICKUP)) {
			angler.move_to(Angler::PICKUP_POSITION);
			intake.intake();
		}

		else if (master.check_single_press(BTN_CAP_PICKUP)) {
			angler.move_to(Angler::CAP_PICKUP_POSITION);
			intake.intake();
		}

		else if (master.check_double_press(BTN_GROUND_PICKUP, BTN_CAP_PICKUP)) {
			angler.move_to(Angler::CAP_FLIP_POSITION);
			intake.outtake();
		}

		else if (master.check_single_press(BTN_SHOT_R_T)) {
			make_shot_request(front_SP.top, Turn_Direction::STRAIGHT, true);
		}

		else if (master.check_single_press(BTN_SHOT_R_M)) {
			make_shot_request(front_SP.mid, Turn_Direction::STRAIGHT, true);
		}

		if (partner.check_single_press(BTN_SHOT_L_T)) {
			make_shot_request(platform_SP.top, Turn_Direction::LEFT);
		}

		else if (partner.check_single_press(BTN_SHOT_L_M)) {
			make_shot_request(platform_SP.mid, Turn_Direction::LEFT);
		}

		else if (partner.check_single_press(BTN_SHOT_R_T)) {
			make_shot_request(platform_SP.top, Turn_Direction::RIGHT);
		}

		else if (partner.check_single_press(BTN_SHOT_R_M)) {
			make_shot_request(platform_SP.mid, Turn_Direction::RIGHT);
		}

		pros::delay(5);
	}
}
