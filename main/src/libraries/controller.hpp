#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/util.hpp"
#include <stdarg.h>

namespace pilons {
  class Controller final : pros::Controller {

  private:
    std::string controller_name;
    static const uint32_t BUTTON_PRESS_TIME = 50;
    static const std::string button_names[12];

    struct button {
      bool pressed = 0;
      bool last_pressed;
      uint32_t last_pressed_time = 0;
    };

    button buttons[12];

    bool check_rising(int button);
    bool check_falling(int button);

  public:
    Controller(pros::controller_id_e_t id, std::string controller_name);

    void update();
    int8_t get_analog(pros::controller_analog_e_t joystick, uint8_t deadzone);
    bool check_single_press(int button);
    bool check_double_press(int button1, int button2);
    int32_t print(uint8_t line, uint8_t col, const char* fmt, ...);
  };
};
