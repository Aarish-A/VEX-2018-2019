#include "main.h"
using namespace pros;

void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor decapper(6);
	pros::Motor drive_fl(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor drive_bl(4, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor drive_fr(10, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor drive_br(8, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

	int decapper_counter = 0;
	int x = 0;
	int y = 0;
	int a = 0;

	while (true) {

		switch(decapper_counter) {
			case 0:
				decapper.move_absolute(0, 125);
				break;
			case 1:
				decapper.move_absolute(2300, 125);
				break;
			case 2:
				decapper.move_absolute(2628, 125);
				break;
			case 3:
				decapper.move_absolute(2950, 125);
				break;
		}

		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP) && (decapper_counter < 4)) {
			decapper_counter++;
		}
		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN) && (decapper_counter > 0)) {
			decapper_counter--;
		}

		printf("decapper: %d\n", decapper_counter);

		if (abs(master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y)) > 5) y = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		else y = 0;

		if (abs(master.get_analog(E_CONTROLLER_ANALOG_LEFT_X)) > 5) a = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		else a = 0;

		if (abs(master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X)) > 5) x = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		else x = 0;

		drive_fl.move(y + x + a);
	 	drive_bl.move(y - x + a);
	 	drive_fr.move(y - x - a);
	 	drive_br.move(y + x - a);

		printf("%d\n", (int)decapper.get_position());

	}
}
