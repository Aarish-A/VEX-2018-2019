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
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	int screen_num = 0;
	while (true) {
		switch (screen_num) {
			case 0:
				master.print(2, 0, "drive:%d %d %d %d               ", m_drive_bl.get_actual_position(), m_drive_br.get_actual_position(), m_drive_fl.get_actual_position(), m_drive_fr.get_actual_position());
				break;
			case 1:
				master.print(2, 0, "intake:%d angler:%d               ", m_intake.get_actual_position(), m_angler.get_actual_position());
				break;
			case 2:
				master.print(2, 0, "pun:%d b_d:%d               ", m_puncher.get_actual_position(), s_ball_detector.get_value());
				break;
			case 3:
				master.print(2, 0, "capper:%d pole_ptoi:%d               ", m_capper.get_actual_position(), s_pole_poti.get_value());
				break;
			}
			if (master.
	}
}
