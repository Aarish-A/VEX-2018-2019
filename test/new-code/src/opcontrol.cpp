#include "main.h"
#include "button.hpp"
#include "controls.hpp"

using namespace pros;

void opcontrol() {
	printf("%d Start opcontrol\n", pros::millis());
	ctrler.print(2, 0, "ST  ");

	while (true) {
		update_buttons();

		printf("R1: %d C: %d L: %d T: %d %d %d\n", pros::millis(), (int)(buttons[DP_R1].pressed), (int)(buttons[DP_R1].last_pressed), buttons[DP_R1].last_pressed_time, (int)check_rising(DP_R1), buttons[DP_R1].button_press_time);
		printf("L1: %d C: %d L: %d T: %d %d\n", pros::millis(), (int)(buttons[DP_L1].pressed), (int)(buttons[DP_L1].last_pressed), buttons[DP_L1].last_pressed_time, (int)check_rising(DP_L1));


		// printf("%d C: %d L: %d T: %d %d\n", pros::millis(), (int)(buttons[SP_A].pressed), (int)(buttons[SP_A].last_pressed), buttons[SP_A].last_pressed_time, (int)check_rising(SP_A));
		if (check_double_press(DP_R1, DP_L1)) printf("%d DOUBLE PRESSED\n", millis()); //ctrler.print(2, 0, "DOUBLE PRESS   ");
		else if (check_single_press(SP_A)) printf("%d SINGLE PRESSED\n", millis()); //ctrler.print(2, 0, "SINGLE PRESS   ");

		delay(10);
	}
}
