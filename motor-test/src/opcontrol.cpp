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

pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Motor motor_a(6, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor motor_b(5, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
void opcontrol() {
	motor_a.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	motor_a.move(-50);
	//motor_a.move_relative(0, 100);
	//motor_b.move(127);
	while (true) {
		printf("%d A: Pos:%f, Vel:%f, Volt:%f, Temp:%f, Current:%f|  B: Pos:%f, Vel:%f \n", motor_a.get_position(), motor_a.get_actual_velocity(), motor_a.get_voltage(), motor_a.get_temperature(), motor_a.get_current_draw(), motor_b.get_position(), motor_b.get_actual_velocity());
		pros::delay(100);
	}
}
