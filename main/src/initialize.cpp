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
#include "shot_select.hpp"

void side_initialize();
void auto_routine_initialize();

void initialize() {
  log_ln(LOG_AUTO, "   --- %d START INITIALIZE --- \n", pros::millis());
  buttons[BTN_SHOT_L_T].button_press_time = BTN_PRESS_TIME;
	buttons[BTN_SHOT_L_M].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_SHOT_R_T].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_SHOT_R_M].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_DECAPPER_UP].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_DECAPPER_DOWN].button_press_time = BTN_PRESS_TIME;
  is_disabled = false;
	log_init();
  printf("log init completed\n");

  ctrler.set_text(0, 0, "               ");
  pros::delay(60);
  ctrler.set_text(1, 0, "               ");
  pros::delay(60);
  partner.set_text(0, 0, "               ");
  pros::delay(60);
  partner.set_text(1, 0, "               ");

  // pros::delay(1000);
  controls_init();
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
void disabled() {
  is_disabled = true;
  shot_req_handle_stop_task();
  auto_update_stop_task();
  pun_set(PUN_HOLD_PWR);
  log_ln(LOG_AUTO, "   --- %d START DISABLED --- \n", pros::millis());
  setDrive(0);
  angler.move(0);
}

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
