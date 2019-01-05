#include "drive.hpp"

using namespace pros;

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

void drive_init() { }

void drive_handle() {
  int y = ctrler.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
	int x = ctrler.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
	int a = ctrler.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
	drive_set(x, y, a);
}
