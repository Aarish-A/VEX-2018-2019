#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "angler.hpp"
#include "puncher.hpp"
#include "intake.hpp"
#include "drive.hpp"
#include "logs.hpp"
#include "gui.hpp"
#include "decapper.hpp"

void initialize() {
  printf("%d Start initialize\n", pros::millis());
  buttons[BTN_SHOT_L_T].button_press_time = BTN_PRESS_TIME;
	buttons[BTN_SHOT_L_M].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_SHOT_R_T].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_SHOT_R_M].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_DECAPPER_UP].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_DECAPPER_DOWN].button_press_time = BTN_PRESS_TIME;

	log_init();

	gui_init();
	pun_init();
	pun_cal();
  drive_init();
  angler_init();
  intake_init();
	angler_cal();
  decapper_cal();
  log_ln(LOG_DRIVE, "%d Finished Init ", pros::millis());
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}
