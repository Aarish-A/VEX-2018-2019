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

	angler.move(-30);
	delay(100);
	while (angler.get_actual_velocity() < -10) delay(10);
	angler.tare_position();
	angler.move_absolute(17 * 7, 100); // 35 for caps, 17 for ground

	int intakePower = 0;

	while (true) {
		if (millis() > nextLog) {
			nextLog = millis() + 100;
			ctrler.print(2, 0, "%.2f   ", getGlobalAngle() * 180.0 / M_PI);
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)) {
			resetGlobalAngle();
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
			setDriveVel(0);
			delay(10);
			turnDrive(0, 200);
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_R1)) {
			if (intakePower > 0) {
				intakePower = 0;
				intake.move(0);
			}
			else {
				intakePower = 127;
				intake.move(127);
			}
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_R2)) {
			if (intakePower < 0) {
				intakePower = 0;
				intake.move(0);
			}
			else {
				intakePower = -127;
				intake.move(-127);
			}
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))	 {
			angler.move_absolute(35 * 7, 100);
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) {
			angler.move_absolute(17 * 7, 100);
		}

		int y = ctrler.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		int x = ctrler.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		int a = ctrler.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);

		printf("%d %d %d \n", pros::millis(), encL.get_value(), encR.get_value());
		setDrive(x, y, a);

		delay(10);
	}
}