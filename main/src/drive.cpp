#include "drive.hpp"
#include "shot_select.hpp"

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
	static int drive_pow = 0;
	static int drive_pow_lst = 0;
	static int drive_brake_timer = 0;

	if (shot_req_num == 0 || shot_req[shot_req_handled_num].drive_turn_handled) {
		//printf("%d Drive update\n", pros::millis());
	  int y = set_dz(ctrler.get_analog(JOY_DRIVE_FW), DRIVE_DZ);
		int x = set_dz(ctrler.get_analog(JOY_DRIVE_STRAFE), DRIVE_DZ);
		int a = set_scaled_dz(ctrler.get_analog(JOY_DRIVE_TURN), DRIVE_TURN_DZ);
		drive_pow = x+y+a;

		if (drive_pow) {
			drive_set(x, y, a);
			drive_brake_timer = 0;
			//printf("%d Drive Set %d\n", pros::millis(), drive_pow);
		}
		else if (!drive_pow && drive_pow_lst) {
			drive_set(0);
			//printf("%d Drive Set 0\n", pros::millis());
			drive_brake_timer = pros::millis() + DRIVE_BRAKE_TIME;
		}

		if (!drive_pow && drive_brake_timer && pros::millis() > drive_brake_timer) {
			//printf("%d Drive Brake \n", pros::millis());
			drive_brake();
			drive_brake_timer = 0;
		}

		drive_pow_lst = drive_pow;
	}

}
