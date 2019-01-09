#include "shot_select.hpp"

/* Shot Positions */
ShotPos front_SP (FieldPos_Front, 90, 70);
ShotPos pf_SP (FieldPos_PF, 90, 70);
ShotPos pf_back_SP (FieldPos_PF_Back, 90, 70);
ShotPos back_SP (FieldPos_Back, 90, 70);

ShotSelect shot_req[2];

int shot_req_num = 0;
int shot_req_handled_num = 0;

bool shot_cancel_pressed = false;

pros::Task shot_req_handle_task ((pros::task_fn_t)shot_req_handle, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Shot_Req_Handle_Task");

/* Set up 4 shot queue buttons */
shot_queue_btn::btn_info shot_queue_btn::shot_queue_btn[4] {
	{BTN_SHOT_L_T, false}, {BTN_SHOT_L_M, false}, {BTN_SHOT_R_T, false}, {BTN_SHOT_R_M, false}
};
int shot_queue_btn::btn_queue_timer = 0;
pros::controller_digital_e_t shot_queue_btn::btn_queue_pressed;

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
	printf("%d Shot Field Pos Set | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
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
	shot_req[shot_req_num-1].flag_pos.y = 127;
	if (turn_dir == Dir_Left) {
		shot_req[shot_req_num-1].flag_pos.x = -48;
	}
	else if (turn_dir == Dir_Right) {
		shot_req[shot_req_num-1].flag_pos.x = 48;
	}
	else shot_req[shot_req_num-1].flag_pos.x = 0;
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
	printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);

	shot_queue_btn::btn_queue_timer = 0;
}

void shot_req_make() {
  //Set Field Pos
  if (shot_req_num == 0)
  {
    if (ctrler.get_digital_new_press(BTN_FIELD_FRONT)) set_field_pos(FieldPos_Front);
    else if (ctrler.get_digital_new_press(BTN_FIELD_PF)) set_field_pos(FieldPos_PF);
    else if (ctrler.get_digital_new_press(BTN_FIELD_BACK)) set_field_pos(FieldPos_Back);
  }

  //Set other shot constants
		//Every button press after the second will overwrite the second.
		//So not all requests have been made, or the second request hasn't been executed yet -> Prevents from overwriting second request during execution
	if (shot_req_num < 2 || shot_req_handled_num < 1)
	{
		for (int i = 0; i < 4; i++) shot_queue_btn::shot_queue_btn[i].set_pressed();
		for (int i = 0; i < 4; i++) {
			if (!shot_queue_btn::btn_queue_timer && shot_queue_btn::shot_queue_btn[i].pressed) {
				shot_queue_btn::btn_queue_timer = pros::millis() + shot_queue_btn::BTN_PRESS_TIME;
				shot_queue_btn::btn_queue_pressed = shot_queue_btn::shot_queue_btn[i].btn_name;
			}
		}
		if (pros::millis() < shot_queue_btn::btn_queue_timer) {
			if ( (shot_queue_btn::btn_queue_pressed == BTN_SHOT_R_T && shot_queue_btn::shot_queue_btn[shot_queue_btn::l_t].pressed) ||
		       (shot_queue_btn::btn_queue_pressed == BTN_SHOT_L_T && shot_queue_btn::shot_queue_btn[shot_queue_btn::r_t].pressed) ) {
				set_shot_req(true, Dir_Centre);
			}
			else if ( (shot_queue_btn::btn_queue_pressed == BTN_SHOT_R_M && shot_queue_btn::shot_queue_btn[shot_queue_btn::l_m].pressed) ||
		       (shot_queue_btn::btn_queue_pressed == BTN_SHOT_L_M && shot_queue_btn::shot_queue_btn[shot_queue_btn::r_m].pressed) ) {
				set_shot_req(false, Dir_Centre);
			}
		}
		else if (shot_queue_btn::btn_queue_timer) {
			if (shot_queue_btn::btn_queue_pressed == BTN_SHOT_L_T) {
				set_shot_req(true, Dir_Left);
			}
			else if (shot_queue_btn::btn_queue_pressed == BTN_SHOT_L_M) {
				set_shot_req(false, Dir_Left);
			}
			else if (shot_queue_btn::btn_queue_pressed == BTN_SHOT_R_T) {
				set_shot_req(true, Dir_Right);
			}
			else if (shot_queue_btn::btn_queue_pressed == BTN_SHOT_R_M) {
				set_shot_req(false, Dir_Right);
			}
		}
	}
	if (ctrler.get_digital_new_press(BTN_SHOOT_CANCEL)) {
		shot_cancel_pressed = true;
		printf("  >>> %d Cancel Shot Req Handle Task - Before Suspend| State %d | shot_req_num = %d, shot_req_handled_num = %d \n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req_handled_num);
		shot_req_handle_task.suspend();
		printf("  >>> %d Cancel Shot Req Handle Task - Suspended| State %d | shot_req_num = %d, shot_req_handled_num = %d \n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req_handled_num);
		setDrive(0);

		shot_req_num = 0;
		shot_req_handled_num = 0;
		set_handled_vars();
		shot_req_handle_task.resume();
		printf("  - %d Shot Req Handle Task  - Resume | State %d | shot_req_num = %d, shot_req_handled_num = %d \n", pros::millis(), shot_req_handle_task.get_state(), shot_req_num, shot_req_handled_num);
	}
}

void shot_req_handle() {
	printf("%d Start Shot Req Handle Task \n",  pros::millis());
	while (true) {
		if (shot_req_num > 0) {
			printf("%d Start handling first shot request \n", pros::millis());
			set_handled_vars(); //Make sure all handled vars are reset to false
			shot_req_handled_num = 0; //Make sure we start handling shot requests from index 0

			if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				pos.reset(0,0,0);
				flatten_against_wall();
				setDrive(0, -15, 0);
			}

			pos.reset(0, 0, 0);

			//Drive Handle 1
			if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				printf("%d S1 Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg));
			}
			shot_req[shot_req_handled_num].drive_turn_handled = true;

			//Shooter Angle Handle 1
			while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
			shot_req_handled_num++;

			if (shot_req_num > 1) {
				//Drive Handle 2
				if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
					printf("%d S2 Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
					turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg));
				}
				shot_req[shot_req_handled_num].drive_turn_handled = true;

				//Shooter Angle Handle 2
				while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
				shot_req_handled_num++;
			}

			shot_req_num = 0;
			shot_req_handled_num = 0;

		}
		pros::delay(10);
	}
}
