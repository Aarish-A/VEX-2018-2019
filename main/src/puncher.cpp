#include "puncher.hpp"

using namespace pros;

PunState pun_state = PunState::Loading;
PunState pun_state_lst  = pun_state;
int pun_state_set_time = 0;
int pun_shots = 0;
bool pun_ball = false;
float last_number = PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD;
float pun_target = 0.0;
float pun_target_two = 0.0;
float pun_target_three = 0.0;

bool auto_set_shot = false;
double auto_angler_target = 0;

void pun_move_state_set(double position, PunState state) {
	if (pun_state == PunState::FatalError) {
		log_ln(LOG_PUNCHER, "  >> %d Move from fatal -> %d ignored", pros::millis(), state);
	}
	else {
		pun_move(position);
		pun_state_lst = pun_state;
		pun_state = state;
		pun_state_set_time = pros::millis();
		log_ln(LOG_PUNCHER, "  >> %d Moved from state %d -> %d | ShotN:%d | Vel:%f", pros::millis(), pun_state_lst, pun_state, pun_shots, puncherLeft.get_actual_velocity());
	}
}

void pun_power_state_set(int power, PunState state) {
	if (pun_state == PunState::FatalError) {
		log_ln(LOG_PUNCHER, "  >> %d Move from fatal -> %d ignored", pros::millis(), state);
	}
	else {
		pun_set(power);
		pun_state_lst = pun_state;
		pun_state = state;
		pun_state_set_time = pros::millis();
		log_ln(LOG_PUNCHER, "  >> %d Moved from state %d -> %d | ShotN:%d | Vel:%f", pros::millis(), pun_state_lst, pun_state, pun_shots, puncherLeft.get_actual_velocity());
	}
}

void pun_state_set(PunState state) {
  log_ln(LOG_INTAKE, "%d Changing intake state to %d", millis(), intake_state);
  switch(state)
  {
    case PunState::CalA: pun_power_state_set(-20, state); break;
    case PunState::CalB: pun_power_state_set(-20, state); break;
    case PunState::CalWait: pun_power_state_set(-20, state); break;
    case PunState::Loading: pun_power_state_set(127, state); break;
		case PunState::Loaded: pun_power_state_set(PUN_HOLD_PWR, state); break;
		case PunState::Cancel: pun_move_state_set(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD, state); break;
		case PunState::Pull_Back: pun_power_state_set(127, state); break;
		case PunState::Bolt_Wait: pun_move_state_set(PUN_OFFSET + (pun_shots * PUN_TPR), state); break;
		case PunState::FatalError: pun_power_state_set(0, state); break;
  }
}

void pun_init() {
	puncherLeft.set_brake_mode(E_MOTOR_BRAKE_HOLD);
}

void pun_set(int power) {
  puncherLeft.move(power);
}

void pun_move(double position, int32_t velocity) {
  puncherLeft.move_absolute(position, velocity);
	log_ln(LOG_PUNCHER, "%d T:%f V:%d | P:f", millis(), position, velocity, puncherLeft.get_position());
}

