#include "main.h"
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

void opcontrol() {
	uint32_t nextLog = 0;
	while (true) {
		if (millis() > nextLog) {
			nextLog = millis() + 100;
			ctrler.print(2, 0, "%.2f   ", getGlobalAngle() * 180.0 / M_PI);
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)) {
			driveFL.tare_position();
			driveBL.tare_position();
			driveFR.tare_position();
			driveBR.tare_position();
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
			setDriveVel(0);
			delay(10);
			turnDrive(-getGlobalAngle(), 100);
		}

		int y = ctrler.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		int x = ctrler.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		int a = ctrler.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		setDrive(x, y, a);

		delay(10);
	}
}
