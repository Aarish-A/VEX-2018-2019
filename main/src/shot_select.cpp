#include "shot_select.hpp"

/* Shot Positions */
ShotPos front_SP (FieldPos_Front, 90, 0);
ShotPos pf_SP (FieldPos_PF, 90, 70);
ShotPos pf_back_SP (FieldPos_PF_Back, 90, 70);
ShotPos back_SP (FieldPos_Back, 90, 10);

ShotSelect shot_req[2];
bool angler_pu_flag = false;

int shot_req_num = 0;
int shot_req_handled_num = 0;

bool shot_cancel_pressed = false;

pros::Task shot_req_handle_task ((pros::task_fn_t)shot_req_handle, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Shot_Req_Handle_Task");

btn_dp_detector shot_queue_dp(pros::E_CONTROLLER_DIGITAL_L1, pros::E_CONTROLLER_DIGITAL_R2);
/* Shot Select Code */
void inc_shot_req_num() {
	if (shot_req_num < 2) shot_req_num++;
  else shot_req_num = 2;
}

void dec_shot_req_num() {
  if (shot_req_num > 0) shot_req_num--;
  else shot_req_num = 0;
}

void set_field_pos(FieldPos field_pos) {
  shot_req[0].field_pos = field_pos;
  shot_req[1].field_pos = field_pos;
	log("%d Shot Field Pos Set (Task State:%d)| RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
}

void set_angle_targ(bool top) {
  switch (shot_req[shot_req_num-1].field_pos) {
    case FieldPos_Front:
      shot_req[shot_req_num-1].angle_targ = top? front_SP.top : front_SP.mid;
      break;

    case FieldPos_Back:
      shot_req[shot_req_num-1].angle_targ = top? back_SP.top : back_SP.mid;
      break;

    case FieldPos_PF:
      shot_req[shot_req_num-1].angle_targ = top? pf_SP.top : pf_SP.mid;
      break;

    case FieldPos_PF_Back:
      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
      break;
  }
}

void set_turn_dir(Dir turn_dir) {
  shot_req[shot_req_num-1].turn_dir = turn_dir;
	if (shot_req[shot_req_num-1].field_pos == FieldPos_PF_Back) { //Shooting from behind the platform
		shot_req[shot_req_num-1].flag_pos.y = 100;
		if (turn_dir == Dir_Left) {
			shot_req[shot_req_num-1].flag_pos.x = -24;
		}
		else if (turn_dir == Dir_Right) {
			shot_req[shot_req_num-1].flag_pos.x = 24;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}
	else { //Shooting from the back
		shot_req[shot_req_num-1].flag_pos.y = 125;
		if (turn_dir == Dir_Left) {
			shot_req[shot_req_num-1].flag_pos.x = -48;
		}
		else if (turn_dir == Dir_Right) {
			shot_req[shot_req_num-1].flag_pos.x = 48;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}
}

void set_handled_vars() {
	shot_req[shot_req_num-1].drive_turn_handled = false;
	shot_req[shot_req_num-1].shot_handled = false;
}

void set_shot_req(bool top, Dir turn_dir) {
	inc_shot_req_num();
	set_angle_targ(top);
	set_turn_dir(turn_dir);
	set_handled_vars();
	log("%d Shot Req (Task State:%d)| RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);

	shot_queue_dp.reset_timer();
}

void shot_req_make() {
  //Set Field Pos
  if (shot_req_num == 0)
  {
    if (btn[BTN_FIELD_FRONT-6].pressed) set_field_pos(FieldPos_Front);
    else if (btn[BTN_FIELD_PF-6].pressed) set_field_pos(FieldPos_PF);
		else if (btn[BTN_SHOOT_CANCEL-6].pressed) set_field_pos(FieldPos_PF_Back);
    else if (btn[BTN_FIELD_BACK-6].pressed) set_field_pos(FieldPos_Back);
  }
	else if (btn[BTN_SHOOT_CANCEL-6].pressed) {
		shot_cancel_pressed = true;
		shot_queue_dp.reset_timer();
		log("  >>> %d Cancel Shot Req Handle Task - Before Suspend| State %d | shot_req_num = %d, shot_req_handled_num = %d \n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req_handled_num);
		shot_req_handle_task.suspend();
		log("  >>> %d Cancel Shot Req Handle Task - Suspended| State %d | shot_req_num = %d, shot_req_handled_num = %d \n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req_handled_num);
		setDrive(0);

		shot_req_num = 0;
		shot_req_handled_num = 0;
		set_handled_vars();
		shot_req_handle_task.resume();
		log("  >>> %d Shot Req Handle Task  - Resume | State %d | shot_req_num = %d, shot_req_handled_num = %d \n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req_handled_num);
	}

  //Set other shot constants
		//Every button press after the second will overwrite the second.
		//So not all requests have been made, or the second request hasn't been executed yet -> Prevents from overwriting second request during execution
	if (shot_req_num < 2 || shot_req_handled_num < 1)
	{
		bool L_T, L_M, R_T, R_M;

		shot_queue_dp.set_first_pressed();
		if (shot_req[0].field_pos == FieldPos_Back) {
			L_T = btn[BTN_SHOT_L_T-6].pressed;
			L_M = btn[BTN_SHOT_L_M-6].pressed;
		}
		else {
			L_T = 0;
			L_M = 0;
			shot_queue_dp.override_first_pressed(BTN_SHOT_L_T);
			shot_queue_dp.override_first_pressed(BTN_SHOT_L_M);
		}
		R_T = btn[BTN_SHOT_R_T-6].pressed;
		R_M = btn[BTN_SHOT_R_M-6].pressed;
		if (pros::millis() < shot_queue_dp.get_timer()) {
			if ( (shot_queue_dp.get_first_pressed() == BTN_SHOT_R_T && L_T) ||
		       (shot_queue_dp.get_first_pressed() == BTN_SHOT_L_T && R_T) ) {
				set_shot_req(true, Dir_Centre);
			}
			else if ( (shot_queue_dp.get_first_pressed() == BTN_SHOT_R_M && L_M) ||
		       (shot_queue_dp.get_first_pressed() == BTN_SHOT_L_M && R_M) ) {
				set_shot_req(false, Dir_Centre);
			}
		}
		else if (shot_queue_dp.get_timer()) {
			if (shot_queue_dp.get_first_pressed() == BTN_SHOT_L_T) {
				set_shot_req(true, Dir_Left);
			}
			else if (shot_queue_dp.get_first_pressed() == BTN_SHOT_L_M) {
				set_shot_req(false, Dir_Left);
			}
			else if (shot_queue_dp.get_first_pressed() == BTN_SHOT_R_T) {
				set_shot_req(true, Dir_Right);
			}
			else if (shot_queue_dp.get_first_pressed() == BTN_SHOT_R_M) {
				set_shot_req(false, Dir_Right);
			}
		}
	}
}

void shot_req_handle() {
	log("%d Start Shot Req Handle Task \n",  pros::millis());
	shot_req_num = 0;
	while (true) {
		if (shot_req_num > 0) {
			log("%d Start handling first shot request \n", pros::millis());
			set_handled_vars(); //Make sure all handled vars are reset to false
			shot_req_handled_num = 0; //Make sure we start handling shot requests from index 0

			if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				pos.reset(0,0,0);
				flatten_against_wall();
				setDrive(0, -15, 0);
			}

			pos.reset(0, 0, 0);
			//Angle handle 1
			angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);

			//Drive Handle 1
			if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				log("%d S1 Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg));
			}
			shot_req[shot_req_handled_num].drive_turn_handled = true;

			//Shooter Handle 1
			while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
			shot_req_handled_num++;

			if (shot_req_num > 1) {
				//Angle handle 1
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);

				//Drive Handle 2
				if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
					log("%d S2 Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
					turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg));
				}
				shot_req[shot_req_handled_num].drive_turn_handled = true;

				//Shooter Handle 2
				while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
				shot_req_handled_num++;

				shot_queue_dp.reset_timer(); //Make sure that timer is reset after a double shot request is completed (the normal polling routine that handles this cannot run during this time)

				angler.move_absolute(ANGLER_PU_POS, 200);
			}
			if (angler.get_position() < 10) angler.move_absolute(ANGLER_PU_POS, 200);

			shot_req_num = 0;
			shot_req_handled_num = 0;

		}
		pros::delay(10);
	}
}
