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
using namespace pros;

uint32_t print_time = 0;
void update_controller_lcd();
void update_game_side();

void opcontrol() {
	is_disabled = false;
	auto_update_stop_task();
	drive_set(0);
	log_ln(LOG_AUTO, "   --- %d START OPCONTROL --- \n", pros::millis());
	while(pros::competition::is_disabled() || !pros::competition::is_connected()) delay(10);
	autonomous();
	pun_state_change(PunState::Loading);
	if (pun_state != PunState::FatalError) {
		pun_set(127);
		shot_pun_go = false;
		auto_set_shot = false;
	}
	while (true) {
		pos.update();
		update_buttons();
		pun_handle();
		angler_handle();
		drive_handle();
		intake_handle();
		decapper_handle();

		if (partner_connected) gui_handle();
		if (!in_menu) {
			shot_req_make();
			update_controller_lcd();
			update_game_side();
		}

		delay(10);
	}
}

void update_game_side() {
	if (partner_connected && check_single_press(BTN_CHANGE_GAME_SIDE, true)) {
		printf("SAVINGGGG");
		if (game_side == 'R') game_side = 'B';
		else if (game_side == 'B') game_side = 'R';
		FILE* log = NULL;
		log = fopen("/usd/game_side.txt", "w");
		if (log == NULL) {
			printf("Couldn't create game side file\n");
		} else {
			fprintf(log, "%c", game_side);
			partner.rumble("..");
			fclose(log);
		}
	}
}

void update_controller_lcd() {
	if (millis() - print_time > 60) {
		print_time = millis();

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
		//ctrler.print(2, 0, "%s %s %d %d %d      ", field_pos_s, team_s, (int)intake.get_temperature(), (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature());
		//ctrler.print(2, 0, "a: %.1f       ", RAD_TO_DEG(getGlobalAngle()));
		// ctrler.print(2, 0, "%.1f %d %d    ", RAD_TO_DEG(getGlobalAngle()), enc_l.get_value(), enc_r.get_value());
		//ctrler.print(2, 0, "%f          ", ((enc_l.get_value() * SPN_TO_IN_L) - (enc_r.get_value() * SPN_TO_IN_R)) / 3600_deg);

	}
}
