#include "shot_select.hpp"

/* Shot Positions */
ShotPos front_SP (FieldPos_Front, 90, 70);
ShotPos pf_SP (FieldPos_PF, 90, 70);
ShotPos pf_back_SP (FieldPos_PF_Back, 90, 70);
ShotPos back_SP (FieldPos_Back, 90, 70);

ShotSelect shot_req[2];
int shot_req_drive_handled;
int shot_req_num = 0;

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

void shot_req_handle() {
  //Set Field Pos
  if (shot_req_num == 0)
  {
    if (ctrler.get_digital_new_press(BTN_FIELD_FRONT)) set_field_pos(FieldPos_Front);
    else if (ctrler.get_digital_new_press(BTN_FIELD_PF)) set_field_pos(FieldPos_PF);
    else if (ctrler.get_digital_new_press(BTN_FIELD_BACK)) set_field_pos(FieldPos_Back);
  }

  //Set other shot constants
  if (ctrler.get_digital_new_press(BTN_SHOT_L_T))
  {
    inc_shot_req_num();
    set_angle_targ(true);
    set_turn_dir(Dir_Left);
		printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
  }
  else if (ctrler.get_digital_new_press(BTN_SHOT_L_M))
  {
    inc_shot_req_num();
    set_angle_targ(false);
    set_turn_dir(Dir_Left);
		printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
  }
  else if (ctrler.get_digital_new_press(BTN_SHOT_R_T))
  {
    inc_shot_req_num();
    set_angle_targ(true);
    set_turn_dir(Dir_Right);
		printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
  }
  else if (ctrler.get_digital_new_press(BTN_SHOT_R_M))
  {
    inc_shot_req_num();
    set_angle_targ(false);
    set_turn_dir(Dir_Right);
		printf("%d Shot Req | RNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f) | 2angle:%d, 2turn:%d (%f, %f)\n", pros::millis(), shot_req_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
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
