#include "main.h"
#include "logs.hpp"
#include "config.hpp"
#include "controls.hpp"
#include "macros/shot_queueing.hpp"
#include "gui.hpp"
#include "libraries/util.hpp"
#include "libraries/task.hpp"

void opcontrol() {
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());
	pilons::Task::stop_all_tasks();
	Subsystem::enable_all();
	enc_r.reset();
	enc_l.reset();

	while (true) {
		// printf("R: %d, L: %d\n", enc_r.get_value(), enc_l.get_value());
		//printf("Light sensor:%d\n",right_platform_sensor.get_value());
		//printf("poti: %d\n",s_pole_poti.get_value());
		// master.print(2,0,"%d %d                 ",enc_l.get_value(), enc_r.get_value());
		std::string fps = "";
		switch(field_position) {
			case Field_Position::FRONT: fps = "Front      "; break;
			case Field_Position::RED_PF: fps = "RedPF     "; break;
			case Field_Position::BLUE_PF: fps = "BluePF   "; break;
			case Field_Position::BACK: fps = "Back        "; break;
		}
		master.print(2, 0, "%c %s", game_side, fps.c_str());

		pos.update();
		master.update();
		partner.update();
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

		/* Intake */
		if (master.check_single_press(BTN_INTAKE_UP)) intake.off() ? intake.intake() : intake.stop();
		else if (master.check_single_press(BTN_INTAKE_DOWN)) intake.off() ? intake.outtake() : intake.stop();

		/* Puncher */
		// if (master.check_single_press(BTN_SHOT_CANCEL)) puncher.cancel_shot();
		// else if (master.check_single_press(BTN_SHOT_R_T)) puncher.shoot();

		/* Macros */
		if (master.check_single_press(BTN_GROUND_PICKUP)) {
			angler.move_to(Angler::PICKUP_POSITION);
			intake.intake();
		}

		else if (master.check_single_press(BTN_CAP_PICKUP)) {
			angler.move_to(Angler::CAP_PICKUP_POSITION);
			intake.intake();
		}

		else if (master.check_double_press(BTN_GROUND_PICKUP, BTN_CAP_PICKUP)) {
			angler.move_to(Angler::CAP_FLIP_POSITION);
			intake.outtake();
		}

		if (master.check_single_press(BTN_SHOT_R_T)) {
			make_shot_request(front_SP.top, Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
			make_shot_request(front_SP.top - 30, Turn_Direction::RIGHT, Field_Position::RED_PF, true);
			make_shot_request(0, Turn_Direction::RIGHT, Field_Position::BLUE_PF, true);
		}
		else if (master.check_single_press(BTN_SHOT_R_M)) {
			make_shot_request(front_SP.mid, Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
		}
		else if (master.check_single_press(BTN_SHOT_L_T)) {
			make_shot_request(front_SP.top, Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
			make_shot_request(front_SP.top - 30, Turn_Direction::LEFT, Field_Position::RED_PF, true);
			make_shot_request(0, Turn_Direction::LEFT, Field_Position::BLUE_PF, true);
		}
		else if (master.check_single_press(BTN_SHOT_CANCEL)) {
			shot_queue_handle_task.stop_task();
		}

		if (master.check_single_press(BTN_FIELD_FRONT)) {
			field_position = Field_Position::FRONT;
		}
		else if (master.check_single_press(BTN_FIELD_RED_PF)) {
			field_position = Field_Position::RED_PF;
		}
		else if (master.check_single_press(BTN_FIELD_BLUE_PF)) {
			field_position = Field_Position::BLUE_PF;
		}

		// if (partner.check_single_press(BTN_SHOT_L_T)) {
		// 	make_shot_request(platform_SP.top, Turn_Direction::LEFT, Field_Position::BLUE_PF);
		// 	make_shot_request(platform_SP.top, Turn_Direction::LEFT, Field_Position::RED_PF);
		// }
		//
		// else if (partner.check_single_press(BTN_SHOT_L_M)) {
		// 	make_shot_request(platform_SP.mid, Turn_Direction::LEFT, Field_Position::BLUE_PF);
		// 	make_shot_request(platform_SP.mid, Turn_Direction::LEFT, Field_Position::RED_PF);
		// }
		//
		// else if (partner.check_single_press(BTN_SHOT_R_T)) {
		// 	make_shot_request(platform_SP.top, Turn_Direction::RIGHT, Field_Position::BLUE_PF);
		// 	make_shot_request(platform_SP.top, Turn_Direction::RIGHT, Field_Position::RED_PF);
		// }
		//
		// else if (partner.check_single_press(BTN_SHOT_R_M)) {
		// 	make_shot_request(platform_SP.mid, Turn_Direction::RIGHT, Field_Position::BLUE_PF);
		// 	make_shot_request(platform_SP.mid, Turn_Direction::RIGHT, Field_Position::RED_PF);
		// }

		pros::delay(5);
	}
}
