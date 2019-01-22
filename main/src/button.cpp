#include "button.hpp"

button buttons[12];

void init_buttons() {
  // button_
}

void update_buttons() {
  for(int i = 0; i < 12; i++) {
    buttons[i].last_pressed = buttons[i].pressed;
    buttons[i].pressed = ctrler.get_digital((pros::controller_digital_e_t)(i + pros::E_CONTROLLER_DIGITAL_L1));
    if (check_rising(i)) buttons[i].last_pressed_time = pros::millis();
    else if (check_falling(i)) buttons[i].last_pressed_time = 0;
  }
}

bool check_rising(int button) {
  return (buttons[button].pressed && !buttons[button].last_pressed);
}

bool check_falling(int button) {
  return (!buttons[button].pressed && buttons[button].last_pressed);
}

bool check_single_press(int button) {
  if (buttons[button].last_pressed_time && (pros::millis() - buttons[button].last_pressed_time) >= buttons[button].button_press_time) {
    buttons[button].last_pressed_time = 0;
    return true;
  }
  else return false;
}

bool check_double_press(int button1, int button2) {
  if (((pros::millis() - buttons[button1].last_pressed_time) < buttons[button1].button_press_time) && check_rising(button2)) {
    buttons[button1].last_pressed_time = 0;
    buttons[button2].last_pressed_time = 0;
    return true;
  }
  if (((pros::millis() - buttons[button2].last_pressed_time) < buttons[button2].button_press_time) && check_rising(button1)) {
    buttons[button1].last_pressed_time = 0;
    buttons[button2].last_pressed_time = 0;
    return true;
  }
  return false;
}
