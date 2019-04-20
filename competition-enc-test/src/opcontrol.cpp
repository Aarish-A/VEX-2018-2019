#include "main.h"

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
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::ADIEncoder enc_r(7, 8, false);
	pros::ADIEncoder enc_l(1, 2, false);
	enc_r.reset();
	enc_l.reset();
	while (true) {
		master.print(2, 0, "L: %d, R: %d", enc_l.get_value() % 360, enc_r.get_value() % 360);
		printf("L: %d, R: %d\n", enc_l.get_value() % 360, enc_r.get_value() % 360);
		pros::delay(2);
	}
}
