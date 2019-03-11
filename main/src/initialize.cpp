#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"

constexpr int BUF_TEST_SIZE = 2000;
char buf_test[BUF_TEST_SIZE];

void initialize() {
	printf("%d enter init()", pros::millis());
	log_init();
	printf("%d done log init", pros::millis());
	pros::delay(50);
	printf("	%d After wait init() \n", pros::millis());
	if (sd_logging_enabled) {
		printf("	%d Start Log Buffer from init() \n", pros::millis());
		pros::Task buffer_to_sd_task((pros::task_fn_t) buffer_to_sd);
	}
	pros::delay(50);

	log_ln(PROGRAM_FLOW, "DONE LOG_INIT");
  controls_init();

	Subsystem::reset_all();
	/*
	for (int i = 0; i < BUF_TEST_SIZE; i++) {
		buf_test[i] = (char)i;
	}
	buf_test[BUF_TEST_SIZE-1] = '\n';
	int dif_sum = 0;;
	for (int i = 0; i < 500; i++) {
		while (log_file == NULL) {
      log_file = fopen(log_file_name, log_mode);
      printf("  \n>>>>>>>>%d Opening file \n\n", pros::millis());
      pros::delay(2);
    }
		uint32_t s_time = pros::millis();
		printf("BEFORE FLUSH %d | T:%d \n", i, pros::millis());
		int flush_amount = fwrite(buf_test, sizeof(buf_test[0]), BUF_TEST_SIZE, log_file);
		close_log_file();
		int dif = pros::millis() - s_time;
		printf("	>> AFTER FLUSH %d | T:%d | Dif: %d\n", i, pros::millis(), dif);
		dif_sum += dif;
	}
	printf("\n\n\n	>>>dif_avg: %f \n\n\n", (double)dif_sum/1000);
*/

	for (int i = 0; i < 60; i++) {
		log_ln(PROGRAM_FLOW, "%d Init log test", i);
		pros::delay(100);
	}


	pros::delay(5000);
	printf("\n\n%d INIT WAIT 5 SEC \n\n", pros::millis());
  {}//log_ln(LOG_DRIVE, "%d Finished Init", pros::millis());

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
