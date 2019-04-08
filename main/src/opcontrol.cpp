#include "main.h"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "controls.hpp"
#include "macros/shot_queueing.hpp"
#include "menu.hpp"
#include "libraries/util.hpp"
#include "libraries/task.hpp"

void capper_move_to_cap_flip_macro(void* _params) {
	capper.move_to_velocity(39 * Capper::GEAR_RATIO, 200);
	pros::delay(200);
	intake.intake();
}

pilons::Task capper_move_to_cap_flip_task("Capper Cap Flip", [](void* param) {
	capper.move_to_velocity(37 * Capper::GEAR_RATIO, 200);
	pros::delay(200);
	intake.intake();
	angler.move_to(Angler::PICKUP_POSITION);
});

double get_shot_angle(long double x, long double _y) {
	constexpr long double ANGLER_MIN_ANGLE = 24.2;
	constexpr long double ANGLER_MAX_ANGLE = 120.0;
	constexpr long double ANGLER_HEIGHT = 18_in;
	constexpr long double DX = 5.0_in;
	constexpr long double M_BOLT = 54.3;
	constexpr long double M_BALL = 56.0;
	constexpr long double G = 9.80665;

	long double y = _y - ANGLER_HEIGHT;

	long double k = 100.0; // N / in
	long double v_bolt = sqrt(k * DX * DX / M_BOLT);
	long double v_ball = (2 * M_BOLT) / (M_BOLT + M_BALL) * v_bolt;

	long double t1 = pow(v_ball, 2.0);

	long double temp = pow(v_ball, 4.0) - G * (G * x * x + 2 * y * v_ball * v_ball);
	long double t2 = 0;
	if (temp >= 0) t2 = sqrt(temp);
	else return 0;
	long double t3 = G * x;

	long double angle_1 = (atan((t1 + t2) / t3) * 180 / M_PI) - ANGLER_MIN_ANGLE;
	long double angle_2 = (atan((t1 - t2) / t3) * 180 / M_PI) - ANGLER_MIN_ANGLE;

	bool angle_1_valid = false;
	bool angle_2_valid = false;

	if (angle_1 >= 0 && angle_1 <= ANGLER_MAX_ANGLE) angle_1_valid = true;
	if (angle_2 >= 0 && angle_2 <= ANGLER_MAX_ANGLE) angle_2_valid = true;

	if (angle_1_valid && !angle_2_valid) return angle_1;
	else if (!angle_1_valid && angle_2_valid) return angle_2;
	else return fminl(angle_1, angle_2);
}


