#include "main.h"
#include "logs.hpp"
#include "config.hpp"
#include "controls.hpp"
#include "macros/shot_queueing.hpp"
#include "gui.hpp"
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

void opcontrol() {
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());
	autonomous();
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
			case Field_Position::FRONT: fps   = "Front          "; break;
			case Field_Position::RED_PF: fps  = "RedPF          "; break;
			case Field_Position::BLUE_PF: fps = "BluePF         "; break;
			case Field_Position::BACK: fps    = "Back           "; break;
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
				make_shot_request(front_SP.top, Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
				break;
			case BTN_SHOT_R_M:
				make_shot_request(front_SP.mid, Turn_Direction::STRAIGHT, Field_Position::FRONT, true);
				break;

			// Capper
			case BTN_CAPPER_DOWN:
				capper.move_to_pickup();
				angler.move_to(Angler::PICKUP_POSITION);
				intake.stop();
				break;
			case BTN_CAPPER_UP:
				if (capper.at_pickup_position()) capper.pickup_cap();
				else if (cap_on_pole_task.running()) cap_on_pole_task.stop_task();
				else cap_on_pole_task.start_task();
				break;
			case BTN_CAP_FLIP:
				capper_move_to_cap_flip_task.start_task();
				break;
		}


		if (master.check_double_press(BTN_GROUND_PICKUP, BTN_CAP_PICKUP)) {
			angler.move_to(Angler::CAP_FLIP_POSITION);
			intake.outtake();
		}

		pros::delay(5);
	}
}
