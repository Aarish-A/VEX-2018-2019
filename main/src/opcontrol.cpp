#include "main.h"
#include "puncher.hpp"
#include "drive.hpp"
#include "intake.hpp"
#include "angler.hpp"
#include "tracking.hpp"
#include "shot_select.hpp"
#include "button.hpp"

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

using namespace pros;
/*
void logPuncher(void *param) {
	FILE* logFile = fopen("/usd/log.txt", "a");
	if (logFile == NULL) {
		log_ln("Could not open log file");
		return;
	}
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", logFile);

	int32_t voltageLst = 0;
	int32_t currentLst = 0;
	double positionLst = 0;

	uint32_t nextFlush = 0;
	bool written = false;

	while (true) {


		if (round(voltage / 100.0) != round(voltageLst / 100.0) || abs(current - currentLst) > 1 || round(position * 10) != round(positionLst * 10)) {
			fprintf(logFile, "%d Pos: %d State: %d %d\r\n", millis(), voltage / 1000.0, current, position);
			log_ln("%d %.1f %d %.1f", millis(), voltage / 1000.0, current, position);
			written = true;
			voltageLst = voltage;
			currentLst = current;
			positionLst = position;
		}

		if (written && millis() >= nextFlush) {
			fclose(logFile);
			while ((logFile = fopen("/usd/log.txt", "a")) == NULL) delay(5);
			written = false;
			nextFlush = millis() + 1000;
		}

		delay(20);
	}
}
*/

void opcontrol() {
	uint32_t lstTime = 0;
	log_ln("%d Start Opcontrol ", pros::millis());
	drive_set(0);

	while (true) {
		for (int i = 0; i < 12; i++) {
			btn[i].check_pressed();
			//if (btn[i].pressed)log_ln("Pressed: %d ", btn[i].btn_name);
		}


		shot_req_make();

		pun_handle();
		drive_handle();
		intake_handle();
		angler_handle();

		pos.update();

		if (millis() - lstTime > 100) {
			lstTime = millis();
			std::string field_pos_s = "def";
			FieldPos field_pos= shot_req[0].field_pos;

			if (field_pos== FieldPos_Front) field_pos_s = "Fr";
			else if (field_pos== FieldPos_Back) field_pos_s = "Bck";
			else if (field_pos== FieldPos_PF) field_pos_s = "PF";
			else if (field_pos== FieldPos_PF_Back) field_pos_s = "PfB";

			vision_object_s_t object_arr[1];
			vision_sensor.read_by_size(0, 1, object_arr);
			ctrler.print(2, 0, "LC: %d, Width: %d", object_arr[0].left_coord, object_arr[0].width);
			// ctrler.print(2, 0, "%.1f,%.1f,%.1f p:%d", pos.x, pos.y, RAD_TO_DEG(pos.a));

			//ctrler.print(2, 0, "%s %d %d %d", field_pos_s, (int)intake.get_temperature(), (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature());
			//ctrler.print(2, 0, "%d%d%d%d%d%def  ", (int)puncherLeft.get_temperature(), (int)puncherRight.get_temperature(), (int)drive_fl.get_temperature(), (int)drive_fr.get_temperature(), (int)drive_bl.get_temperature(), (int)drive_br.get_temperature());
			//ctrler.print(2, 0, "%.3f", ((enc_l.get_value() * SPN_TO_IN_L) - (enc_r.get_value() * SPN_TO_IN_R)) / 3600_deg);
			//log_ln("%d %.3f %.3f", millis(), puncherLeft.get_power(), puncherRight.get_power());
			//log_ln("%d | P:%f TP:%f V:%f TV:%f P:%f ", millis(), angler.get_position(), angler.get_target_position(), angler.get_actual_velocity(), angler.get_target_velocity(), angler.get_power());
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)) {
			vision_object_s_t object_arr[1];
			vision_sensor.read_by_size(0, 1, object_arr);
			printf("PRINTED SIGNATURE");
			printf("Signature: %d", object_arr[0].signature);
			ctrler.print(2, 0, "   %d", object_arr[0].signature);
		}

		delay(10);
	}
}
