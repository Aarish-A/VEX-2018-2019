#include "puncher.hpp"

using namespace pros;

PunState pun_state;
int pun_shots = 0;
bool pun_ball = false;

bool auto_set_shot = false;

void pun_init() {
	puncher.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void pun_set(int power) {
  puncher.move(power);
}

void pun_move(double position, int32_t velocity) {
  puncher.move_absolute(position, velocity);
}

void pun_cal() {
  uint32_t timeout_time = millis() + 500;
  bool success = true;
  pun_set(-20);
  while (fabs(puncher.get_actual_velocity()) < 15 && (success = (millis() < timeout_time)))
	{
		printf("%d A: %f \n", pros::millis, puncher.get_actual_velocity());
		delay(10);
	}
  if (!success) {
		printf("disableA\n");
    pun_fatal_disable();
    return;
  }
  timeout_time = millis() + 2500; // + 1300;
  while (fabs(puncher.get_actual_velocity()) > 10 && (success = (millis() < timeout_time)))
	{
		printf("%d B: %f \n", pros::millis, puncher.get_actual_velocity());
		delay(10);
	}
  if (!success) {
		printf("disableB\n");
    pun_fatal_disable();
    return;
  }
  puncher.tare_position();

  pun_move(PUN_OFFSET + PUN_HOLD);
  PunState pun_state = PunState::Load;
}

void pun_handle() {
  static uint32_t ball_on_time = 0;
	static uint32_t wait_slip_end = 0;

	while (true)
	{
	  if (ball_sensor.get_value() < PUN_BALL_THRESH) {
				ball_on_time = millis();
				pun_ball = true;
			}

			if (millis() >= ball_on_time + PUN_BALL_OFF_TIME) {
				pun_ball = false;
			}

			switch (pun_state) {
				case PunState::Load:
					if (fabs(puncher.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) <= (4 * PUN_RATIO)) {
						pun_set(5);
						pun_state = PunState::Hold;
					}
					if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B) || auto_set_shot) {
						pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
						printf("%d Shot start\n", millis());
						pun_state = PunState::ShotStart;
					}
					break;

				case PunState::Hold:
					if (fabs(puncher.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) > (6 * PUN_RATIO)) {
						pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
						pun_state = PunState::Load;
					}
					if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B) || auto_set_shot) {
						pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
						printf("%d Shot start\n", millis());
						pun_state = PunState::ShotStart;
					}
					break;

				case PunState::ShotStart:
					auto_set_shot = false;
					if (puncher.get_position() < PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN && puncher.get_position() > PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START && !pun_ball) {
						pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
						printf("%d Shot failure, no ball\n", millis());
						ctrler.rumble(" .");
						pun_state = PunState::Load;
					}
					else if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_A) && puncher.get_position() < PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) {
						pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
						printf("%d Shot failure, canceled\n", millis());
						pun_state = PunState::Load;
					}
					else if (PUN_OFFSET + (pun_shots * PUN_TPR) - puncher.get_position() < 1) {
						wait_slip_end = millis() + PUN_WAIT_TIME;
						printf("%d Shot end\n", millis());
						pun_state = PunState::ShotWait;
					}
					break;

				case PunState::ShotWait:
					if (millis() >= wait_slip_end) {
						pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
						pun_state = PunState::Load;
					}
					break;

	      case PunState::FatalError:

	        break;
			}
			pros::delay(10);
		}
}

void pun_fatal_disable() {
  pun_state = PunState::FatalError;
  puncher.move_relative(0, 0);
  puncher.move(0);
  printf("%d FATAL Puncher fatal error\n", millis());
}
