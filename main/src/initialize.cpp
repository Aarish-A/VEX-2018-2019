#include "main.h"
#include "util.hpp"
#include "config.hpp"
#include "puncher.hpp"
#include "drive.hpp"
#include "angler.hpp"
#include "intake.hpp"

using namespace pros;

void initialize() {
	log_init();
	log_ln("%d Finished Init ", pros::millis());

	buttons[DP_R1].button_press_time = BTN_PRESS_TIME;
	buttons[DP_L1].button_press_time = BTN_PRESS_TIME;

	// pun_init();
	// pun_cal();
  // drive_init();
  // angler_init();
  // intake_init();
	//
	// angler_cal();
	//log_ln("%d Init Done ", pros::millis(), x);

  ///pros::Task pun_task ((pros::task_fn_t)pun_handle, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Pun_Task");
	//pros::Task tracking_task ((pros::task_fn_t)pos.task(), (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Track_Task");
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