void opcontrol() {
	log_ln(PROGRAM_FLOW, "   --- %d START OPCONTROL --- \n", pros::millis());
<<<<<<< HEAD
	if (auto_routine == Auto_Routines::DRIVER_SKILLS) autonomous();
=======
	//autonomous();
>>>>>>> f0758f749a6f845cea5e9be6e90d189ab163b933
	pilons::Task::stop_all_tasks();
	Subsystem::enable_all();
	enc_r.reset();
	enc_l.reset();
	pos.reset();
	while (true) {
		// printf("R: %d, L: %d\n", enc_r.get_value(), enc_l.get_value());
		// printf("Light sensor:%f\n",gyro.get_value());
		//printf("poti: %d\n",s_pole_poti.get_value());
		// master.print(2,0,"%d %d                 ",enc_l.get_value(), enc_r.get_value());
		std::string fps = "";
		switch(field_position) {
			case Field_Position::FRONT: fps   = "Front    "; break;
			case Field_Position::RED_PF: fps  = "RedPF    "; break;
			case Field_Position::BLUE_PF: fps = "BluePF   "; break;
			case Field_Position::BACK: fps    = "Back     "; break;
		}

		pos.update();
		master.update();
		if (partner.is_connected()) {
			partner.write_line(0, " ");
			partner.write_line(1, " ");
			partner.write_line(2, "%c %s", game_side, fps.c_str());
			partner.update();
		}
		intake.update();
		drive.update();
		angler.update();
		puncher.update();
		capper.update();

		/* Drive */
		int8_t drive_y = master.get_analog(JOY_DRIVE_THROTTLE, 10);
		int8_t drive_x = master.get_analog(JOY_DRIVE_STRAFE, 10);
		int8_t drive_a = master.get_analog(JOY_DRIVE_TURN, 25);
		drive.driver_set(drive_x, drive_y, drive_a);

		/* Angler */
		int8_t angler_power = master.get_analog(JOY_ANGLER, 40);
		angler.driver_set(angler_power);
		// printf("%f\n", m_angler.get_position() / 7.0);
		// 24.2

		if (!menu_enabled) {
			master.write_line(0, " ");
			master.write_line(1, " ");
			master.write_line(2, "%c %s", game_side, fps.c_str());

			switch(master.single_pressed) {
				// Intake
				case BTN_INTAKE_UP:
					(intake.off() && !capper.at_pickup_position()) ? intake.intake() : intake.stop();
					break;
				case BTN_INTAKE_DOWN:
					(intake.off() && !capper.at_pickup_position()) ? intake.outtake() : intake.stop();
					break;

				// Angler
				case BTN_GROUND_PICKUP:
					angler.move_to(Angler::PICKUP_POSITION);
					intake.intake();
					break;
				case BTN_CAP_PICKUP:
					angler.move_to(Angler::CAP_PICKUP_POSITION);
					intake.intake();
					break;

				// Shot Queueing
				case BTN_SHOT_R_T:
					make_shot_request(shot_positions[(int)SP::G_FRONT_TOP], Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
					break;
				case BTN_SHOT_R_M:
					make_shot_request(shot_positions[(int)SP::G_FRONT_MID], Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
					break;
				// case BTN_SHOT_CANCEL:
				// 	shot_queue_handle_task.stop_task();
				// 	break;

				// Capper
				case BTN_CAPPER_DOWN:
					capper.move_to_pickup();
					angler.move_to(Angler::PICKUP_POSITION);
					intake.stop();
					break;
				case BTN_CAPPER_UP:
				if(partner.is_connected()) change_field_position(Field_Position::FRONT);
				else {
					if (capper.at_pickup_position()) capper.pickup_cap();
					else if (cap_on_pole_task.running()) cap_on_pole_task.stop_task();
					else cap_on_pole_task.start_task();
				}
				break;
				case BTN_CAP_FLIP:
					capper_move_to_cap_flip_task.start_task();
					break;
				case BTN_DRIVE_LOCK:
					drive.lock();
					break;
			}
		} else {
			menu_update();
			switch(master.single_pressed) {
				case BTN_MENU_NEXT_SCREEN: menu_next_screen(); break;
				case BTN_MENU_PREVIOUS_SCREEN: menu_previous_screen(); break;
				case BTN_MENU_NEXT_ELEMENT: menu_next_element(); break;
				case BTN_MENU_PREVIOUS_ELEMENT: menu_previous_element(); break;
				case BTN_MENU_INCREMENT_ACTION: menu_element_increment_action(); break;
				case BTN_MENU_DECREMENT_ACTION: menu_element_decrement_action(); break;
				case BTN_MENU_SELECTED_ACTION: menu_selected_action(); break;
				case BTN_MENU_CLEAR: menu_clear(); break;
				case BTN_MENU_SAVE: menu_save(); break;
			}
		}

		if (master.check_double_press(BTN_SHOT_R_M, BTN_SHOT_L_M)) {
			printf("Menu enabled/renabled\n");
			menu_enabled = !menu_enabled;
		}

		if (partner.is_connected()) {
			switch(partner.single_pressed) {
				case BTN_SHOT_R_T:
					make_shot_request(shot_positions[(int)SP::G_PLATFORM_TOP], Turn_Direction::RIGHT, field_position);
					break;
				case BTN_SHOT_R_M:
					make_shot_request(shot_positions[(int)SP::G_PLATFORM_MID], Turn_Direction::RIGHT, field_position);
					break;
				case BTN_SHOT_L_T:
					make_shot_request(shot_positions[(int)SP::G_PLATFORM_TOP], Turn_Direction::LEFT, field_position);
					break;
				case BTN_SHOT_L_M:
					make_shot_request(shot_positions[(int)SP::G_PLATFORM_MID], Turn_Direction::LEFT, field_position);
					break;
				case BTN_FIELD_RED_PF:
					change_field_position(Field_Position::RED_PF);
					break;
				case BTN_FIELD_BLUE_PF:
					change_field_position(Field_Position::BLUE_PF);
					break;
				case BTN_FIELD_FRONT:
					change_field_position(Field_Position::FRONT);
					break;
				case BTN_SHOT_CANCEL:
					change_field_position(Field_Position::BACK);
					break;
			}
		//
		//
		// 	if (master.check_double_press(BTN_GROUND_PICKUP, BTN_CAP_PICKUP)) {
		// 		angler.move_to(Angler::CAP_FLIP_POSITION);
		// 		intake.outtake();
		// 	}
		// }
		//
		//
			if(partner.check_double_press(BTN_SHOT_R_M, BTN_SHOT_L_M))
			{
				make_shot_request(shot_positions[(int)SP::G_BACK_MID], Turn_Direction::STRAIGHT, field_position);
			}
			if(partner.check_double_press(BTN_SHOT_R_T, BTN_SHOT_L_T))
			{
				make_shot_request(shot_positions[(int)SP::G_BACK_TOP], Turn_Direction::STRAIGHT, field_position);
			}
		// if (master.check_double_press(BTN_GROUND_PICKUP, BTN_CAP_PICKUP)) {
		// 	angler.move_to(Angler::CAP_FLIP_POSITION);
		// 	intake.outtake();
		// }

		pros::delay(5);
	}
}
}
