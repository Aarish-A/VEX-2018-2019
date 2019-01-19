#pragma once

#include "main.h"
#include "config.hpp"

constexpr int BTN_PRESS_TIME = 50;

struct button {
  bool pressed;
  bool last_pressed;
  uint32_t last_pressed_time;
  uint32_t button_press_time = 0;
  bool button_recognized = false;
};

button buttons[12];

void update_buttons();
bool check_rising(pros::controller_digital_e_t button);
bool check_falling(pros::controller_digital_e_t button);
bool check_single_press(pros::controller_digital_e_t button);
bool check_double_press(pros::controller_digital_e_t button1, pros::controller_digital_e_t button2);

const bool ENABLE_BTN_PRESS_LOGS = true;
