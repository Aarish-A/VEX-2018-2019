#include "shot_select.hpp"

bool shot_pun_go = false;
bool angler_enabled = true;
bool drive_enabled = true;

pros::Task* shot_req_handle_task = nullptr;

/* Shot Positions */
volatile ShotPos front_SP (FieldPos_Front, 98, 0);
volatile ShotPos auto_SP (FieldPos_Front, 88, 0);
volatile ShotPos pf_SP (FieldPos_PF, 90, 70);
volatile ShotPos pf_back_SP (FieldPos_PF_Back_Red, 88, 0);
volatile ShotPos back_SP (FieldPos_Back, 90, 10);
volatile ShotPos skills_front_SP (FieldPos_Front, 112, 41);
volatile ShotPos skills_corner_SP (FieldPos_Back, 130, 61);
volatile ShotPos skills_back_SP (FieldPos_Back, 163, 67, 36);

ShotSelect shot_req[2];
bool angler_pu_flag = false;

int shot_req_num = 0;
int shot_req_handled_num = 0;

bool shot_cancel_pressed = false;
bool shot_task_running = false;

/* Shot Select Code */
void inc_shot_req_num() {
	if (shot_req_num < 2) shot_req_num++;
  else shot_req_num = 2;
}

void dec_shot_req_num() {
  if (shot_req_num > 0) shot_req_num--;
  else shot_req_num = 0;
}

