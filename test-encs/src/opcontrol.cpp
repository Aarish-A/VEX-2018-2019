#include "main.h"
#include "config.hpp"

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
void opcontrol() {
	enc_l.reset();
	enc_r.reset();
	enc_1.reset();
	enc_2.reset();
	enc_3.reset();
	enc_4.reset();
	while (true) {
		//printf("%d %d %d \n", pros::millis(), enc_l.get_value(), enc_r.get_value());

		printf("%d|||  %d|%d %d|%d %d|%d %d|%d \n", pros::millis(), enc_1.get_value(), enc_1.get_value()%360, enc_2.get_value(), enc_2.get_value()%360, enc_3.get_value(), enc_3.get_value()%360, enc_4.get_value(), enc_4.get_value()%360);
		if (ctrler.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B))
		{
			enc_l.reset();
			enc_r.reset();
		}
		pros::delay(100);
	}
}
