#include "main.h"
#include "puncher.hpp"
#include "drive.hpp"
#include "intake.hpp"

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
	uint32_t lstTime = 0;

	while (true) {
		pun_handle();
		drive_handle();
		intake_handle();

		if (millis() - lstTime > 100) {
			lstTime = millis();
			ctrler.print(2, 0, "%d %d  ", (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature());
			printf("%d %.3f %.3f\n", millis(), puncherLeft.get_power(), puncherRight.get_power());
		}

		delay(10);
	}
}
