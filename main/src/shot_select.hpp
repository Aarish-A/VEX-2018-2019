#pragma once

#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "auto.hpp"
#include "angle_target.hpp"
#include "button.hpp"
#include "angler.hpp"
#include "controls.hpp"
#include "intake.hpp"
#include "vision.hpp"

/* Flags */
constexpr bool SHOT_DRIVE_BRAKE = false;

/* Constants */
constexpr double FLAG_WIDTH = 16.50;

constexpr double ANGLER_BALL = 13;
constexpr int ANGLER_DZ = 30;

constexpr double ANGLER_PU_POS = 85;//ANGLER_BALL * 7;

constexpr double ANGLER_CAP_PU_POS = 220;

constexpr double ANGLER_CAP_FLIP_POS = 50;

constexpr double ANGLER_FLIP_FLAG_POS = 105;

constexpr double ANGLER_PARK_POS = 240;

constexpr double ANGLER_BOT_LIM_POS = 40;
constexpr double ANGLER_TOP_LIM_POS = 320;

enum Dir {
  Dir_Centre,
  Dir_Left,
  Dir_Right
};

enum FieldPos {
  FieldPos_Front,
  FieldPos_Back,
  FieldPos_PF,
  FieldPos_PF_Back_Red,
  FieldPos_PF_Back_Blue
};

struct ShotPos {
  FieldPos field_pos;
  int top;
  int mid;

  ShotPos(FieldPos field_pos, int top, int mid) : field_pos(field_pos), top(top), mid(mid) {}
};

extern volatile ShotPos front_SP;
extern volatile ShotPos pf_SP;
extern volatile ShotPos auto_SP;
extern volatile ShotPos pf_back_SP;
extern volatile ShotPos back_SP;

struct ShotSelect {
  int angle_targ;
  Dir turn_dir;
  FieldPos field_pos;
  vector flag_pos;
  bool drive_turn_handled = true;
  int angler_to;
  bool shot_handled;
};

extern bool angler_pu_flag;

/* Helper Functions */
void set_field_pos(FieldPos field_pos);
void set_angle_targ(bool top);
void set_turn_dir(Dir turn_dir);
void set_handled_vars();
void set_handled_vars_all();

void set_shot_req(bool top, Dir turn_dir) ;

void shot_req_make();
void shot_req_handle(void *param);


/* Shot Num */
extern ShotSelect shot_req[2];

extern int shot_req_num;
extern int shot_req_handled_num;

extern bool shot_cancel_pressed;

extern bool shot_pun_go;

/* Task Handle */
void shot_req_handle_stop_task();
void shot_req_handle_start_task();
extern pros::Task* shot_req_handle_task;

void inc_shot_req_num();
void dec_shot_req_num();

/* Time Outs */
const int ANGLER_REACH_T0 = 1200;
