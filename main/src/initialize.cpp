#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"

void initialize() {
	printf("%d enter init()", pros::millis());
	//log_init();
	for (int i = 0; i < LOG_BUFFER_SIZE; i++) log_buffer[i] = 'c';
	pros::delay(100);
	log_ln(PROGRAM_FLOW, " Construct buffer_to_sd() task from log_init()");
	pros::Task buffer_to_sd_task((pros::task_fn_t) buffer_to_sd);
	printf("%d done log init", pros::millis());

  controls_init();

	Subsystem::reset_all();
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
