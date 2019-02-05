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
	drive_fl.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_fr.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_bl.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_br.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	log_ln(LOG_DRIVE, "%d Finished drive initialize", pros::millis());
}

void drive_handle() {
	static int drive_pow = 0;
	static int a_lst = 0;
	static int drive_pow_lst = 0;
	static int drive_brake_timer = 0;

	if ((shot_req_num == 0 || shot_req[shot_req_handled_num].drive_turn_handled) ) {
		//log_ln("%d Drive update", pros::millis());
	  int y = set_dz(ctrler.get_analog(JOY_DRIVE_FW), DRIVE_DZ);
		int x = set_dz(ctrler.get_analog(JOY_DRIVE_STRAFE), DRIVE_DZ);
		int a = set_scaled_dz(ctrler.get_analog(JOY_DRIVE_TURN), DRIVE_TURN_DZ);
		drive_pow = x+y+a;

		if (drive_pow) {
			drive_set(x, y, a);
			drive_brake_timer = 0;
			//log_ln("%d Drive Set %d %d %d", pros::millis(), drive_pow, a, a_lst);
		}
		else if (!a && a_lst) {
			drive_brake_timer = pros::millis() + DRIVE_BRAKE_TIME;
			if (a_lst < 0) drive_set(0, 0, drive_brake_pow);
			else if (a_lst > 0) drive_set(0, 0, -drive_brake_pow);
			else drive_set(0);
			log_ln(LOG_DRIVE, "%d Start turn Brake %d", pros::millis(), drive_brake_pow);
		}
		else if (!drive_brake_timer && !drive_pow && drive_pow_lst) {
			drive_set(0);
			log_ln(LOG_DRIVE, "%d Drive Set 0", pros::millis());
			//drive_brake_timer = pros::millis() + DRIVE_BRAKE_TIME;
		}

		if (drive_brake_timer && pros::millis() > drive_brake_timer) {
			log_ln(LOG_DRIVE, "%d Drive Brake End ", pros::millis());
			//LOG(("%d Drive Brake End \n", pros::millis()));

			//drive_brake();
			drive_set(0); //TODO: DELTE BEFORE COMP

			drive_brake_timer = 0;
		}

		drive_pow_lst = drive_pow;
		a_lst = a;
	}

}
