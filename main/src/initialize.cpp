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

void side_initialize();
void auto_routine_initialize();

void initialize() {
  printf("%d Start initialize\n", pros::millis());
  buttons[BTN_SHOT_L_T].button_press_time = BTN_PRESS_TIME;
	buttons[BTN_SHOT_L_M].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_SHOT_R_T].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_SHOT_R_M].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_DECAPPER_UP].button_press_time = BTN_PRESS_TIME;
  buttons[BTN_DECAPPER_DOWN].button_press_time = BTN_PRESS_TIME;
  side_initialize();
  auto_routine_initialize();

	log_init();
  printf("log init completed\n");
  // pros::delay(1000);
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

void auto_routine_initialize() {
  is_disabled = false;
  FILE* file = NULL;
  file = fopen("/usd/auto_routine.txt", "r");

  if (file == NULL) {
    printf("Could not open auto routine file");
  } else {
    fscanf(file, "%d", &current_auto_routine);
    printf("printed %d", current_auto_routine);
    fclose(file);
  }
}

void side_initialize() {
  FILE* file = NULL;
  file = fopen("/usd/game_side.txt", "r");

  if (file == NULL) {
    printf("Could not open game side file");
  } else {
    fscanf(file, "%c", &game_side);
    printf("printed %c", game_side);
    fclose(file);
  }
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  is_disabled = true;
  printf("  >>>%d IN DISABLED\n", pros::millis());
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
