#pragma once

#include "main.h"
#include "config.hpp"

struct btn_info {
  pros::controller_digital_e_t btn_name;
  bool pressed;
  void check_pressed();
};

constexpr int BTN_PRESS_TIME = 50;
class btn_dp_detector {
  pros::controller_digital_e_t btn_start;
  pros::controller_digital_e_t btn_end;
  int timer;
  pros::controller_digital_e_t btn_first_pressed;

public:
  btn_dp_detector(pros::controller_digital_e_t btn_start, pros::controller_digital_e_t btn_end);
  void set_first_pressed();
  void reset_timer();
  int get_timer();
  pros::controller_digital_e_t  get_first_pressed();

  void override_first_pressed(pros::controller_digital_e_t btn_override) ;
};

extern btn_info btn[12];
