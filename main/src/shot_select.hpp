#pragma once

#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "auto.hpp"
#include "angle_target.hpp"

enum Dir {
  Dir_Centre,
  Dir_Left,
  Dir_Right
};

enum FieldPos {
  FieldPos_Front,
  FieldPos_Back,
  FieldPos_PF,
  FieldPos_PF_Back
};

struct ShotPos {
  FieldPos field_pos;
  int top;
  int mid;

  ShotPos(FieldPos field_pos, int top, int mid) : field_pos(field_pos), top(top), mid(mid) {}
};

extern ShotPos front_SP;
extern ShotPos pf_SP;
extern ShotPos pf_back_SP;
extern ShotPos back_SP;

struct ShotSelect {
  int angle_targ;
  Dir turn_dir;
  FieldPos field_pos;
  vector flag_pos;

  bool drive_turn_handled;
  bool shot_handled;
};

void set_field_pos(FieldPos field_pos);
void set_angle_targ(bool top);
void set_turn_dir(Dir turn_dir);
void set_handled_vars();

void set_shot_req(bool top, Dir turn_dir) ;

void shot_req_make();
void shot_req_handle();

/* Shot Num */
extern ShotSelect shot_req[2];

extern int shot_req_num;
extern int shot_req_handled_num;

extern bool shot_cancel_pressed;

extern pros::Task shot_req_handle_task;

void inc_shot_req_num();
void dec_shot_req_num();

/* Btn Pressed Timer */
namespace shot_queue_btn{
  enum shot_queue_btn{
    l_t, l_m, r_t, r_m
  };
  struct btn_info {
    pros::controller_digital_e_t btn_name;
    bool pressed;
    void set_pressed() {
      pressed = ctrler.get_digital_new_press(btn_name);
    }
  };

  extern btn_info shot_queue_btn[4];
  extern int btn_queue_timer;
  extern pros::controller_digital_e_t btn_queue_pressed;
  constexpr int BTN_PRESS_TIME = 50;
}
