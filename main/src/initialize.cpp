#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "logs.hpp"
#include "config.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"

void initialize() {
	log_init();
  controls_init();

	Subsystem::reset_all();

  log_ln(LOG_DRIVE, "%d Finished Init", pros::millis());
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
