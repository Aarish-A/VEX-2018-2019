#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "logs.hpp"
#include "config.hpp"
using namespace pros;

uint32_t print_time = 0;
void update_controller_lcd();
void update_game_side();

void opcontrol() {
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());

	while (true) {
		update_buttons();
		intake.update();

		delay(10);
	}
}
