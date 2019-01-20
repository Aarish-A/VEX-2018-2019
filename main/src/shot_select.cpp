#include "shot_select.hpp"

pros::Task* shot_req_handle_task = nullptr;

/* Shot Positions */
ShotPos front_SP (FieldPos_Front, 90, 0);
ShotPos pf_SP (FieldPos_PF, 90, 70);
ShotPos pf_back_SP (FieldPos_PF_Back, 88, 0);
ShotPos back_SP (FieldPos_Back, 90, 10);

ShotSelect shot_req[2];
bool angler_pu_flag = false;

int shot_req_num = 0;
int shot_req_handled_num = 0;

bool shot_cancel_pressed = false;

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

    case FieldPos_PF_Back:
      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
      break;
  }
}

void set_turn_dir(Dir turn_dir) {
  shot_req[shot_req_num-1].turn_dir = turn_dir;
	if (shot_req[shot_req_num-1].field_pos == FieldPos_PF_Back_Red) { //Shooting from behind the platform
		shot_req[shot_req_num-1].flag_pos.y = 89;
		if (turn_dir == Dir_Left) {
			shot_req[shot_req_num-1].flag_pos.x = -30;
		}
		else if (turn_dir == Dir_Right) {
			shot_req[shot_req_num-1].flag_pos.x = 18;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}
	else if(shot_req[shot_req_num-1].field_pos == FieldPos_PF_Back_Blue) {
		shot_req[shot_req_num-1].flag_pos.y = 89;
		if (turn_dir == Dir_Left) {
			shot_req[shot_req_num-1].flag_pos.x = -18;
		}
		else if (turn_dir == Dir_Right) {
			shot_req[shot_req_num-1].flag_pos.x = 30;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}
	else { //Shooting from the back
		shot_req[shot_req_num-1].flag_pos.y = 125;
		if (turn_dir == Dir_Left) {
			shot_req[shot_req_num-1].flag_pos.x = -48;
		}
		else if (turn_dir == Dir_Right) {
			shot_req[shot_req_num-1].flag_pos.x = 48;
		}
		else shot_req[shot_req_num-1].flag_pos.x = 0;
	}
}

void set_handled_vars() {
	shot_req[shot_req_num-1].drive_turn_handled = false;
	shot_req[shot_req_num-1].shot_handled = false;
}

void set_shot_req(bool top, Dir turn_dir) {
	inc_shot_req_num();
	set_angle_targ(top);
	set_turn_dir(turn_dir);
	set_handled_vars();
	log_ln(LOG_SHOTS, "%d Shot Req (Task PTR:%d)| Anglr:%f | RNum:%d | HandledNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f), Hndled1 drive:%d shot:%d| 2angle:%d, 2turn:%d (%f, %f), Hndled1 drive:%d shot:%d ", pros::millis(), shot_req_handle_task, angler.get_position(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[0].drive_turn_handled, shot_req[0].shot_handled, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y, shot_req[1].drive_turn_handled, shot_req[1].shot_handled);
}

void shot_req_make() {
  //Set Field Pos
  if (shot_req_num == 0)
  {
    if (check_single_press(BTN_FIELD_FRONT)) set_field_pos(FieldPos_Front);
    else if (check_single_press(BTN_FIELD_PF_BACK_RED)) {
			set_field_pos(FieldPos_PF_Back_Red);
			angler_move(330, 100);
		}
		else if (check_single_press(BTN_FIELD_PF_BACK_BLUE)) {
			set_field_pos(FieldPos_PF_Back_Blue);
			angler_move(330, 100);
		}
		//else if (btn[BTN_SHOOT_CANCEL-6].pressed) set_field_pos(FieldPos_PF_Back);
    else if (check_single_press(BTN_FIELD_BACK)) set_field_pos(FieldPos_Back);
  }
	else if (check_single_press(BTN_SHOOT_CANCEL)) {
		shot_cancel_pressed = true;
		log_ln(LOG_SHOTS, "  >>> %d Cancel Shot Req Handle Task - Before Restart| TaskPtr %d | shot_req_num = %d, shot_req_handled_num = %d ", pros::millis(), shot_req_handle_task, shot_req_num, shot_req_handled_num);
		shot_req_handle_stop_task();
		log_ln(LOG_SHOTS, "  >>> %d Cancel Shot Req Handle Task - After Stop| TaskPtr %d | shot_req_num = %d, shot_req_handled_num = %d ", pros::millis(), shot_req_handle_task, shot_req_num, shot_req_handled_num);
		setDrive(0);

		shot_req_num = 0;
		shot_req_handled_num = 0;
		set_handled_vars();
		shot_req_handle_start_task();
		log_ln(LOG_SHOTS, "  >>> %d Shot Req Handle Task  - Resume | TaskPtr %d | shot_req_num = %d, shot_req_handled_num = %d ", pros::millis(), shot_req_handle_task, shot_req_num, shot_req_handled_num);
	}

  //Set other shot constants
		//Every button press after the second will overwrite the second.
		//So not all requests have been made, or the second request hasn't been executed yet -> Prevents from overwriting second request during execution
	if (shot_req_num < 2 || shot_req_handled_num < 1)
	{

		bool L_T, L_M, R_T, R_M;
		bool T_DOUBLE, M_DOUBLE;

		if (shot_req[0].field_pos == FieldPos_Back || shot_req[0].field_pos == FieldPos_PF_Back) { //Only register left buttons if shooting from front
			T_DOUBLE = check_double_press(BTN_SHOT_L_T, BTN_SHOT_R_T);
			M_DOUBLE = check_double_press(BTN_SHOT_L_M, BTN_SHOT_R_M);
			L_T = check_single_press(BTN_SHOT_L_T);
			L_M = check_single_press(BTN_SHOT_L_M);
		}
		else {
			T_DOUBLE = false;
			M_DOUBLE = false;
			L_T = false;
			L_M = false;
		}
		R_T = check_single_press(BTN_SHOT_R_T);
		R_M = check_single_press(BTN_SHOT_R_M);

		//Make shot requests depending on which button sequence was pressed
		if (T_DOUBLE) set_shot_req(true, Dir_Centre);
		else if (M_DOUBLE) set_shot_req(false, Dir_Centre);
		else if (L_T) set_shot_req(true, Dir_Left);
		else if (L_M) set_shot_req(false, Dir_Left);
		else if (R_T) set_shot_req(true, Dir_Right);
		else if (R_M) set_shot_req(false, Dir_Right);
	}
}

void shot_req_handle_stop_task() {
	if(shot_req_handle_task != nullptr)
	{
		log_ln(LOG_SHOTS, "  >>> %d Stop Shot Req Handle Task", pros::millis());
		shot_req_handle_task->remove();
		delete shot_req_handle_task;
		shot_req_handle_task = nullptr;
	}
}

void shot_req_handle_start_task() {
	shot_req_handle_stop_task();
	log_ln(LOG_SHOTS, "  >>> %d Start Shot Req Handle Task", pros::millis());
	shot_req_handle_task = new pros::Task((pros::task_fn_t)shot_req_handle);//, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Shot_Req_Handle_Task");
}


void shot_req_handle(void *param) {
	log_ln(LOG_SHOTS, "%d Start Shot Req Handle Task ",  pros::millis());
	shot_req_num = 0;
	while (true) {
		if (shot_req_num > 0) {
			log_ln(LOG_SHOTS, "%d Start handling first shot request ", pros::millis());
			set_handled_vars(); //Make sure all handled vars are reset to false
			shot_req_handled_num = 0; //Make sure we start handling shot requests from index 0

			if (shot_req[0].field_pos == FieldPos_Back || shot_req[0].field_pos == FieldPos_PF_Back)  {
				pos.reset(0,0,0);
				flatten_against_wall(shot_req[0].field_pos == FieldPos_Back ? false : true, true);
				pos.reset(0, 0, 0);
			}
			//Angle handle 1
			angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);

			//Drive Handle 1
			if (shot_req[shot_req_handled_num].field_pos == FieldPos_PF_Back) {
				log_ln(LOG_SHOTS, "%d S1 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				setDrive(0, -60, 0);
			  while (pos.y > -2_in) pros::delay(10);
			  log_ln(LOG_SHOTS, "%d Done Back up PF (%f, %f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			  turn_vel(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/70_deg), 0);
			}
			else if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				log_ln(LOG_SHOTS, "%d S1 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg), 0, true);
			}
			shot_req[shot_req_handled_num].drive_turn_handled = true;

			//Shooter Handle 1
			angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
			while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
			shot_req_handled_num++;

			if (shot_req_num > 1) {
				//Angle handle 1
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);

				//Drive Handle 2
				if (shot_req[shot_req_handled_num].field_pos == FieldPos_PF_Back) {
					log_ln(LOG_SHOTS, "%d S2 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				  turn_vel(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/70_deg), 0);
				}
				else if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
					log_ln(LOG_SHOTS, "%d S2 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
					turn_vel_side(new PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg), 0, true);
				}
				shot_req[shot_req_handled_num].drive_turn_handled = true;

				//Shooter Handle 2
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
				while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
				shot_req_handled_num++;

				angler.move_absolute(ANGLER_PU_POS, 200);
			}
			if (angler.get_position() < 10) angler.move_absolute(ANGLER_PU_POS, 200);

			shot_req_num = 0;
			shot_req_handled_num = 0;

		}
		pros::delay(10);
	}
}
