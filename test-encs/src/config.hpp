#pragma once
#include "main.h"

enum class auto_routines {
  FRONT,
  FRONT_PARK,
  BACK_MID_FIRST,
  BACK_FAR_FIRST,
  UNUSED4,
  UNUSED5,
  UNUSED6,
  UNUSED7,

  K_AUTO_ROUTINE_NUM
};

extern pros::Controller ctrler;
extern pros::Motor drive_fl;
extern pros::Motor drive_bl;
extern pros::Motor drive_fr;
extern pros::Motor drive_br;
extern pros::Motor angler;
extern pros::Motor intake;
extern pros::Motor decapper;
extern pros::Motor puncherLeft;
extern pros::Motor puncherRight;
extern pros::ADIEncoder enc_l;
extern pros::ADIEncoder enc_r;
extern pros::ADIEncoder enc_s;
extern pros::ADIEncoder enc_1;
extern pros::ADIEncoder enc_2;
extern pros::ADIEncoder enc_3;
extern pros::ADIEncoder enc_4;
extern pros::ADIAnalogIn autoPoti;
extern pros::ADIAnalogIn poti_1;
extern pros::ADIAnalogIn poti_2;
extern pros::ADIAnalogIn poti_3;
extern pros::ADILineSensor ball_sensor;
extern pros::Vision vision_sensor;

extern char game_side;
extern auto_routines current_auto_routine;

extern bool is_disabled;
