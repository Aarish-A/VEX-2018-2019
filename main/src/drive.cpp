#include "drive.hpp"

//using namespace pros;

void drive_set(int x, int y, int a) {
	drive_fl.move(y + x + a);
	drive_bl.move(y - x + a);
	drive_fr.move(y - x - a);
	drive_br.move(y + x - a);
}

void drive_set(int pow) {
  drive_set(pow, pow, pow);
}

void drive_set_vel(int x, int y, int a) {
	drive_fl.move_velocity(y + x + a);
	drive_bl.move_velocity(y - x + a);
	drive_fr.move_velocity(y - x - a);
	drive_br.move_velocity(y + x - a);
}

void drive_set_vel(int vel) {
  drive_set(vel, vel, vel);
}

void drive_init() {
	drive_fl.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	drive_fr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	drive_bl.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	drive_br.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void drive_handle() {
  int y = set_dz(ctrler.get_analog(JOY_DRIVE_FW), DRIVE_DZ);
	int x = set_dz(ctrler.get_analog(JOY_DRIVE_STRAFE), DRIVE_DZ);
	int a = set_scaled_dz(ctrler.get_analog(JOY_DRIVE_TURN), DRIVE_TURN_DZ);
	drive_set(x, y, a);
}