void pun_handle() {
  static uint32_t ball_on_time = 0;
  static uint32_t wait_slip_end = 0;

  if (ball_sensor.get_value() < PUN_BALL_THRESH) {
			ball_on_time = millis();
			if (!pun_ball) log_ln(LOG_PUNCHER, "	> %d Ball On. Anglr:%f Pos:%f BallSen:%d", pros::millis(), angler.get_position(), puncherLeft.get_position(), ball_sensor.get_value());//, angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = true;
		}

		if (millis() >= ball_on_time + PUN_BALL_OFF_TIME) {
			if (pun_ball) log_ln(LOG_PUNCHER, "	> %d Ball Off. Anglr:%f Pos:%f BallSen:%d. A:%f", pros::millis(), angler.get_position(), puncherLeft.get_position(), ball_sensor.get_value(), RAD_TO_DEG(getGlobalAngle()));//, RAD_TO_DEG(a_targ->getTarget()-pos.a));//, angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = false;
		}

		switch (pun_state) {
			case PunState::CalA:
			{
				if (fabs(puncherLeft.get_actual_velocity()) > 12) {
					pun_state_set(PunState::CalB);
				}
				else if (millis() >= pun_state_set_time + 500) {
					log_ln(LOG_PUNCHER, "disableA - TO - don't kill pun_cal");
					pun_state_set(PunState::CalB);
				}
				break;
			}
			case PunState::CalB:
			{
				if (fabs(puncherLeft.get_actual_velocity()) < 10) {
					pun_state_set(PunState::CalWait);
				}
				else if (millis() >= pun_state_set_time + 1500)	{
					log_ln(LOG_PUNCHER, "disableB");
			    pun_fatal_disable();
				}
				break;
			}
			case PunState::CalWait:
			{
				if (millis() >= pun_state_set_time + 100) {
					log_ln(LOG_PUNCHER, "%d Pun Cal Before Tare (*PUN_RATIO). LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position()/PUN_RATIO, puncherRight.get_position()/PUN_RATIO);
				  puncherLeft.tare_position();
				  puncherRight.tare_position();
					log_ln(LOG_PUNCHER, "%d Pun Cal Done Tare. LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());

					pun_state_set(PunState::Loading);
				}
				break;
			}
			case PunState::Loading:
			{
				double targ = (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
				double cur_err =  targ - puncherLeft.get_position();
				double e_targ = (4.0 * PUN_RATIO);
				if (cur_err <= e_targ) {
					log_ln(LOG_PUNCHER, "%d PunLoading. PunPos: %f", pros::millis(), puncherLeft.get_position());
					last_number = PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD;
					pun_state_set(PunState::Loaded);
				}
				if (millis() > pun_state_set_time+1000) { //Takes 300 ms
					log_ln(LOG_PUNCHER, " >>> %d PUN FATAL ERROR (from Loading) - T_O | Pos: %f | Cur_Err(%f) needs to be <= to %f", millis(), puncherLeft.get_position(), cur_err, e_targ);
					cancel_shot_cleanup();
					pun_state_set(PunState::FatalError);
				}
				break;
			}
			case PunState::Loaded:
			{
				if (shot_pun_go || (auto_set_shot && fabs(angler.get_position()-auto_angler_target) < 5)) //For auto - auto_set_shot flag set to true
					{
						uint32_t shot_timeout = pros::millis() + 4000;
						++pun_shots;
						pun_target = PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN;
						pun_target_two = PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[0];
						pun_target_three = (PUN_OFFSET + (pun_shots * PUN_TPR)-(15*PUN_RATIO));
						printf("%d Shot start (from ShotLoaded first condition) | shot_pun_go = %d | pos : %f | shot_req_num: %d | shot_req_handled_num: %d | drive_turn_handled: %d | anglrOffs: %f | anglerTO: %d ", pros::millis(), shot_pun_go, puncherLeft.get_position(), shot_req_num, shot_req_handled_num, shot_req[shot_req_handled_num].drive_turn_handled, fabs(angler.get_position()-shot_req[shot_req_handled_num].angle_targ), shot_req[shot_req_handled_num].angler_to);

						pun_state_set(PunState::Pull_Back);
					}
				break;
			}
			case PunState::Cancel:
			{
				double targ = (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
				double cur_err =  targ - puncherLeft.get_position();
				double e_targ = (4.0 * PUN_RATIO);
				if(cur_err >= -e_targ)
				{
					log_ln(LOG_PUNCHER, "%d Made it to prev position: %f", pros::millis(), puncherLeft.get_position());
					last_number = PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD;
					shot_req_num = 0;
					angler_move(ANGLER_PU_POS);
					pun_state_set(PunState::Loaded);
				}
				if (millis() > pun_state_set_time + 3000) {
					log_ln(LOG_PUNCHER, " >>> %d PUN FATAL ERROR (from Loading) - T_O | Pos: %f | Cur_Err(%f) needs to be <= to %f", millis(), puncherLeft.get_position(), cur_err, e_targ);
					cancel_shot_cleanup();
					pun_state_set(PunState::FatalError);
				}
				break;
			}
			case PunState::Pull_Back:
			{
				//log_ln(LOG_PUNCHER, "%d PULLBACK, CUR: %f T: %d", pros::millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN));
				double puncher_position = puncherLeft.get_position();
				if (puncher_position < (pun_target) && !pun_ball
				 	&& puncher_position > (pun_target_two)) {
					log_ln(LOG_PUNCHER, "%d Shot failure, no ball pos:%f (b/w:%f & %f). BallSen:%d", millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN), ball_sensor.get_value());
					ctrler.rumble(" .");
					partner.rumble(" .");
					cancel_shot_cleanup();

					auto_set_shot = false;
					log_ln(LOG_PUNCHER, "%d Shot Req: %d Shot ReqH: %d Shot Handled: %d", millis(), shot_req_num, shot_req_handled_num, shot_req[shot_req_handled_num].shot_handled = true);
					pun_state_set(PunState::Cancel);
				}
				else if (shot_cancel_pressed && puncherLeft.get_position() < pun_target) {
					log_ln(LOG_PUNCHER, "%d Shot failure, canceled. Enter PunLoad", millis());
					cancel_shot_cleanup();
					auto_set_shot = false;
					shot_cancel_pressed = false;
					shot_req_num = 0;
					log_ln(LOG_PUNCHER, "%d shot_handled0: %d shot_handled1: %d", millis(), shot_req[0].shot_handled, shot_req[1].shot_handled);
					pun_state_set(PunState::Cancel);

				}
				else if (pun_target_three < puncherLeft.get_position()) { //15 deg before slip position
					wait_slip_end = millis() + PUN_WAIT_TIME;

					log_ln(LOG_PUNCHER, "%d Shot end Pos:%f. Angler At: %f A:%f", millis(), puncherLeft.get_position(), angler.get_position(), RAD_TO_DEG(pos.a));

					pun_state_set(PunState::Bolt_Wait);
				}
				else if(millis() > pun_state_set_time+800)
				{
					log_ln(LOG_PUNCHER, "%d Time exceeded in PullBack State, going into Fatal Error", millis());
					cancel_shot_cleanup();
					pun_state_set(PunState::FatalError);
				}
				break;
			}
			case PunState::Bolt_Wait:
			{
				//log_ln(LOG_PUNCHER, "%d BOLT WAIT, CUR: %f", pros::millis(), puncherLeft.get_position());
				if (millis() >= wait_slip_end) {
					log_ln(LOG_PUNCHER, "%d Done shot wait move to %f T:%f, Angler at %f", millis(), (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD), puncherLeft.get_target_position(), angler.get_position());

					shot_pun_go = false;
					auto_set_shot = false;
					pun_state_set(PunState::Loading);
				}
				break;
			}

      case PunState::FatalError:
			{
				pun_set(0);
				ctrler.print(2, 0, "FATAL ERROR");
        break;
			}
		}
}

void pun_fatal_disable() {
  pun_state_set(PunState::FatalError);
  puncherLeft.move_relative(0, 0);
  // puncherRight.move_relative(0, 0);
  puncherLeft.move(0);
  puncherRight.move(0);
  log_ln(LOG_PUNCHER, "%d FATAL Puncher fatal error", millis());
}
