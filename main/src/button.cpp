#include "button.hpp"

pros::Controller ctrler(pros::E_CONTROLLER_MASTER);
pros::Controller partner(pros::E_CONTROLLER_PARTNER);

button buttons[12];
button partner_joy[12];
const char* button_names[12] = {"L1", "L2", "R1", "R2", "UP", "DOWN", "LEFT", "RIGHT", "X", "B", "Y", "A"};

void init_buttons() {
  // button_
}

void update_buttons() {
  for(int i = 0; i < 12; i++) {
    buttons[i].last_pressed = buttons[i].pressed;
    buttons[i].pressed = ctrler.get_digital((pros::controller_digital_e_t)(i + pros::E_CONTROLLER_DIGITAL_L1));
    if (check_rising(i)) buttons[i].last_pressed_time = pros::millis();
    else if (check_falling(i)) buttons[i].last_pressed_time = 0;

    if (partner_connected) {
      partner_joy[i].last_pressed = partner_joy[i].pressed;
      partner_joy[i].pressed = partner.get_digital((pros::controller_digital_e_t)(i + pros::E_CONTROLLER_DIGITAL_L1));
      if (check_rising_partner(i)) partner_joy[i].last_pressed_time = pros::millis();
      else if (check_falling_partner(i)) partner_joy[i].last_pressed_time = 0;
    }
  }
}

bool check_rising(int button) {
  return (buttons[button].pressed && !buttons[button].last_pressed);
}

bool check_falling(int button) {
  return (!buttons[button].pressed && buttons[button].last_pressed);
}

bool check_rising_partner(int button) {
  return (partner_joy[button].pressed && !partner_joy[button].last_pressed);
}

bool check_falling_partner(int button) {
  return (!partner_joy[button].pressed && partner_joy[button].last_pressed);
}


bool check_single_press(int button, bool partner, bool overridable) {
  if(!partner || (!partner_connected && overridable)) {
    if (buttons[button].last_pressed_time && (pros::millis() - buttons[button].last_pressed_time) >= buttons[button].button_press_time) {
      buttons[button].last_pressed_time = 0;
      {}//log_ln(LOG_JOYSTICK, "%d Button %s Single Pressed - Main", pros::millis(), button_names[button]);
      return true;
    } else return false;
  } else {
    if (partner_joy[button].last_pressed_time && (pros::millis() - partner_joy[button].last_pressed_time) >= partner_joy[button].button_press_time) {
      partner_joy[button].last_pressed_time = 0;
      {}//log_ln(LOG_JOYSTICK, "%d Button %s Single Pressed - Partner", pros::millis(), button_names[button]);
      return true;
    } else return false;
  }
}

bool check_double_press(int button1, int button2) {
  if (((pros::millis() - buttons[button1].last_pressed_time) < buttons[button1].button_press_time) && check_rising(button2)) {
    buttons[button1].last_pressed_time = 0;
    buttons[button2].last_pressed_time = 0;
    return true;
  }
  else if (((pros::millis() - partner_joy[button1].last_pressed_time) < partner_joy[button1].button_press_time) && check_rising_partner(button2)) {
    partner_joy[button1].last_pressed_time = 0;
    partner_joy[button2].last_pressed_time = 0;
    return true;
  }
  if (((pros::millis() - buttons[button2].last_pressed_time) < buttons[button2].button_press_time) && check_rising(button1)) {
    buttons[button1].last_pressed_time = 0;
    buttons[button2].last_pressed_time = 0;
    return true;
  }
  else if (((pros::millis() - partner_joy[button2].last_pressed_time) < partner_joy[button2].button_press_time) && check_rising_partner(button1)) {
    partner_joy[button1].last_pressed_time = 0;
    partner_joy[button2].last_pressed_time = 0;
    return true;
  }
  return false;
}
