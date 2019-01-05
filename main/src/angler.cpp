#include "angler.hpp"

using namespace pros;

//anglerState angler_state;

void angler_init() {
	angler.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void angler_set(int power) {
  angler.move(power);
}

void angler_move(double position, int32_t velocity) {
  angler.move_absolute(position, velocity);
}

void angler_cal() {
  angler.move(-30);
	delay(100);
	while (angler.get_actual_velocity() < -10) delay(10);
	angler.tare_position();
	angler.move_absolute(17 * 7, 100);
  delay(100);
}

void angler_handle() {
  angler_init();
  angler_cal();

}
