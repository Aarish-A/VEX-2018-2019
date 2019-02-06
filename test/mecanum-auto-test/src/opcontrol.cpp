#include "main.h"
#include "config.hpp"
#include "math.h"

double WIDTH = 13.3;
double HEIGHT = 10.7;
double DIAMETER = 7.4653975 * 2;
double CIRCUMFRENCE = M_PI * DIAMETER;
double WHEEL_CIRCUMFRENCE = M_PI * 2.783;
double GEAR_RATIO = 1;
double WHL_DIA_L = 2.783;


void opcontrol() {
	drive_fr.tare_position();
	drive_fl.tare_position();
	drive_br.tare_position();
	drive_bl.tare_position();
	enc_l.reset();
	float target =  CIRCUMFRENCE / WHEEL_CIRCUMFRENCE  * 360 * GEAR_RATIO;
	int timer = pros::millis();
	printf("%f\n",target);
	drive_fr.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_fl.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_br.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_bl.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	drive_fr.move(50);
	drive_fl.move(-50);
	drive_br.move(50);
	drive_bl.move(-50);
	while(fabs(enc_l.get_value())<target) {pros::delay(10);}
	drive_fr.move(0);
	drive_fl.move(0);
	drive_br.move(0);
	drive_bl.move(0);
	printf("%f\n",drive_fr.get_position());
	printf("%f\n",drive_fl.get_position());

}
