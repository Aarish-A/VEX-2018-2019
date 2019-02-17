#include "shot_select.hpp"

pros::Task* shot_req_handle_task = nullptr;

/* Shot Positions */
ShotPos front_SP (FieldPos_Front, 98, 0);
ShotPos auto_SP (FieldPos_Front, 88, 0);
ShotPos pf_SP (FieldPos_PF, 90, 70);
ShotPos pf_back_SP (FieldPos_PF_Back_Red, 88, 0);
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

    case FieldPos_PF_Back_Red:
      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top : pf_back_SP.mid;
      break;
		case FieldPos_PF_Back_Blue:
	      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top+18 : pf_back_SP.mid+18;
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
      shot_req[shot_req_num-1].angle_targ = top? pf_back_SP.top+18 : pf_back_SP.mid+18;
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
}

void set_shot_req(bool top, Dir turn_dir) {
	inc_shot_req_num();
	if(turn_dir == Dir_Right) set_angle_targ_right(top);
	else if(turn_dir == Dir_Left) set_angle_targ(top);
	top ? log_ln(LOG_SHOTS, "TOP PRESSED") : log_ln(LOG_SHOTS, "MID PRESSED");
	set_turn_dir(turn_dir);
	set_handled_vars();
	shot_req[shot_req_num-1].drive_turn_handled = true;
	log_ln(LOG_SHOTS, "%d Shot Req (Task PTR:%d)| Anglr:%f | RNum:%d | HandledNum:%d | FPos:%d | 1angle:%d, 1trn:%d (%f, %f), Hndled1 drive:%d shot:%d| 2angle:%d, 2turn:%d (%f, %f), Hndled1 drive:%d shot:%d ", pros::millis(), shot_req_handle_task, angler.get_position(), shot_req_num, shot_req_handled_num, shot_req[0].field_pos, shot_req[0].angle_targ, shot_req[0].turn_dir, shot_req[0].flag_pos.x, shot_req[0].flag_pos.y, shot_req[0].drive_turn_handled, shot_req[0].shot_handled, shot_req[1].angle_targ, shot_req[1].turn_dir, shot_req[1].flag_pos.x, shot_req[1].flag_pos.y, shot_req[1].drive_turn_handled, shot_req[1].shot_handled);
}

