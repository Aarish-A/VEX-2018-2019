#include "main.h"
#include "button.hpp"
#include "controls.hpp"

using namespace pros;

void opcontrol() {

	while (true) {
		update_buttons();


		if (check_double_press(DP_R1, DP_L1)) ctrler.print(2, 0, "DOUBLE PRESS   ");
		else if (check_single_press(SP_A)) ctrler.print(2, 0, "SINGLE PRESS   ");
		else if (check_falling(FE_X)) ctrler.print(2, 0, "FALLING EDGE    ");


		delay(10);
	}
}
