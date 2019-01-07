#include "shot_select.hpp"

ShotPos front_SP (FieldPos_Front, 90, 70);
ShotPos pf_SP (FieldPos_PF, 90, 70);
ShotPos pf_back_SP (FieldPos_PF, 90, 70);
ShotPos back_SP (FieldPos_Back, 90, 70);

ShotSelect shot_req[2];
int shot_req_num = 0;

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
  }
  else if (ctrler.get_digital_new_press(BTN_SHOT_L_M))
  {
    inc_shot_req_num();
    set_angle_targ(false);
    set_turn_dir(Dir_Left);
  }
  else if (ctrler.get_digital_new_press(BTN_SHOT_R_T))
  {
    inc_shot_req_num();
    set_angle_targ(true);
    set_turn_dir(Dir_Right);
  }
  else if (ctrler.get_digital_new_press(BTN_SHOT_R_M))
  {
    inc_shot_req_num();
    set_angle_targ(false);
    set_turn_dir(Dir_Right);
  }

}