void shot_req_make() {
  //Set Field Pos
  if (shot_req_num == 0)
  {
    if (check_single_press(BTN_FIELD_FRONT, true)) {
			set_field_pos(FieldPos_Front);
			pos.reset();
		}
    else if (check_single_press(BTN_FIELD_PF_BACK_RED, true)) {
			set_field_pos(FieldPos_PF_Back_Red);
		}
		else if (check_single_press(BTN_FIELD_PF_BACK_BLUE, true)) {
			set_field_pos(FieldPos_PF_Back_Blue);
		}
		//else if (btn[BTN_SHOOT_CANCEL-6].pressed) set_field_pos(FieldPos_PF_Back);
    else if (check_single_press(BTN_FIELD_BACK, true)) set_field_pos(FieldPos_Back);
  }
	else if (check_single_press(BTN_SHOOT_CANCEL) || check_single_press(BTN_SHOOT_CANCEL, true)) {
		shot_cancel_pressed = true;
		//printf("%d IF 0: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		log_ln(LOG_SHOTS, "  >>> %d Cancel Shot Req Handle Task - Before Restart| TaskPtr %d | shot_req_num = %d, shot_req_handled_num = %d ", pros::millis(), shot_req_handle_task, shot_req_num, shot_req_handled_num);
		//printf("%d IF 1: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		shot_req_handle_stop_task();
		//printf("%d IF 2: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		log_ln(LOG_SHOTS, "  >>> %d Cancel Shot Req Handle Task - After Stop| TaskPtr %d | shot_req_num = %d, shot_req_handled_num = %d ", pros::millis(), shot_req_handle_task, shot_req_num, shot_req_handled_num);
		//printf("%d IF 3: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		setDrive(0);
		//printf("%d IF 4: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		//printf("%d IF 5: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		//printf("%d IF 6: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		set_handled_vars();
		//printf("%d IF 7: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		shot_req_handle_start_task();
		//printf("%d IF 8: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		shot_req_num = 0;
		shot_req_handled_num = 0;
		//printf("%d IF 9: %d %d\n \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
		log_ln(LOG_SHOTS, "  >>> %d Shot Req Handle Task  - Resume | TaskPtr %d | shot_req_num = %d, shot_req_handled_num = %d ", pros::millis(), shot_req_handle_task, shot_req_num, shot_req_handled_num);
	}

  //Set other shot constants
		//Every button press after the second will overwrite the second.
		//So not all requests have been made, or the second request hasn't been executed yet -> Prevents from overwriting second request during execution
	if (shot_req_num < 2 || shot_req_handled_num < 1)
	{

		bool L_T, L_M, R_T, R_M;
		bool T_DOUBLE, M_DOUBLE;

		if (shot_req[0].field_pos == FieldPos_Back || shot_req[0].field_pos == FieldPos_PF_Back_Blue || shot_req[0].field_pos == FieldPos_PF_Back_Red) { //Only register left buttons if shooting from front
			T_DOUBLE = check_double_press(BTN_SHOT_L_T, BTN_SHOT_R_T);
			M_DOUBLE = check_double_press(BTN_SHOT_L_M, BTN_SHOT_R_M);
			L_T = check_single_press(BTN_SHOT_L_T, true);
			L_M = check_single_press(BTN_SHOT_L_M, true);
			R_T = check_single_press(BTN_SHOT_R_T, true);
			R_M = check_single_press(BTN_SHOT_R_M, true);
		}
		else {
			T_DOUBLE = false;
			M_DOUBLE = false;
			L_T = false;
			L_M = false;
			R_T = check_single_press(BTN_SHOT_R_T);
			R_M = check_single_press(BTN_SHOT_R_M);
		}


		//Make shot requests depending on which button sequence was pressed
		if (T_DOUBLE) set_shot_req(true, Dir_Centre);
		else if (M_DOUBLE) set_shot_req(false, Dir_Centre);
		else if (L_T) {
			set_shot_req(true, Dir_Left);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot LT, L_T was %d", pros::millis(), (int)L_T);
		}
		else if (L_M) {
			set_shot_req(false, Dir_Left);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot LM, L_M was %d", pros::millis(), (int)L_M);
		}
		else if (R_T) {
			set_shot_req(true, Dir_Right);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot RT, R_T was %d", pros::millis(), (int)R_T);
		}
		else if (R_M) {
			set_shot_req(false, Dir_Right);
			log_ln(LOG_SHOTS, "%d Was supposed to shoot RM, R_M was %d", pros::millis(), (int)R_M);
		}
	}
}

