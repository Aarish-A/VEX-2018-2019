#include "main.h"
#include "config.hpp"
#include "math.h"

double WIDTH = 13.3;
double HEIGHT = 10.7;
double DIAMETER = sqrt(WIDTH * WIDTH + HEIGHT * HEIGHT);
double CIRCUMFRENCE = M_PI * DIAMETER;
double WHEEL_CIRCUMFRENCE = M_PI * 4.0;
double GEAR_RATIO = 60.0 / 84.0;
double WHL_DIA_L = 2.783;


void opcontrol() {
	drive_fr.tare_position();
	drive_fl.tare_position();
	drive_br.tare_position();
	drive_bl.tare_position();
	float target =  CIRCUMFRENCE / WHEEL_CIRCUMFRENCE / sqrt(2)/2 * 360 * GEAR_RATIO;
	int timer = pros::millis();
	printf("%f\n",target);
	drive_fr.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_fl.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_br.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_bl.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_fr.move_absolute(target, 10);
	drive_fl.move_absolute(-target, 10);
	drive_br.move_absolute(target, 10);
	drive_bl.move_absolute(-target, 10);
	pros::delay(80000);

	printf("%f\n",drive_fr.get_position());
	printf("%f\n",drive_fl.get_position());

}
