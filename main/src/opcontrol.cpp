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

using namespace pros;

void opcontrol() {
	printf("%d Start opcontrol\n", pros::millis());
	shot_req_handle_start_task();
	// ctrler.print(2, 0, "RUNNING");

	while (true) {
		pos.update();
		update_buttons();
		shot_req_make();
		pun_handle();
		drive_handle();
		intake_handle();
		angler_handle();
		decapper_handle();
		//printf("%d\n", (int)decapper.get_position());
		delay(10);
	}
}
