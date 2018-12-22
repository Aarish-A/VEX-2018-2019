#include "main.h"
#include "tracking.hpp"
#include "config.hpp"
#include "auto.hpp"

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

using namespace pros;
using namespace pilons::tracking;

void opcontrol() {
	controller.clear();

	printf("Hello, world\n");

	pos.startTask();
	pros::delay(1000);
	moveToTargetAngle(pos, 0, 24, 0);

	return;

	uint32_t update = 0;
	while (true) {
		/*
		int y = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		int x = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		int a = controller.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);

		driveFL.moveVx + a);
		driveFR.move(y - x - a);
		driveBR.move(y + x - a);

		if (controller.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)) {
			pos.reset();
		}

		pos.update();

		if (millis() > update) {
			controller.print(2, 0, "%2d %2d %2d %2d", (int)driveFL.get_temperature(), (int)driveBL.get_temperature(), (int)driveFR.get_temperature(), (int)driveBR.get_temperature());
			update = millis() + 100;
		}
		*/

		pros::delay(1);
	}
}