void set_field_pos(FieldPos field_pos) {
  shot_req[0].field_pos = field_pos;
  shot_req[1].field_pos = field_pos;
	log_ln(LOG_SHOTS, "%d Shot Field Pos Set (TaskPtr :%d)| Anglr:%f | RNum:%d | HandledNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f), Hndled1 drive:%d shot:%d| 2angle:%d, 2turn:%d (%f, %f), Hndled1 drive:%d shot:%d ", pros::millis(), shot_req_handle_task, angler.get_position(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[0].drive_turn_handled, shot_req[0].shot_handled, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y, shot_req[1].drive_turn_handled, shot_req[1].shot_handled);
}

void set_angle_targ(bool top) {
  switch (shot_req[shot_req_num-1].field_pos) {
    case FieldPos_Front:
      shot_req[shot_req_num-1].angle_targ = top? front_SP.top : front_SP.mid;
      break;

    case FieldPos_Back:
      shot_req[shot_req_num-1].angle_targ = top? back_SP.top : back_SP.mid;
      break;

    case FieldPos_PF:
      shot_req[shot_req_num-1].angle_targ = top? pf_SP.top : pf_SP.mid;
      break;

    case FieldPos_PF_Back_Red:
      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
      break;
		case FieldPos_PF_Back_Blue:
	      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
	      break;
  }
	log_ln(LOG_SHOTS, "%d Set Angle Targ: %d at i:%d | front_SP t:%d m:%d | back_SP t:%d m:%d | pf_SP t:%d m:%d", pros::millis(), shot_req[shot_req_num-1].angle_targ, shot_req_num-1, front_SP.top, front_SP.mid, back_SP.top, back_SP.mid, pf_back_SP.top, pf_back_SP.mid);
}

void set_angle_targ_right(bool top) {
  switch (shot_req[shot_req_num-1].field_pos) {
    case FieldPos_Front:
      shot_req[shot_req_num-1].angle_targ = top? front_SP.top : front_SP.mid;
      break;

    case FieldPos_Back:
      shot_req[shot_req_num-1].angle_targ = top? back_SP.top : back_SP.mid;
      break;

    case FieldPos_PF:
      shot_req[shot_req_num-1].angle_targ = top? pf_SP.top : pf_SP.mid;
      break;

    case FieldPos_PF_Back_Red:
      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
      break;
		case FieldPos_PF_Back_Blue:
	      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
	      break;
  }
	log_ln(LOG_SHOTS, "%d Set Angle Targ: %d at i:%d | front_SP t:%d m:%d | back_SP t:%d m:%d | pf_SP t:%d m:%d", pros::millis(), shot_req[shot_req_num-1].angle_targ, shot_req_num-1, front_SP.top, front_SP.mid, back_SP.top, back_SP.mid, pf_back_SP.top, pf_back_SP.mid);
}

void set_turn_dir(Dir turn_dir) {
  shot_req[shot_req_num-1].turn_dir = turn_dir;

	if (shot_req[shot_req_num-1].field_pos == FieldPos_PF_Back_Red) { //Shooting from behind the platform (red)
	// if (detecting_signature(1)) {
		// ctrler.print(2, 0, "Red     ");
		shot_req[shot_req_num-1].flag_pos.y = 94;
		if (turn_dir == Dir_Left) {
			// ctrler.print(2, 0, "Red  Left");
			if (game_side == 'R') shot_req[shot_req_num-1].flag_pos.x = -30 - FLAG_WIDTH;
			else if (game_side == 'B') shot_req[shot_req_num-1].flag_pos.x = -33;
		}
		else if (turn_dir == Dir_Right) {
			// ctrler.print(2, 0, "Red  Right");
			if (game_side == 'R') shot_req[shot_req_num-1].flag_pos.x = 20  - FLAG_WIDTH;
			else if (game_side == 'B') shot_req[shot_req_num-1].flag_pos.x = 16;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}
	// else if (detecting_signature(2)) {
		// ctrler.print(2, 0, "Blue   ");
	else if(shot_req[shot_req_num-1].field_pos == FieldPos_PF_Back_Blue) { //Shooting from behind the platform (blue)
		shot_req[shot_req_num-1].flag_pos.y = 94;
		if (turn_dir == Dir_Left) {
			// ctrler.print(2, 0, "Blue  Left");
			if (game_side == 'R') shot_req[shot_req_num-1].flag_pos.x = -26 - FLAG_WIDTH;
			else if (game_side == 'B') shot_req[shot_req_num-1].flag_pos.x = -29;
		}
		else if (turn_dir == Dir_Right) {
			// ctrler.print(2, 0, "Blue  Right");
			if (game_side == 'R') shot_req[shot_req_num-1].flag_pos.x = 23 - FLAG_WIDTH;
			else if (game_side == 'B') shot_req[shot_req_num-1].flag_pos.x = 20;
		}
		else {
			shot_req[shot_req_num-1].flag_pos.x = 0;
		}
	}
	// else if (detecting_signature(3)) {
		// ctrler.print(2, 0, "Yellow   ");
	// else if(shot_req[shot_req_num-1].field_pos == FieldPos_PF_Back_Blue) { //Shooting from behind the platform (blue)
		// shot_req[shot_req_num-1].flag_pos.y = 94;
		// shot_req[shot_req_num-1].flag_pos.x = 0;
	// }
	else { //Shooting from the back
		// ctrler.print(2, 0, "No colour  ");
		shot_req[shot_req_num-1].flag_pos.y = 125;
		if (turn_dir == Dir_Left) {
			shot_req[shot_req_num-1].flag_pos.x = -48;
		}
		else if (turn_dir == Dir_Right) {
			shot_req[shot_req_num-1].flag_pos.x = 48;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}

	if (blue_team) shot_req[shot_req_num-1].flag_pos.x += (5.5 + (FLAG_WIDTH/2));
}

void set_handled_vars() {
	if((shot_req_num-1) >=0 && (shot_req_num-1) <= 1)
	{
		shot_req[shot_req_num-1].drive_turn_handled = false;
		shot_req[shot_req_num-1].shot_handled = false;
		shot_req[shot_req_num-1].angler_to = 0;
	}
	else log_ln(LOG_SHOTS, "	>>>> %d SET_HANDLED_VARS TRIED TO ACCESS OUT OF BOUNDS. SHOT_REQ_NUM = %d", pros::millis(), shot_req_num);
}
void set_handled_vars_all() {
	shot_req[0].drive_turn_handled = false;
	shot_req[0].shot_handled = false;
	shot_req[0].angler_to = 0;
	shot_req[1].drive_turn_handled = false;
	shot_req[1].shot_handled = false;
	shot_req[1].angler_to = 0;
	//log_ln(LOG_SHOTS, "	>>>> %d SET_HANDLED_VARS TRIED TO ACCESS OUT OF BOUNDS. SHOT_REQ_NUM = %d", pros::millis(), shot_req_num);
}

void set_shot_req(bool top, Dir turn_dir) {
	inc_shot_req_num();
	if(turn_dir == Dir_Right) set_angle_targ_right(top);
	else set_angle_targ(top);
	//set_angle_targ(top);
	top ? log_ln(LOG_SHOTS, "TOP PRESSED") : log_ln(LOG_SHOTS, "MID PRESSED");
	set_turn_dir(turn_dir);
	set_handled_vars();
	shot_req[shot_req_num-1].drive_turn_handled = true;
	log_ln(LOG_SHOTS, "%d Shot Req (Task PTR:%d)| Anglr:%f | RNum:%d | HandledNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f), Hndled1 drive:%d shot:%d| 2angle:%d, 2turn:%d (%f, %f), Hndled1 drive:%d shot:%d ", pros::millis(), shot_req_handle_task, angler.get_position(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[0].drive_turn_handled, shot_req[0].shot_handled, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y, shot_req[1].drive_turn_handled, shot_req[1].shot_handled);
}

void shot_req_make() {
	bool RT_M, RM_M;
	bool LT_P, LM_P, RT_P, RM_P;

	RT_M = check_single_press(BTN_SHOT_R_T);
	RM_M = check_single_press(BTN_SHOT_R_M);
	LT_P = check_single_press(BTN_SHOT_L_T, true);
	LM_P = check_single_press(BTN_SHOT_L_M, true);
	RT_P = check_single_press(BTN_SHOT_R_T, true);
	RM_P = check_single_press(BTN_SHOT_R_M, true);

	if (check_single_press(BTN_FIELD_FRONT_P, true) || check_single_press(BTN_FIELD_FRONT_M)) set_field_pos(FieldPos_Front);
	else if (check_single_press(BTN_FIELD_PF_BACK_RED, true)) set_field_pos(FieldPos_PF_Back_Red);
	else if (check_single_press(BTN_FIELD_PF_BACK_BLUE, true)) set_field_pos(FieldPos_PF_Back_Blue);

	if (check_single_press(BTN_SHOOT_CANCEL) || check_single_press(BTN_SHOOT_CANCEL, true)) {
		cancel_shot_cleanup();
		if (shot_task_running) shot_cancel_pressed = true;
		log_ln(LOG_SHOTS, "%d CANCELLED SHOT WITH BUTTON PRESS", pros::millis());
		setDrive(0);
	}

	if (shot_req[0].field_pos != FieldPos_Front) {
		if (LT_P) {
			set_shot_req(true, Dir_Left);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot LT, L_T was %d", pros::millis(), (int)LT_P);
		}
		else if (LM_P) {
			set_shot_req(false, Dir_Left);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot LM, L_M was %d", pros::millis(), (int)LM_P);
		}
		else if (RT_P) {
			set_shot_req(true, Dir_Right);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot RT, R_T was %d", pros::millis(), (int)RT_P);
		}
		else if (RM_P) {
			set_shot_req(false, Dir_Right);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot RM, R_M was %d", pros::millis(), (int)RM_P);
		}
	} else if (shot_req[0].field_pos == FieldPos_Front) {
		if (RT_M) set_shot_req(true, Dir_Centre);
		else if (RM_M) set_shot_req(false, Dir_Centre);
	}

	// This is added in to allow shots to be triggered with any button when there is no partner controller connected...
	// The two ifs are separated only for clarity (when partner controller is connected vs when its not)
	bool shot_button_triggered = false;
	if (partner_connected && (RT_M || RM_M)) shot_button_triggered = true;
	else if (!partner_connected && (RT_M || RM_M || LT_P || RM_P)) shot_button_triggered = true;

	if (shot_button_triggered && (shot_req_num == 1 || shot_req[0].field_pos != FieldPos_Front)) {
		log_ln(LOG_PUNCHER, "%d Attempted to Start task", pros::millis());
		shot_req_handle_start_task();
	}
}

void shot_cleanup() {
	shot_task_running = false;
	shot_pun_go = false;
	setDrive(0);
	drive_enabled = true;
	angler_enabled = true;
	shot_req_num = 0;
	angler.move_absolute(ANGLER_PU_POS, 200);
	intake_state_set(IntakeState::Forw);
}

void cancel_shot_cleanup() {
	shot_req_handle_stop_task();
	shot_cleanup();
}

void shot_req_handle_stop_task() {
	//printf("  >>> %d ---S_STOP 0 check - %d %d \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
	if(shot_req_handle_task != nullptr)
	{
		shot_task_running = false;
		log_ln(LOG_SHOTS, "  >>> %d Stop Shot Req Handle Task", pros::millis());
		if (shot_req_handle_task->get_state() != pros::E_TASK_STATE_DELETED)
			shot_req_handle_task->remove();
		delete shot_req_handle_task;
		shot_req_handle_task = nullptr;
	}
}

void shot_req_handle_start_task() {
	//printf("  >>> %d ---S_Start 1 - %d %d \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
	shot_req_handle_stop_task();
	//printf("  >>> %d S_Start 2 Stop - %d %d \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
	log_ln(LOG_SHOTS, "  >>> %d Start Shot Req Handle Task", pros::millis());
	shot_req_handle_task = new pros::Task((pros::task_fn_t)shot_req_handle);//, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Shot_Req_Handle_Task");
	//printf("  >>> %d S_Start 3 new task - %d %d \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
}


void shot_req_handle(void *param) {
	log_ln(LOG_SHOTS, "%d Start Shot Req Handle Task ",  pros::millis());
	shot_task_running = true;
	//shot_req_num = 0; //DELETE
	if(shot_req_num > 0) {
		ShotSelect tempShot = shot_req[0];
		if (tempShot.field_pos == FieldPos_PF_Back_Red || tempShot.field_pos == FieldPos_PF_Back_Blue) {
			drive_enabled = false;
			angler_enabled = false;
			pos.reset(0,0,0);
			setDrive(0, 10, 0);
			pros::delay(150);
			pos.reset(0, 0, 0);
			log_ln(LOG_SHOTS, "%d S1 Turn to face %f, %f ", pros::millis(), tempShot.flag_pos.x, tempShot.flag_pos.y);
			setDriveVel(0);
			pros::delay(20);
			log_ln(LOG_SHOTS, "%d Done Back up PF (%f, %f, %f) Vel(%f, %f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.velLocal.x, pos.velLocal.y, RAD_TO_DEG(pos.aVel));
			turn_vel_side(PointAngleTarget({tempShot.flag_pos.x, tempShot.flag_pos.y}), (200/60_deg), 0, false);
		}
		angler.move_absolute(tempShot.angle_targ, 200);
		log_ln(LOG_SHOTS, "%d Start Shot 1", pros::millis());
		tempShot.angler_to = pros::millis() + ANGLER_REACH_T0;
		while(fabs(angler.get_position() - tempShot.angle_targ) > 5 && pros::millis() < tempShot.angler_to) pros::delay(5);
		if (pros::millis() > tempShot.angler_to)
		{
			log_ln(LOG_SHOTS, "%d SHOT 1 - ANGLER TO", pros::millis());
			return;
		}
		shot_pun_go = true;
		log_ln(LOG_SHOTS, "%d Started first shot: %d", pros::millis(), shot_req_num);
		while(shot_pun_go) pros::delay(5);
		log_ln(LOG_SHOTS, "%d Finished first shot: %d", pros::millis(), shot_req_num);
		log_ln(LOG_SHOTS, "%d SHOT COMPLETED AT: %f, SUPPOSED TO BE AT %d", pros::millis(), angler.get_position(), tempShot.angle_targ);

		if (shot_req_num > 1) {
			ShotSelect tempShot = shot_req[1];
			if (tempShot.field_pos == FieldPos_PF_Back_Blue || tempShot.field_pos == FieldPos_PF_Back_Red) {
				log_ln(LOG_SHOTS, "%d S1 Turn to face %f, %f ", pros::millis(), tempShot.flag_pos.x, tempShot.flag_pos.y);
				turn_vel(PointAngleTarget({tempShot.flag_pos.x, tempShot.flag_pos.y}), (200/80_deg), 0);
			}
			log_ln(LOG_SHOTS, "%d Start Shot 2", pros::millis());
			angler.move_absolute(tempShot.angle_targ, 200);
			tempShot.angler_to = pros::millis() + ANGLER_REACH_T0;
			while(fabs(angler.get_position() - tempShot.angle_targ) > 5 && pros::millis() < tempShot.angler_to) pros::delay(5);
			if (pros::millis() > tempShot.angler_to)
			{
				log_ln(LOG_SHOTS, "%d SHOT 2 - ANGLER TO", pros::millis());
				return;
			}
			shot_pun_go = true;
			log_ln(LOG_SHOTS, "%d Started second shot: %d", pros::millis(), shot_req_num);
			while(shot_pun_go) pros::delay(5);
			log_ln(LOG_SHOTS, "%d SHOT COMPLETED AT: %f, SUPPOSED TO BE AT %d", pros::millis(), angler.get_position(), tempShot.angle_targ);
			log_ln(LOG_SHOTS, "%d Finished second shot: %d", pros::millis(), shot_req_num);
		}
		shot_req_num = 0;
	}
	log_ln(LOG_PUNCHER, "%d Finished shot task", pros::millis());
	shot_cleanup();
}
