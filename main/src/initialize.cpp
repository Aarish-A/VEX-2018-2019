#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"

void initialize() {
	log_init();
	printf("%d done log init", pros::millis());
	pros::delay(50);
	printf("	%d After wait init() \n", pros::millis());
	if (sd_logging_enabled) {
		printf("	%d Start Log Buffer from init() \n", pros::millis());
		pros::Task buffer_to_sd_task((pros::task_fn_t) buffer_to_sd);
	}
/*
	log_ln(PROGRAM_FLOW, "DONE LOG_INIT");
  controls_init();

	Subsystem::reset_all();
	log_ln(PROGRAM_FLOW, "%d Init first log", pros::millis());

	log_ln(PROGRAM_FLOW, "%d 1 Init first log", pros::millis());
	log_ln(PROGRAM_FLOW, "%d 2 Init first log", pros::millis());
	log_ln(PROGRAM_FLOW, "%d 3 Init first log", pros::millis());
	log_ln(PROGRAM_FLOW, "%d 4 Init first log", pros::millis());
	log_ln(PROGRAM_FLOW, "%d 5 Init first log", pros::millis());
  {}//log_ln(LOG_DRIVE, "%d Finished Init", pros::millis());
*/
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
