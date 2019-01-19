#include "button.hpp"

button buttons[12];

void init_buttons() {

}

void update_buttons() {
  for(int i = 0; i < 12; i++) {
    buttons[i].last_pressed = buttons[i].pressed;
    buttons[i].pressed = ctrler.get_digital((pros::controller_digital_e_t)(i + pros::E_CONTROLLER_DIGITAL_L1));
    if (check_rising((pros::controller_digital_e_t)(i + pros::E_CONTROLLER_DIGITAL_L1))) buttons[i].last_pressed_time = pros::millis();
  }
}

bool check_rising(pros::controller_digital_e_t button) {
  return (buttons[button].pressed && !buttons[button].last_pressed);
}

bool check_falling(pros::controller_digital_e_t button) {
  return (!buttons[button].pressed && buttons[button].last_pressed);
}

bool check_single_press(pros::controller_digital_e_t button) {
  if ((pros::millis() - buttons[button].last_pressed) > buttons[button].button_press_time) {
    buttons[button].last_pressed = 0;
    return true;
  }
}

bool check_double_press(pros::controller_digital_e_t button1, pros::controller_digital_e_t button2) {
  if (((pros::millis() - buttons[button1].last_pressed) < buttons[button1].button_press_time) && check_rising(button2)) {
         return true;
     }
  if (((pros::millis() - buttons[button2].last_pressed) < buttons[button2].button_press_time) && check_rising(button1)) {
         return true;
     }
     return false;
}