void shot_req_handle_stop_task() {
	//printf("  >>> %d ---S_STOP 0 check - %d %d \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid);
	if(shot_req_handle_task != nullptr)
	{
		log_ln(LOG_SHOTS, "  >>> %d Stop Shot Req Handle Task", pros::millis());
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
	//shot_req_num = 0; //DELETE
	while (true) {
		if (is_disabled) printf(" >>> %d IN SHOT_REQ_HANDLE IN DISABLED S_R_N:\n", pros::millis());
		while(!check_single_press(BTN_SHOT_R_T) && (shot_req[0].field_pos == FieldPos_PF_Back_Red || shot_req[0].field_pos == FieldPos_PF_Back_Blue)) {
			pros::delay(10);
			//printf("Drive thing: %d\n", shot_req[shot_req_num-1].drive_turn_handled);
		}
		if (shot_req_num > 0) {
			if (!SHOT_DRIVE_BRAKE) setDrive(0); //Set drive pow to 0

			if (shot_req[0].field_pos == FieldPos_PF_Back_Red || shot_req[0].field_pos == FieldPos_PF_Back_Blue) {
				intake_state_set(0, IntakeState::Off);
				angler.move(0);
			}

			log_ln(LOG_SHOTS, "%d Start handling first shot request ", pros::millis());
			printf("I am here aarish\n");
			set_handled_vars(); //Make sure all handled vars are reset to false
			shot_req_handled_num = 0; //Make sure we start handling shot requests from index 0

			if (shot_req[0].field_pos == FieldPos_Back || shot_req[0].field_pos == FieldPos_PF_Back_Red || shot_req[0].field_pos == FieldPos_PF_Back_Blue)  {
				pos.reset(0,0,0);
				if (shot_req[0].field_pos == FieldPos_Back) flatten_against_wall(false, true);
				else if (shot_req[0].field_pos == FieldPos_PF_Back_Red || shot_req[0].field_pos == FieldPos_PF_Back_Blue) {
					setDrive(0, 10, 0);
					pros::delay(150);
					//flatten_angle(true, true, true);
				}
				pos.reset(0, 0, 0);
			}
			//Angle handle 1
			if (shot_req[shot_req_handled_num].angle_targ > ANGLER_PU_POS || (shot_req[0].field_pos != FieldPos_PF_Back_Red && shot_req[0].field_pos != FieldPos_PF_Back_Blue)) //Only move angler before drive if we are sure it won't hit PF
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);

			//Drive Handle 1
			if (shot_req[0].field_pos == FieldPos_PF_Back_Red || shot_req[0].field_pos == FieldPos_PF_Back_Blue) {
				log_ln(LOG_SHOTS, "%d S1 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				// setDrive(0, -40, 0);
			  // while (pos.y > -3.5_in) pros::delay(10);
				//
				// setDrive(0, 15, 0);

				// pros::delay(150);
				// setDrive(0);

				setDriveVel(0);
				pros::delay(20);
			  log_ln(LOG_SHOTS, "%d Done Back up PF (%f, %f, %f) Vel(%f, %f, %f)", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.velLocal.x, pos.velLocal.y, RAD_TO_DEG(pos.aVel));
				turn_vel_side(PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/60_deg), 0, false);
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
			}
			else if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
				log_ln(LOG_SHOTS, "%d S1 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				turn_vel_side(PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg), 0, true);
			}
			shot_req[shot_req_handled_num].angler_to = pros::millis() + ANGLER_REACH_T0;
			shot_req[shot_req_handled_num].drive_turn_handled = true;

			//Shooter Handle 1
			angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
			printf("angler shot target: %f\n", angler.get_target_position());
			while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
			shot_req_handled_num++;
			printf("angler shot target: %f\n", angler.get_position());
			if (shot_req_num > 1) {
				//Angle handle 1
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);

				//Drive Handle 2
				if (shot_req[0].field_pos == FieldPos_PF_Back_Red || shot_req[0].field_pos == FieldPos_PF_Back_Blue) {
					log_ln(LOG_SHOTS, "%d S2 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
				  turn_vel(PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/80_deg), 0);
				}
				else if (shot_req[shot_req_handled_num].field_pos == FieldPos_Back) {
					log_ln(LOG_SHOTS, "%d S2 Turn to face %f, %f ", pros::millis(), shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y);
					turn_vel_side(PointAngleTarget({shot_req[shot_req_handled_num].flag_pos.x, shot_req[shot_req_handled_num].flag_pos.y}), (200/50_deg), 0, true);
				}
				shot_req[shot_req_handled_num].angler_to = pros::millis() + ANGLER_REACH_T0;
				shot_req[shot_req_handled_num].drive_turn_handled = true;

				//Shooter Handle 2
				angler.move_absolute(shot_req[shot_req_handled_num].angle_targ, 200);
				while (!shot_req[shot_req_handled_num].shot_handled) pros::delay(10);
				shot_req_handled_num++;

				angler.move_absolute(ANGLER_PU_POS, 200);
			}
			angler.move_absolute(ANGLER_PU_POS, 200);
			intake_state_set(127, IntakeState::Forw);

			setDrive(0);
			log_ln(LOG_SHOTS, ">>> %d Done shot requests - move angler pos:%f", pros::millis(), angler.get_position());

			//if (angler.get_position() < ANGLER_BOT_LIM_POS) angler.move_absolute(ANGLER_PU_POS, 200);

			shot_req_num = 0;
			shot_req_handled_num = 0;
		}
		pros::delay(10);
	}
}
