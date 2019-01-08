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
	shot_req[shot_req_num-1].flag_pos.y = 120;
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
	  if (ctrler.get_digital_new_press(BTN_SHOT_L_T))
	  {
	    inc_shot_req_num();
	    set_angle_targ(true);
	    set_turn_dir(Dir_Left);
			set_handled_vars();
			printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
	  }
	  else if (ctrler.get_digital_new_press(BTN_SHOT_L_M))
	  {
	    inc_shot_req_num();
	    set_angle_targ(false);
	    set_turn_dir(Dir_Left);
			set_handled_vars();
			printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
	  }
	  else if (ctrler.get_digital_new_press(BTN_SHOT_R_T))
	  {
	    inc_shot_req_num();
	    set_angle_targ(true);
	    set_turn_dir(Dir_Right);
			set_handled_vars();
			printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
	  }
	  else if (ctrler.get_digital_new_press(BTN_SHOT_R_M))
	  {
	    inc_shot_req_num();
	    set_angle_targ(false);
	    set_turn_dir(Dir_Right);
			set_handled_vars();
			printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
	  }
		else if (ctrler.get_digital_new_press(BTN_SHOOT_CANCEL)) {
			shot_cancel_pressed = true;
			shot_req_handle_task.suspend();
			setDrive(0);

			shot_req_num = 0;
			shot_req_handled_num = 0;
			set_handled_vars();
		}
	}

/*
	if (	ctrler.get_digital_new_press(BTN_FIELD_FRONT) || ctrler.get_digital_new_press(BTN_FIELD_PF)
			|| ctrler.get_digital_new_press(BTN_FIELD_BACK) ) {
				printf("%d Shot Field Pos Set | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d | 2angle:%d, 2turn:%d\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[1].angle_targ, shot_req[1].turn_dir);
			}
	if (ctrler.get_digital_new_press(BTN_SHOT_L_T) || ctrler.get_digital_new_press(BTN_SHOT_L_M)
			|| ctrler.get_digital_new_press(BTN_SHOT_R_T) || ctrler.get_digital_new_press(BTN_SHOT_R_M)) {
				printf("%d Shot Set | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d | 2angle:%d, 2turn:%d\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[1].angle_targ, shot_req[1].turn_dir);
			}
*/
}

void shot_req_handle() {
	printf("%d Start Shot Req Handle Task \n",  pros::millis());
	while (true) {
		if (shot_req_num > 0) {
			set_handled_vars(); //Make sure all handled vars are reset to false
			shot_req_handled_num = 0; //Make sure we start handling shot requests from index 0

			if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				setDrive(-50);
				pros::delay(100);
				while (pos.velLocal.y < -1) {
					printf("%d Shot Back Up(%f, %f, %f) Vel(%f, %f) VeelLoc(%f, %f)\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.velLocal.x, pos.velLocal.y);
					pros::delay(10);
				}
			}

			pos.reset(0, 0, 0);

			//Drive Handle 1
			printf("%d S1 Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
			if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back)
				turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg));
			shot_req[shot_req_handled_num].drive_turn_handled = true;

			//Shooter Angle Handle 1
			while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
			shot_req_handled_num++;

			if (shot_req_num > 1) {
				//Drive Handle 2
				printf("%d S2 Turn to face %f, %f \n", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back)
					turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg));
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
