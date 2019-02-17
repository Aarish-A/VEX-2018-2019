#include "puncher.hpp"

using namespace pros;

PunState pun_state = PunState::Loading;
PunState pun_state_lst  = pun_state;
int pun_state_change_time = 0;
int pun_shots = 0;
bool pun_ball = false;
float last_number = PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD;
float pun_target = 0.0;
float pun_target_two = 0.0;
float pun_target_three = 0.0;

bool auto_set_shot = false;
double auto_angler_target = 0;

void pun_state_change(PunState state) {
		pun_state_lst = pun_state;
		pun_state = state;
		pun_state_change_time = pros::millis();
		log_ln(LOG_PUNCHER, "  >> %d Moved from state %d -> %d. ShotN:%d", pros::millis(), pun_state_lst, pun_state, pun_shots);
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
		log_ln(LOG_PUNCHER, "disableA");
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

	log_ln(LOG_PUNCHER, "%d Pun Cal Before Tare (*PUN_RATIO). LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position()/PUN_RATIO, puncherRight.get_position()/PUN_RATIO);
  puncherLeft.tare_position();
  puncherRight.tare_position();
	log_ln(LOG_PUNCHER, "%d Pun Cal Done Tare. LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());

	log_ln(LOG_PUNCHER, "%d Pun Cal - START LOADING - LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());
  pun_set(127);
	while (puncherLeft.get_position() < PUN_OFFSET + PUN_HOLD) pros::delay(10);
	log_ln(LOG_PUNCHER, "%d Pun Cal - DONE LOADING - LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());
	pun_set(PUN_HOLD_PWR);
  pun_state_change(PunState::Loaded);
}

void pun_handle() {
  static uint32_t ball_on_time = 0;
  static uint32_t wait_slip_end = 0;
	//log_ln(LOG_PUNCHER, "%d LN: %f", pros::millis(), last_number);
	// log_ln(LOG_PUNCHER, "%d Ball Detector is at %d", pros::millis(), ball_sensor.get_value());
	// log_ln(LOG_PUNCHER, "%d The ball is %d", pros::millis(), (int)pun_ball);

  if (ball_sensor.get_value() < PUN_BALL_THRESH) {
			ball_on_time = millis();
			if (!pun_ball) log_ln(LOG_PUNCHER, "	> %d Ball On. Anglr:%f Pos:%f BallSen:%d", pros::millis(), angler.get_position(), puncherLeft.get_position(), ball_sensor.get_value());//, angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = true;
		}

		if (millis() >= ball_on_time + PUN_BALL_OFF_TIME) {
			//printf(" >>> %d ANGLE TARGET CREATE - BALL_OFF \n", pros::millis());
			//AngleTarget* a_targ = PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y});
			if (pun_ball) log_ln(LOG_PUNCHER, "	> %d Ball Off. Anglr:%f Pos:%f BallSen:%d. A:%f", pros::millis(), angler.get_position(), puncherLeft.get_position(), ball_sensor.get_value(), RAD_TO_DEG(getGlobalAngle()));//, RAD_TO_DEG(a_targ->getTarget()-pos.a));//, angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			//if (pun_ball) printf("	> %d Ball Off.Pos:%f BallSen:%d | AnglrTarg:%f Anglr:%f | Pos (%f, %f, %f) \n", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value(), angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = false;
		}

		switch (pun_state) {
			case PunState::Loading:
			{
				double targ = (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
				double cur_err =  targ - puncherLeft.get_position();
				double e_targ = (4.0 * PUN_RATIO);
				log_ln(LOG_PUNCHER, "%d PUNLOADING, CUR: %f, T: %f, E: %f, ETarg:%f", pros::millis(), puncherLeft.get_position(), targ, cur_err, e_targ);
				if (cur_err <= e_targ) {
					pun_set(PUN_HOLD_PWR);
					log_ln(LOG_PUNCHER, "%d PunLoading. PunPos: %f", pros::millis(), puncherLeft.get_position());
					last_number = PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD;
					pun_state_change(PunState::Loaded);
				}
				if (millis() > pun_state_change_time+1000) { //Takes 300 ms
					pun_set(0);
					log_ln(LOG_PUNCHER, " >>> %d PUN FATAL ERROR (from Loading) - T_O | Pos: %f | Cur_Err(%f) needs to be <= to %f", millis(), puncherLeft.get_position(), cur_err, e_targ);
					pun_state_change(PunState::FatalError);
				}
				break;
			}
			case PunState::Loaded:
			{
				if (( shot_req_num > 0 && shot_req[shot_req_handled_num].drive_turn_handled && //For Driver control - shot request must have been made, drive must have been handled & angler must have reached or timed out
						(fabs(angler.get_position()-shot_req[shot_req_handled_num].angle_targ) < 5 || (shot_req[shot_req_handled_num].angler_to && pros::millis() > shot_req[shot_req_handled_num].angler_to) ) )
						|| (auto_set_shot && fabs(angler.get_position()-auto_angler_target) < 5)) //For auto - auto_set_shot flag set to true
					{
						//pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
						uint32_t shot_timeout = pros::millis() + 4000;
						++pun_shots;
						pun_set(127);
						pun_target = PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN;
						pun_target_two = PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num];
						pun_target_three = (PUN_OFFSET + (pun_shots * PUN_TPR)-(15*PUN_RATIO));
						log_ln(LOG_PUNCHER, "%d Shot start (from ShotLoaded first condition) | shot_req_num: %d | drive_turn_handled: %d | anglrOffs: %f | anglerTO: %f ", pros::millis(), shot_req_num, shot_req[shot_req_handled_num].drive_turn_handled, fabs(angler.get_position()-shot_req[shot_req_handled_num].angle_targ), shot_req[shot_req_handled_num].angler_to);

						pun_state_change(PunState::Pull_Back);
					}
				/*else if(auto_set_shot)
				{
					++pun_shots;
					pun_set(127);
					log_ln(LOG_PUNCHER, "%d Shot start (from ShotLoaded second condition))", pros::millis());

					pun_state_change(PunState::Pull_Back);
				}*/
				break;
			}

			case PunState::Pull_Back:
			{
				//log_ln(LOG_PUNCHER, "%d PULLBACK, CUR: %f T: %d", pros::millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN));
				double puncher_position = puncherLeft.get_position();
				log_ln(LOG_PUNCHER, "%d Pullback: Puncher at %f, trying to get to between %f and %f, ball: %d", pros::millis(), puncher_position, pun_target, pun_target_two, (int)pun_ball);
				if (puncher_position < (pun_target) && !pun_ball
				 	&& puncher_position > (pun_target_two)) {
					log_ln(LOG_PUNCHER, "%d Shot failure, no ball pos:%f (b/w:%f & %f). BallSen:%d", millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN), ball_sensor.get_value());
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					ctrler.rumble(" .");
					partner.rumble(" .");
					auto_set_shot = false;
					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;
					log_ln(LOG_PUNCHER, "%d Shot Req: %d Shot ReqH: %d Shot Handled: %d", millis(), shot_req_num, shot_req_handled_num, shot_req[shot_req_handled_num].shot_handled = true);
					pun_state_change(PunState::Loading);
				}
				else if (shot_cancel_pressed && puncherLeft.get_position() < pun_target) {
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					log_ln(LOG_PUNCHER, "%d Shot failure, canceled. Enter PunLoad", millis());

					auto_set_shot = false;
					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;
					shot_cancel_pressed = false;
					pun_state_change(PunState::Loading);

				}
				else if (pun_target_three < puncherLeft.get_position()) { //15 deg before slip position
					wait_slip_end = millis() + PUN_WAIT_TIME;
					pun_set(0);
					pun_move(PUN_OFFSET + (pun_shots * PUN_TPR));

					log_ln(LOG_PUNCHER, "%d Shot end Pos:%f. A:%f", millis(), puncherLeft.get_position(), RAD_TO_DEG(pos.a));
					//printf(" >>> %d ANGLE TARGET CREATE - PUN_PULL_BACK \n", pros::millis());

					auto_set_shot = false;
					pun_state_change(PunState::Bolt_Wait);
				}
				else if(millis() > pun_state_change_time+800)
				{
					pun_set(0);
					log_ln(LOG_PUNCHER, "%d Time exceeded in PullBack State, going into Fatal Error", millis());
					pun_state_change(PunState::FatalError);
				}
				break;
			}
			case PunState::Bolt_Wait:
			{
				//log_ln(LOG_PUNCHER, "%d BOLT WAIT, CUR: %f", pros::millis(), puncherLeft.get_position());
				if (millis() >= wait_slip_end) {
					//pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
					pun_set(127);
					log_ln(LOG_PUNCHER, "%d Done shot wait move to %f T:%f", millis(), (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD), puncherLeft.get_target_position());

					shot_req[shot_req_handled_num].shot_handled = true;

					pun_state_change(PunState::Loading);
				}
				break;
			}

      case PunState::FatalError:
			{
				pun_set(0);
				//log_ln(LOG_PUNCHER,"puncher power was just set to 0 in state: Fatal Error");
				//log_ln(LOG_PUNCHER, "In fatal error puncher power: %d", puncherLeft.get_power());
        break;
			}
		}
}

void pun_fatal_disable() {
  pun_state_change(PunState::FatalError);
  puncherLeft.move_relative(0, 0);
  // puncherRight.move_relative(0, 0);
  puncherLeft.move(0);
  puncherRight.move(0);
  log_ln(LOG_PUNCHER, "%d FATAL Puncher fatal error", millis());
}
