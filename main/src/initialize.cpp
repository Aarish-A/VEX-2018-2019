#include "main.h"
#include "controls.hpp"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "menu.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/angler.hpp"
#include "libraries/task.hpp"
#include "macros/shot_queueing.hpp"

void initialize() {
	log_init();
	menu_init();
	shot_queue_init();
	Subsystem::reset_all();
	while(Subsystem::any_resetting()) {
		Subsystem::update_all();
		pros::delay(2);
	}
  log_ln(PROGRAM_FLOW, "%d Finished Init", pros::millis());
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	pilons::Task::stop_all_tasks();

	m_drive_bl.move(0);
	m_drive_br.move(0);
	m_drive_fl.move(0);
	m_drive_fr.move(0);
	m_puncher.move(0);
	m_angler.move(0);
	m_capper.move(0);
	m_intake.move(0);
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
