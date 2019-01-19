#include "main.h"
#include "button.hpp"
#include "controls.hpp"

using namespace pros;

void opcontrol() {
	printf("%d Start opcontrol\n", pros::millis());
	ctrler.print(2, 0, "ST  ");

	while (true) {
		update_buttons();


		if (check_double_press(DP_R1, DP_L1)) printf("DOUBLE PRESSED\n"); //ctrler.print(2, 0, "DOUBLE PRESS   ");
		else if (check_single_press(SP_A)) printf("SINGLE PRESSED\n"); //ctrler.print(2, 0, "SINGLE PRESS   ");
		else if (check_falling(FE_X)) printf("FALLING EDGED\n"); //ctrler.print(2, 0, "FALLING EDGE    ");


		delay(10);
	}
}
