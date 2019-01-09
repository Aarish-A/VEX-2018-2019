#include "puncher.hpp"

using namespace pros;

PunState pun_state;
int pun_shots = 0;
bool pun_ball = false;

bool auto_set_shot = false;

void pun_init() {
	puncherLeft.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	puncherRight.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void pun_set(int power) {
  puncherLeft.move(power);
  puncherRight.move(power);
}

void pun_move(double position, int32_t velocity) {
  puncherLeft.move_absolute(position, velocity);
  puncherRight.move_absolute(position, velocity);
}

void pun_cal() {
  uint32_t timeout_time = millis() + 500;
  bool success = true;
  pun_set(-20);
  while (fabs(puncherLeft.get_actual_velocity()) < 12 && (success = (millis() < timeout_time)))
	{
		printf("%d A: %f \n", pros::millis(), puncherLeft.get_actual_velocity());
		delay(10);
	}
  if (!success) {
		printf("disableA\n");
    pun_fatal_disable();
    return;
  }
  timeout_time = millis() + 2500; // + 1300;
  while (fabs(puncherLeft.get_actual_velocity()) > 10 && (success = (millis() < timeout_time)))
	{
		printf("%d B: %f \n", pros::millis(), puncherLeft.get_actual_velocity());
		delay(10);
	}
  if (!success) {
		printf("disableB\n");
    pun_fatal_disable();
    return;
  }
  delay(100);
  puncherLeft.tare_position();
  puncherRight.tare_position();

  pun_move(PUN_OFFSET + PUN_HOLD);
  PunState pun_state = PunState::Load;
}

void pun_handle() {
  static uint32_t ball_on_time = 0;
  static uint32_t wait_slip_end = 0;

  if (ball_sensor.get_value() < PUN_BALL_THRESH) {
			ball_on_time = millis();
			if (!pun_ball) printf("	> %d Ball On. Pos:%f BallSen:%d | AnglrTarg:%f Anglr:%f | Pos (%f, %f, %f) \n", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value(), angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = true;
		}

		if (millis() >= ball_on_time + PUN_BALL_OFF_TIME) {
			if (pun_ball) printf("	> %d Ball Off.Pos:%f BallSen:%d | AnglrTarg:%f Anglr:%f | Pos (%f, %f, %f) \n", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value(), angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = false;
		}

		switch (pun_state) {
			case PunState::Load:
				if (fabs(puncherLeft.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) <= (4 * PUN_RATIO)) {
					pun_set(PUN_HOLD_PWR);
					pun_state = PunState::Hold;
				}
				if (( shot_req_num > 0 && shot_req[shot_req_handled_num].drive_turn_handled )|| auto_set_shot) {
					pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
					printf("%d Shot start\n", millis());
					printf("%d ShotLoad | ReqNum:%d ShtNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f,%f) | 2angle:%d, 2turn:%d (%f,%f)\n", pros::millis(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
					pun_state = PunState::ShotStart;
				}
				break;

			case PunState::Hold:
				if (fabs(puncherLeft.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) > (6 * PUN_RATIO)) {
					pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
					pun_state = PunState::Load;
				}
				if (( shot_req_num > 0 && shot_req[shot_req_handled_num].drive_turn_handled )| auto_set_shot) {
					pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
					printf("%d Shot start\n", millis());
					printf("%d ShotHold | ReqNum:%d ShtNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f,%f) | 2angle:%d, 2turn:%d (%f,%f)\n", pros::millis(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);
					pun_state = PunState::ShotStart;
				}
				break;

			case PunState::ShotStart:
				auto_set_shot = false;
				if (puncherLeft.get_position() < (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) && !pun_ball
				 	&& puncherLeft.get_position() > (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]) ) {
					printf("%d Shot failure, no ball pos:%f (b/w:%f & %f). BallSen:%d\n", millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN), ball_sensor.get_value());
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					ctrler.rumble(" .");

					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;

					pun_state = PunState::Load;
				}
				else if (shot_cancel_pressed && puncherLeft.get_position() < PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) {
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					printf("%d Shot failure, canceled\n", millis());

					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;

					shot_cancel_pressed = false;

					pun_state = PunState::Load;
				}
				else if (PUN_OFFSET + (pun_shots * PUN_TPR) - puncherLeft.get_position() < 1) {
					wait_slip_end = millis() + PUN_WAIT_TIME;
					printf("%d Shot end\n", millis());

					shot_req[shot_req_handled_num].shot_handled = true;

					pun_state = PunState::ShotWait;
				}
				break;

			case PunState::ShotWait:
				if (millis() >= wait_slip_end) {
					pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
					printf("%d Done shot wait\n", millis());
					pun_state = PunState::Load;
				}
				break;

      case PunState::FatalError:

        break;
		}
		pros::delay(10);
}

void pun_fatal_disable() {
  pun_state = PunState::FatalError;
  puncherLeft.move_relative(0, 0);
  puncherRight.move_relative(0, 0);
  puncherLeft.move(0);
  puncherRight.move(0);
  printf("%d FATAL Puncher fatal error\n", millis());
}
