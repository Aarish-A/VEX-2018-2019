#include "button.hpp"

btn_info btn[12] {
  {pros::E_CONTROLLER_DIGITAL_L1, false },
  {pros::E_CONTROLLER_DIGITAL_L2, false},
  {pros::E_CONTROLLER_DIGITAL_R1, false},
  {pros::E_CONTROLLER_DIGITAL_R2, false},
  {pros::E_CONTROLLER_DIGITAL_UP, false},
  {pros::E_CONTROLLER_DIGITAL_DOWN, false},
  {pros::E_CONTROLLER_DIGITAL_LEFT, false},
  {pros::E_CONTROLLER_DIGITAL_RIGHT, false},
  {pros::E_CONTROLLER_DIGITAL_X, false},
  {pros::E_CONTROLLER_DIGITAL_B, false},
  {pros::E_CONTROLLER_DIGITAL_Y, false},
  {pros::E_CONTROLLER_DIGITAL_A, false}
};

void btn_info::check_pressed() {
  //pressed = btn[btn_name-6].pressed;
  pressed = ctrler.get_digital_new_press(btn_name);
}

btn_dp_detector::btn_dp_detector(pros::controller_digital_e_t btn_start, pros::controller_digital_e_t btn_end) {
  this->btn_start = btn_start;
  this->btn_end = btn_end;
}
void btn_dp_detector::set_first_pressed() {
  for (int i = (btn_start-6); i <= (btn_end-6); i++) {
    if (!timer && btn[i].pressed) {
      timer = pros::millis() + BTN_PRESS_TIME;
      btn_first_pressed = btn[i].btn_name;
    }
  }
}

void btn_dp_detector::reset_timer() {
  timer = 0;
}

int btn_dp_detector::get_timer() {
  return timer;
}

pros::controller_digital_e_t btn_dp_detector::get_first_pressed() {
  return btn_first_pressed;
}
