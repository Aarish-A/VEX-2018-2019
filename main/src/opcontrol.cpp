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

using namespace pros;

void opcontrol() {
	int p_time = 0;
	shot_req_handle_start_task(); //Start shot req handle task
	printf("%d Start opcontrol\n", pros::millis());
	// ctrler.print(2, 0, "RUNNING");

	while (true) {
		pos.update();
		update_buttons();
		shot_req_make();
		pun_handle();
		drive_handle();
		intake_handle();
		angler_handle();
		decapper_handle();
		//printf("%d\n", (int)decapper.get_position());

		if (millis() - p_time > 100) {
			p_time = millis();
			std::string field_pos_s = "def";
			FieldPos field_pos= shot_req[0].field_pos;

			if (field_pos== FieldPos_Front) field_pos_s = "Fr ";
			else if (field_pos== FieldPos_Back) field_pos_s = "Bck";
			else if (field_pos== FieldPos_PF) field_pos_s = "PF ";
			else if (field_pos== FieldPos_PF_Back_Red) field_pos_s = "PfR";
			else if (field_pos== FieldPos_PF_Back_Blue) field_pos_s = "PfB";

			std::string team_s = blue_team? "b" : "r";

			ctrler.print(2, 0, "%s %s %d %d %d      ", field_pos_s, team_s, (int)intake.get_temperature(), (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature());
			//ctrler.print(2, 0, "%.1f %.1f %.1f     ", pos.x, pos.y, RAD_TO_DEG(pos.a));
			//ctrler.print(2, 0, "%d %d %.1f     ", enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(pos.a));
			//ctrler.print(2, 0, "%f          ", ((enc_l.get_value() * SPN_TO_IN_L) - (enc_r.get_value() * SPN_TO_IN_R)) / 3600_deg);

		}

		delay(10);
	}
}
