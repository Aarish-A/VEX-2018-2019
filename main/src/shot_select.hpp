#pragma once

#include "main.h"
#include "config.hpp"

enum Dir {
  Dir_Centre,
  Dir_Left,
  Dir_Right
};

enum Field_Pos {
  Field_Pos_Front,
  Field_Pos_Back,
  Field_Pos_PF,
  Field_Pos_PF_Back
};

struct Shot_Pos {
  Field_Pos field_pos;
  int top;
  int mid;

  Shot_Pos(Field_Pos field_pos, int top, int mid) : field_pos(field_pos), top(top), mid(mid) {}
};

extern Shot_Pos front_SP;
extern Shot_Pos pf_SP;
extern Shot_Pos pf_back_SP;
extern Shot_Pos back_SP;

struct Shot_Select {
  int angle_targ;
  Dir turn_dir;
  Field_Pos field_pos;
};

void set_field_pos(Field_Pos field_pos);
void set_angle_targ(bool top);
void set_turn_dir(Dir turn_dir);
void shot_req_handle();

/* Shot Num */
extern Shot_Select shot_req[2];

extern int shot_req_num;
void inc_shot_req_num();
void dec_shot_req_num();
