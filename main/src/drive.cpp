#include "drive.hpp"
#include "auto.hpp"
#include "puncher.hpp"
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

	if (shot_req_drive_handled  < shot_req_num && shot_req_drive_handled == shot_num && !handling_shot_req) {
		printf("%d Construct Drive Shoot Task\n", pros::millis());
		pros::Task drive_shot_turn_task ((pros::task_fn_t)drive_shot_turn, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Drive_Shot_Turn_Task");
	}
	else if (!handling_shot_req) {
		//printf("%d Drive update\n", pros::millis());
	  int y = set_dz(ctrler.get_analog(JOY_DRIVE_FW), DRIVE_DZ);
		int x = set_dz(ctrler.get_analog(JOY_DRIVE_STRAFE), DRIVE_DZ);
		int a = set_scaled_dz(ctrler.get_analog(JOY_DRIVE_TURN), DRIVE_TURN_DZ);
		drive_pow = x+y+a;

		if (drive_pow) drive_set(x, y, a);
		else if (!drive_pow && drive_pow_lst) {
			drive_set(0);
			printf("%d Drive Set 0\n", pros::millis());
		}
	}

	drive_pow_lst = drive_pow;
}

void drive_shot_turn() {
	handling_shot_req = true;
	pos.reset(0, 0, 0);
	//double x_targ = shot_req[shot_num-1].turn_dir == lD;
	printf("%d Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_drive_handled].flag_pos.x, shot_req[shot_req_drive_handled].flag_pos.y);
	turn_vel_side(new PointAngleTarget({shot_req[shot_req_drive_handled].flag_pos.x, shot_req[shot_req_drive_handled].flag_pos.y}), (200/50_deg));

	shot_req_drive_handled++;
	handling_shot_req = false;
	return;
}
