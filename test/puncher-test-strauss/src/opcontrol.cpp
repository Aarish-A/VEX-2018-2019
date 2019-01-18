#include "main.h"
#include <iostream>

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

using namespace pros;

enum class PunState { Load, Hold, ShotStart, ShotWait };

const double PUN_RATIO = (60.0 / 36.0);
const double PUN_OFFSET = 45 * PUN_RATIO;
const double PUN_HOLD = 180 * PUN_RATIO; // Relative to the slip point
const double PUN_TPR = 360 * PUN_RATIO; // Relative to the slip point
const double PUN_NO_RETURN = 50 * PUN_RATIO; // Back from the slip point
const double PUN_BALL_CHK_START = PUN_TPR - (PUN_HOLD + 5); // Back from the slip point
const uint32_t PUN_WAIT_TIME = 100;
const int PUN_BALL_THRESH = 2200;
const uint32_t PUN_BALL_OFF_TIME = 50;

Motor punMtr1(2, E_MOTOR_GEARSET_18);

void setPuncher(int power) {
	punMtr1.move(power);
	//punMtr2.move(power);
}

void movePuncher(double position, int32_t velocity = 200) {
	punMtr1.move_absolute(position, velocity);
	//punMtr2.move_absolute(position, velocity);
}

void logPuncher(void *param) {
	FILE* logFile = fopen("/usd/log.txt", "a");
	if (logFile == NULL) {
		printf("Could not open log file\n");
		return;
	}
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", logFile);

	int32_t voltageLst = 0;
	int32_t currentLst = 0;
	double positionLst = 0;

	uint32_t nextFlush = 0;
	bool written = false;

	while (true) {
		int32_t voltage = punMtr1.get_voltage();
		int32_t current = punMtr1.get_current_draw();
		double position = punMtr1.get_position();

		if (round(voltage / 100.0) != round(voltageLst / 100.0) || abs(current - currentLst) > 1 || round(position * 10) != round(positionLst * 10)) {
			fprintf(logFile, "%d %.1f %d %.1f\r\n", millis(), voltage / 1000.0, current, position);
			printf("%d %.1f %d %.1f\n", millis(), voltage / 1000.0, current, position);
			written = true;
			voltageLst = voltage;
			currentLst = current;
			positionLst = position;
		}

		if (written && millis() >= nextFlush) {
			fclose(logFile);
			while ((logFile = fopen("/usd/log.txt", "a")) == NULL) delay(5);
			written = false;
			nextFlush = millis() + 1000;
		}

		delay(20);
	}
}
#include "puncher.hpp"

using namespace pros;

PunState pun_state = PunState::Load;
PunState pun_state_lst  = pun_state;
int pun_state_change_time = 0;
int pun_shots = 0;
bool pun_ball = false;

bool auto_set_shot = false;

void pun_state_change(PunState state) {
	pun_state_lst = pun_state;
	pun_state = state;
	pun_state_change_time = pros::millis();
}

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
		log_ln("%d A: %f ", pros::millis(), puncherLeft.get_actual_velocity());
		delay(10);
	}
  if (!success) {
		log_ln("disableA");
    pun_fatal_disable();
    return;
  }
  timeout_time = millis() + 2500; // + 1300;
  while (fabs(puncherLeft.get_actual_velocity()) > 10 && (success = (millis() < timeout_time)))
	{
		log_ln("%d B: %f ", pros::millis(), puncherLeft.get_actual_velocity());
		delay(10);
	}
  if (!success) {
		log_ln("disableB");
    pun_fatal_disable();
    return;
  }
  delay(100);

	log_ln("%d Pun Cal Before Tare. LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());
  puncherLeft.tare_position();
  puncherRight.tare_position();
	log_ln("%d Pun Cal Done Tare. LeftPos:%f, RightPos:%f", pros::millis(), puncherLeft.get_position(), puncherRight.get_position());

  pun_move(PUN_OFFSET + PUN_HOLD);
  PunState pun_state_change(PunState::Load);
}

