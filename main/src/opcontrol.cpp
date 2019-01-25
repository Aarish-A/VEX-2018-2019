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

/* Info abt drive efficency */
const double EFF_LIM = 30;
void log_drive_efficency(void *param);
double eff_fl = drive_fl.get_efficiency();
double eff_bl = drive_bl.get_efficiency();
double eff_fr = drive_fr.get_efficiency();
double eff_br = drive_br.get_efficiency();

void opcontrol() {
	int print_time = 0;
	shot_req_handle_start_task(); //Start shot req handle task
	Task log_drive_efficency_task(log_drive_efficency); //Start logging drive efficency
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

		if (millis() - print_time > 100) {
			print_time = millis();
			std::string field_pos_s = "def";
			FieldPos field_pos= shot_req[0].field_pos;

			if (field_pos== FieldPos_Front) field_pos_s = "Fr ";
			else if (field_pos== FieldPos_Back) field_pos_s = "Bck";
			else if (field_pos== FieldPos_PF) field_pos_s = "PF ";
			else if (field_pos== FieldPos_PF_Back_Red) field_pos_s = "PfR";
			else if (field_pos== FieldPos_PF_Back_Blue) field_pos_s = "PfB";

			std::string team_s = blue_team? "b" : "r";

			eff_fl = drive_fl.get_efficiency();
			eff_bl = drive_bl.get_efficiency();
			eff_fr = drive_fr.get_efficiency();
			eff_br = drive_br.get_efficiency();

			if (eff_fl < EFF_LIM || eff_fr < EFF_LIM || eff_bl < EFF_LIM || eff_br < EFF_LIM)
				ctrler.rumble(". .");

			//ctrler.print(2, 0, "%s %.1f %.1f %.1f %.1f    ", field_pos_s, eff_fl, eff_bl, eff_fr, eff_br);

			//ctrler.print(2, 0, "%s %s %d %d %d      ", field_pos_s, team_s, (int)intake.get_temperature(), (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature());

			ctrler.print(2, 0, "%.1f %.1f %.1f     ", pos.x, pos.y, RAD_TO_DEG(pos.a));
			//ctrler.print(2, 0, "%d %d %.1f     ", enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(pos.a));
			//ctrler.print(2, 0, "%f          ", ((enc_l.get_value() * SPN_TO_IN_L) - (enc_r.get_value() * SPN_TO_IN_R)) / 3600_deg);

		}

		delay(10);
	}
}

void log_drive_efficency(void *param)
{
	const int FLUSH_DELAY = 10000;
	const int LOG_DELAY = 1000;

	const char* file_name = "/usd/drive_efficency.txt";

	FILE* drive_efficency = fopen(file_name, "a");
	pros::delay(500);
	if (drive_efficency == NULL)
	{
		log_ln(LOG_DRIVE,"	>>>> Could not open drive efficency log file");
		return;
	}
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", drive_efficency);

	uint32_t nextFlush = 0;

	while (true)
	{
		fprintf(drive_efficency, "%f, %f, %f, %f \r\n", eff_fl, eff_bl, eff_fr, eff_br);

		if (millis() >= nextFlush)
		{
			fclose(drive_efficency);
			while ((drive_efficency = fopen(file_name, "a")) == NULL) delay(5);
			nextFlush = millis() + FLUSH_DELAY;
		}

		delay(LOG_DELAY);
	}
}
