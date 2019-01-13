#include "main.h"
#include "util.hpp"
#include "config.hpp"
#include "puncher.hpp"
#include "drive.hpp"
#include "angler.hpp"
#include "intake.hpp"

using namespace pros;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	log_init();
	log("%d Finished Init \n", pros::millis());
	
	pun_init();
	pun_cal();
  drive_init();
  angler_init();
  intake_init();

	angler_cal();
	//log("%d Init Done \n", pros::millis(), x);

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