void opcontrol() {
  static uint32_t ball_on_time = 0;
  static uint32_t wait_slip_end = 0;

  if (ball_sensor.get_value() < PUN_BALL_THRESH) {
			ball_on_time = millis();
			if (!pun_ball) log_ln("	> %d Ball On. Pos:%f BallSen:%d | AnglrTarg:%f Anglr:%f | Pos (%f, %f, %f)", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value(), angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = true;
		}

		if (millis() >= ball_on_time + PUN_BALL_OFF_TIME) {
			if (pun_ball) log_ln("	> %d Ball Off.Pos:%f BallSen:%d | AnglrTarg:%f Anglr:%f | Pos (%f, %f, %f)", pros::millis(), puncherLeft.get_position(), ball_sensor.get_value(), angler.get_target_position(), angler.get_position(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			pun_ball = false;
		}

		switch (pun_state) {
			case PunState::Load:
				//log_ln(" >> %d PunLoad %f ", millis(), puncherLeft.get_position());
				if (fabs(puncherLeft.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) <= (4 * PUN_RATIO)) {
					pun_set(PUN_HOLD_PWR);
					log_ln("%d PunLoad -> PunHold. PunPos: %f ", pros::millis(), puncherLeft.get_position());
					pun_state_change(PunState::Hold);
				}
				break;

			case PunState::Hold:
				//log_ln(" >> %d PunHold %f ", millis(), puncherLeft.get_position());
				if (fabs(puncherLeft.get_position() - (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD)) > (6 * PUN_RATIO)) {
					pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
					log_ln("%d PunLoad -> PunHold. PunPos: %f ", pros::millis(), puncherLeft.get_position());
					pun_state_change(PunState::Load);
				}
				if (( shot_req_num > 0 && shot_req[shot_req_handled_num].drive_turn_handled && fabs(angler.get_position()-shot_req[shot_req_handled_num].angle_targ) < 5) ||auto_set_shot) {
					pun_move(PUN_OFFSET + (++pun_shots * PUN_TPR));
					log_ln("%d Shot start (from ShotHold) | Shtr Pos:%f t:%f | ReqNum:%d ShtNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f,%f) | 2angle:%d, 2turn:%d (%f,%f)", pros::millis(), puncherLeft.get_position(), puncherLeft.get_target_position(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y);

					pun_state_change(PunState::ShotStart);
				}
				break;

			case PunState::ShotStart:
				if (puncherLeft.get_position() < (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) && !pun_ball
				 	&& puncherLeft.get_position() > (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]) ) {
					log_ln("%d Shot failure, no ball pos:%f (b/w:%f & %f). BallSen:%d", millis(), puncherLeft.get_position(), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_BALL_CHK_START[shot_req_handled_num]), (PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN), ball_sensor.get_value());
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					ctrler.rumble(" .");

					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;

					pun_state_change(PunState::Load);
				}
				else if (shot_cancel_pressed && puncherLeft.get_position() < PUN_OFFSET + (pun_shots * PUN_TPR) - PUN_NO_RETURN) {
					pun_move(PUN_OFFSET + (--pun_shots * PUN_TPR) + PUN_HOLD);
					log_ln("%d Shot failure, canceled. Enter PunLoad", millis());

					shot_req[shot_req_handled_num].shot_handled = true;
					shot_req_handled_num = 0;

					shot_cancel_pressed = false;

					pun_state_change(PunState::Load);
				}
				else if (PUN_OFFSET + (pun_shots * PUN_TPR) - puncherLeft.get_position() < 1) {
					wait_slip_end = millis() + PUN_WAIT_TIME;
					log_ln("%d Shot end", millis());

					pun_state_change(PunState::ShotWait);
				}
				break;

			case PunState::ShotWait:
				if (millis() >= wait_slip_end) {
					auto_set_shot = false;
					pun_move(PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD);
					log_ln("%d Done shot wait move to %f T:%f ", millis(), (PUN_OFFSET + (pun_shots * PUN_TPR) + PUN_HOLD), puncherLeft.get_target_position());
					shot_req[shot_req_handled_num].shot_handled = true;

					pun_state_change(PunState::Load);
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
  log_ln("%d FATAL Puncher fatal error", millis());
}
