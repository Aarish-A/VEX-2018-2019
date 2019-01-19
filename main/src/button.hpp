#pragma once

#include "main.h"
#include "config.hpp"

constexpr uint32_t BTN_PRESS_TIME = 50;

struct button {
  bool pressed = 0;
  bool last_pressed;
  uint32_t last_pressed_time = 0;
  uint32_t button_press_time = 0;
};

extern button buttons[12];

void init_buttons();
void update_buttons();
bool check_rising(int button);
bool check_falling(int button);
bool check_single_press(int button);
bool check_double_press(int button1, int button2);

const bool ENABLE_BTN_PRESS_LOGS = true;
