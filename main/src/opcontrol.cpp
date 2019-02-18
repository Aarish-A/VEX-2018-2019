#include "main.h"
#include "button.hpp"
#include "controls.hpp"
#include "angler.hpp"
#include "puncher.hpp"
#include "intake.hpp"
#include "drive.hpp"
#include "logs.hpp"
#include "gui.hpp"
#include "decapper.hpp"
#include "vision.hpp"
#include "config.hpp"
#include "auto.hpp"
//#include "autonomous.cpp"
using namespace pros;

uint32_t print_time = 0;
void update_controller_lcd();

void opcontrol() {
	//resetGlobalAngle();
	is_disabled = false;
	auto_update_stop_task();
	drive_set(0);
	//Task log_drive_efficency_task(log_drive_efficency); //Start logging drive efficency
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());
	// ctrler.print(2, 0, "RUNNING");

	//pun_state_change(PunState::Loading);

	u_int32_t loop_counter = 0;
	while (true) {
		pos.update();
		// printf("Left Tracking %d\n", enc_l.get_value());
		// printf("Right Tracking %d\n", enc_r.get_value());
		// printf("Global angle: %f\n", RAD_TO_DEG(getGlobalAngle()));
		update_buttons();
		if (current_gui_tab == gui_tab_states::diagnostics_tab) {
			shot_req_make();
			drive_handle();
			intake_handle();
			angler_handle();
			//decapper_handle();
			update_controller_lcd();
		} else {
			gui_handle();
		}
		pun_handle();
		vision_handle();
		//printf("%d %d %d \n", pros::millis(), enc_l.get_value(), enc_r.get_value());
		// printf("%d\n",autoPoti.get_value());
		// printf("%d\n", autoPoti.get_value());
		//printf("%d\n", (int)decapper.get_position());
		//log_ln(LOG_SHOTS, "  >>> %d PF BACK: %d %d | l_counter:%d", pros::millis(), pf_back_SP.top, pf_back_SP.mid, loop_counter);
		//printf("  >>> %d PRINT - PF BACK: %d %d | l_counter:%d \n", pros::millis(), pf_back_SP.top, pf_back_SP.mid, loop_counter);
		//printf("%d, %f to %f \n", pros::millis(), puncherLeft.get_position(), PUN_OFFSET + (1 * PUN_TPR) - PUN_BALL_CHK_START[1]);
		loop_counter++;
		delay(10);
	}
}

void update_controller_lcd() {
	if (millis() - print_time > 30) {
		print_time = millis();
		std::string field_pos_s = "def";
		FieldPos field_pos= shot_req[0].field_pos;

		if (field_pos== FieldPos_Front) field_pos_s = "Fr ";
		else if (field_pos== FieldPos_Back) field_pos_s = "Bck";
		else if (field_pos== FieldPos_PF) field_pos_s = "PF ";
		else if (field_pos== FieldPos_PF_Back_Red) field_pos_s = "PfR";
		else if (field_pos== FieldPos_PF_Back_Blue) field_pos_s = "PfB";

		std::string team_s = blue_team? "b" : "r";
		//ctrler.print(2, 0, "%d %d%d%d%d%d%d      ", field_pos_s, team_s, (int)angler.get_temperature(), (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature(), (int)drive_fl.get_temperature(), (int)drive_fr.get_temperature(), (int)drive_bl.get_temperature(), (int)drive_br.get_temperature());
		//ctrler.print(2, 0, "BD: %d   ", ball_sensor.get_value());
		// eff_fl = drive_fl.get_efficiency();
		// eff_bl = drive_bl.get_efficiency();
		// eff_fr = drive_fr.get_efficiency();
		// eff_br = drive_br.get_efficiency();

		/*if (eff_fl < EFF_LIM || eff_fr < EFF_LIM || eff_bl < EFF_LIM || eff_br < EFF_LIM)
			ctrler.rumble(". .");
			*/

		// ctrler.print(2, 0, "%d,%d,%d,%d", (int)drive_bl.get_temperature(), (int)drive_br.get_temperature(), (int)drive_fl.get_temperature(), (int)drive_fr.get_temperature());
		partner.print(2, 0, "%s    ", field_pos_s);
		//ctrler.print(2, 0, "%s %s %d %d %d      ", field_pos_s, team_s, (int)intake.get_temperature(), (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature());
		// ctrler.print(2, 0, "%d  ", (int)current_auto_routine);
		//ctrler.print(2, 0, "a: %.1f       ", RAD_TO_DEG(getGlobalAngle()));
		ctrler.print(2, 0, "%.1f %d %d    ", RAD_TO_DEG(getGlobalAngle()), enc_l.get_value(), enc_r.get_value());
		//ctrler.print(2, 0, "%f          ", ((enc_l.get_value() * SPN_TO_IN_L) - (enc_r.get_value() * SPN_TO_IN_R)) / 3600_deg);

	}
}
