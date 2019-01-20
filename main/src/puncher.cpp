#include "puncher.hpp"

using namespace pros;

PunState pun_state = PunState::Loading;
PunState pun_state_lst  = pun_state;
int pun_state_change_time = 0;
int pun_shots = 0;
bool pun_ball = false;

bool auto_set_shot = false;

void pun_state_change(PunState state) {
	log_ln(LOG_PUNCHER, "  >> %d %d -> %d\n", pros::millis(), pun_state_lst, pun_state);
	pun_state_lst = pun_state;
	pun_state = state;
	pun_state_change_time = pros::millis();
}

void pun_init() {
	puncherLeft.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void pun_set(int power) {
  puncherLeft.move(power);
}

void pun_move(double position, int32_t velocity) {
  puncherLeft.move_absolute(position, velocity);
}

void pun_cal() {
  uint32_t timeout_time = millis() + 500;
  bool success = true;
  pun_set(-20);
  while (fabs(puncherLeft.get_actual_velocity()) < 12 && (success = (millis() < timeout_time)))
	{
		log_ln(LOG_PUNCHER, "%d A: %f T:%f", pros::millis(), puncherLeft.get_actual_velocity(), puncherLeft.get_temperature());
		delay(10);
	}
  if (!success) {
		log_ln(LOG_PUNCHER, "disableA \n");
    pun_fatal_disable();
    return;
  }
  timeout_time = millis() + 2500; // + 1300;
  while (fabs(puncherLeft.get_actual_velocity()) > 10 && (success = (millis() < timeout_time)))
	{
		log_ln(LOG_PUNCHER, "%d B: %f T:%f", pros::millis(), puncherLeft.get_actual_velocity(), puncherLeft.get_temperature());
		delay(10);
	}
  if (!success) {
		log_ln(LOG_PUNCHER, "disableB");
    pun_fatal_disable();
    return;
  }
  delay(100);

	log_ln(LOG_PUNCHER, "%d Pun Cal Before Tare. LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());
  puncherLeft.tare_position();
  puncherRight.tare_position();
	log_ln(LOG_PUNCHER, "%d Pun Cal Done Tare. LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());

  pun_move(PUN_OFFSET + PUN_HOLD);
  pun_state_change(PunState::Loading);
}

void pun_handle() {
  static uint32_t ball_on_time = 0;
  static uint32_t wait_slip_end = 0;

  if (ball_sensor.get_value() < PUN_BALL_THRESH) {
			ball_on_time = millis();
			if (!pun_ball) log_ln(LOG_PUNCHER, "	> %d Ball On. Pos:%f BallSen:%d", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value());//, angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = true;
		}

		if (millis() >= ball_on_time + PUN_BALL_OFF_TIME) {
			if (pun_ball) log_ln(LOG_PUNCHER, "	> %d Ball Off. Pos:%f BallSen:%d", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value());//, angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			//if (pun_ball) printf("	> %d Ball Off.Pos:%f BallSen:%d | AnglrTarg:%f Anglr:%f | Pos (%f, %f, %f) \n", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value(), angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = false;
		}

		switch (pun_state) {
			case PunState::Loading:
				if (fabs(puncherLeft.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) <= (4 * PUN_RATIO)) {
					pun_set(PUN_HOLD_PWR);
					log_ln(LOG_PUNCHER, "%d PunLoading. PunPos: %f", pros::millis(), puncherLeft.get_position());
					pun_state_change(PunState::Loaded);
				}
				break;

			case PunState::Loaded:
				//printf(" >> %d PunLoaded %f ", millis(), puncherLeft.get_position());
				//if (( shot_req_num > 0 && shot_req[shot_req_handled_num].drive_turn_handled && fabs(angler.get_position()-shot_req[shot_req_handled_num].angle_targ) < 5) ||auto_set_shot) {
				if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_A)) {
					//pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
					++pun_shots;
					pun_set(127);
					log_ln(LOG_PUNCHER, "%d Shot start (from ShotLoaded)", pros::millis());//"| Shtr Pos:%f t:%f | ReqNum:%d ShtNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f,%f) | 2angle:%d, 2turn:%d (%f,%f) \n", pros::millis(), puncherLeft.get_position(), puncherLeft.get_target_position(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);

					pun_state_change(PunState::Pull_Back);
				}
				break;

			case PunState::Pull_Back:
				if (puncherLeft.get_position() < (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) && !pun_ball
				 	&& puncherLeft.get_position() > (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]) ) {
					log_ln(LOG_PUNCHER, "%d Shot failure, no ball pos:%f (b/w:%f & %f). BallSen:%d", millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN), ball_sensor.get_value());
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					ctrler.rumble(" .");
					/*
					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;
					pun_state_change(PunState::Load);
					*/
				}
				else if (/*shot_cancel_pressed*/ ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B) && puncherLeft.get_position() < PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) {
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					log_ln(LOG_PUNCHER, "%d Shot failure, canceled. Enter PunLoad", millis());
					/*
					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;
					shot_cancel_pressed = false;
					pun_state_change(PunState::Load);
					*/
				}
				else if ((PUN_OFFSET + (pun_shots * PUN_TPR)-(15*PUN_RATIO)) < puncherLeft.get_position()) { //10 ticks before slip position
					wait_slip_end = millis() + PUN_WAIT_TIME;
					pun_set(0);
					pun_move(PUN_OFFSET + (pun_shots * PUN_TPR));

					log_ln(LOG_PUNCHER, "%d Shot end Pos:%f", millis(), puncherLeft.get_position());

					pun_state_change(PunState::Bolt_Wait);
				}
				break;

			case PunState::Bolt_Wait:
				if (millis() >= wait_slip_end) {
					auto_set_shot = false;
					//pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
					pun_set(127);
					log_ln(LOG_PUNCHER, "%d Done shot wait move to %f T:%f", millis(), (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD), puncherLeft.get_target_position());
					/*
					shot_req[shot_req_handled_num].shot_handled = true;
					*/
					pun_state_change(PunState::Loading);
				}
				break;

      case PunState::FatalError:

        break;
		}
}

void pun_fatal_disable() {
  pun_state_change(PunState::FatalError);
  puncherLeft.move_relative(0, 0);
  puncherRight.move_relative(0, 0);
  puncherLeft.move(0);
  puncherRight.move(0);
  log_ln(LOG_PUNCHER, "%d FATAL Puncher fatal error", millis());
}
