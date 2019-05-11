#include "controller.hpp"

uint32_t pilons::Controller::last_screen_update_time = 0;
bool pilons::Controller::master_printing = true;
const std::string pilons::Controller::button_names[12] = {"L1", "L2", "R1", "R2", "UP", "DOWN", "LEFT", "RIGHT", "X", "B", "Y", "A"};

/* Constructor */
pilons::Controller::Controller(pros::controller_id_e_t id, std::string controller_name) : pros::Controller(id), controller_name(controller_name) {
  this->buttons[BTN_SHOT_L_T].button_press_time = 100;
	this->buttons[BTN_SHOT_L_M].button_press_time = 100;
  this->buttons[BTN_SHOT_R_T].button_press_time = 100;
  this->buttons[BTN_SHOT_R_M].button_press_time = 100;
  this->buttons[BTN_PUNCHER_RESET_UP].button_press_time = 100;
  this->buttons[BTN_PUNCHER_RESET_RIGHT].button_press_time = 100;
}

/* Private Functions */
bool pilons::Controller::check_rising(int button) {
  return (this->buttons[button].pressed && !this->buttons[button].last_pressed);
}

bool pilons::Controller::check_falling(int button) {
  return (!this->buttons[button].pressed && this->buttons[button].last_pressed);
}

bool pilons::Controller::pressed(int button) {
  return this->buttons[button].pressed;
}

/* Public Functions */
void pilons::Controller::update() {
  if (pros::millis() > pilons::Controller::last_screen_update_time + pilons::Controller::SCREEN_UPDATE_INTERVAL) {
    if ((this->controller_name == "Master" && pilons::Controller::master_printing) || (this->controller_name == "Partner" && !pilons::Controller::master_printing)) {
      this->Controller::print(this->update_line_number, 0, (this->screen_lines[this->update_line_number]).c_str());
      if (this->update_line_number < 2) this->update_line_number++;
      else this->update_line_number = 0;
    }
    pilons::Controller::master_printing = !pilons::Controller::master_printing;
    pilons::Controller::last_screen_update_time = pros::millis();
  }

  for(int i = 0; i < 12; i++) {
    this->buttons[i].last_pressed = this->buttons[i].pressed;
    this->buttons[i].pressed = this->get_digital((pros::controller_digital_e_t)(i + pros::E_CONTROLLER_DIGITAL_L1));
    if (this->check_rising(i)) this->buttons[i].last_pressed_time = pros::millis();
    // else if (this->check_falling(i)) this->buttons[i].last_pressed_time = 0;
    if (this->check_single_press(i)) this->single_pressed_queue.push_back(i);
  }
  if (this->single_pressed_queue.size() > 0) {
    this->single_pressed = this->single_pressed_queue[0];
    this->single_pressed_queue.pop_front();
  } else single_pressed = -1;
}

int8_t pilons::Controller::get_analog(pros::controller_analog_e_t joystick, uint8_t deadzone) {
  int8_t val = this->pros::Controller::get_analog(joystick);
  if (deadzone) {
    int8_t after_dz = (abs(val) > deadzone ? val : 0);
    if (after_dz != 0) return (int8_t)((abs(after_dz) - deadzone) * (127.0) / (127 - deadzone)) * sgn(after_dz);
    else return 0;
  } else return val;
}

bool pilons::Controller::check_single_press(int button) {
  if (this->buttons[button].last_pressed_time && (pros::millis() - this->buttons[button].last_pressed_time >= this->buttons[button].button_press_time)) {
    this->buttons[button].last_pressed_time = 0;
    log_ln(USER_IN, "Single Pressed %s on %s controller", (this->button_names[button]).c_str(), (this->controller_name).c_str());
    return true;
  } else return false;
}

bool pilons::Controller::check_double_press(int button1, int button2) {
  if (pros::millis() - this->buttons[button1].last_pressed_time < this->buttons[button1].button_press_time && this->check_rising(button2)) {
    this->buttons[button1].last_pressed_time = 0;
    this->buttons[button2].last_pressed_time = 0;
    return true;
  } else if (pros::millis() - this->buttons[button2].last_pressed_time < this->buttons[button2].button_press_time && this->check_rising(button1)) {
    this->buttons[button1].last_pressed_time = 0;
    this->buttons[button2].last_pressed_time = 0;
    return true;
  } else return false;
}

void pilons::Controller::write_line(uint8_t line, const char* format, ...) {
  va_list args;
  va_start(args, format);
  char buffer[64];
  vsprintf(buffer, format, args);
  this->screen_lines[line] = buffer;
  while (this->screen_lines[line].size() < 16) {
    this->screen_lines[line] += " ";
  }
  va_end(args);
}
