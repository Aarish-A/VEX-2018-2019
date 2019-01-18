#pragma once

#include "main.h"
#include "config.hpp"

struct btn_info {
  pros::controller_digital_e_t btn_name; //Btn name
  bool pressed; //Wether or not the button has been pressed this cycle
  void check_pressed(); //Set pressed to true if a rising edge has been detected; otherwise false. Call in main loop of opcontrol
};

constexpr int BTN_PRESS_TIME = 50; //Max amount of time b/w first and second press during a double press (measured in ms)
class btn_dp_detector {
  pros::controller_digital_e_t btn_start; //Beginning of doule press group (assumes all the buttons in the group are also adjacent in the enumeration)
  pros::controller_digital_e_t btn_end; //End of doule press group (assumes all the buttons in the group are also adjacent in the enumeration)
  int timer; //Timer -> Should always be b/w 0 & BTN_PRESS_TIME
  pros::controller_digital_e_t btn_first_pressed; //The btn in a double pressed group that is pressed first when the timer is not running. Set using set_first_pressed

public:
  btn_dp_detector(pros::controller_digital_e_t btn_start, pros::controller_digital_e_t btn_end);
  void set_first_pressed(); //If any of the buttons in the double pressed group are pressed, sets_btn_first_pressed and timer (to millis()+50).
                              //Only executes if timer is currently not running
  void reset_timer(); //Resets the timer. Call after either a single or double pressed has been detected, and the desired action has been executed
  int get_timer(); //Returns timer val -> should always be b/w 0 & 50
  pros::controller_digital_e_t  get_first_pressed(); //Returns name of first pressed bttn

  void override_first_pressed(pros::controller_digital_e_t btn_override); //If the btn_first_pressed was set to the btn_override, it will be overwriten to pun_fatal_disable
                                                                          //Effectively disables a button in a double press group, during a certain period
};

extern btn_info btn[12]; //Btn array to store all the buttons.
                        //To access elements of this array, call btn[BTN_ENUM_VAL - 6] b/c the pros enumeration values begin at 6, whereas the indices of the array begin at 0

const bool ENABLE_BTN_PRESS_LOGS = true;
