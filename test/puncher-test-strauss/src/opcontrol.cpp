#include "main.h"
#include <iostream>

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

enum class PunState { Load, Hold, ShotStart, ShotWait };

const double PUN_RATIO = (60.0 / 36.0);
const double PUN_OFFSET = 45 * PUN_RATIO;
const double PUN_HOLD = 180 * PUN_RATIO; // Relative to the slip point
const double PUN_TPR = 360 * PUN_RATIO; // Relative to the slip point
const double PUN_NO_RETURN = 50 * PUN_RATIO; // Back from the slip point
const double PUN_BALL_CHK_START = PUN_TPR - (PUN_HOLD + 5); // Back from the slip point
const uint32_t PUN_WAIT_TIME = 100;
const int PUN_BALL_THRESH = 2200;
const uint32_t PUN_BALL_OFF_TIME = 50;

Motor punMtr1(2, E_MOTOR_GEARSET_18);

void setPuncher(int power) {
	punMtr1.move(power);
	//punMtr2.move(power);
}

void movePuncher(double position, int32_t velocity = 200) {
	punMtr1.move_absolute(position, velocity);
	//punMtr2.move_absolute(position, velocity);
}

void logPuncher(void *param) {
	FILE* logFile = fopen("/usd/log.txt", "a");
	if (logFile == NULL) {
		printf("Could not open log file");
		return;
	}
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", logFile);

	int32_t voltageLst = 0;
	int32_t currentLst = 0;
	double positionLst = 0;

	uint32_t nextFlush = 0;
	bool written = false;

	while (true) {
		int32_t voltage = punMtr1.get_voltage();
		int32_t current = punMtr1.get_current_draw();
		double position = punMtr1.get_position();

		if (round(voltage / 100.0) != round(voltageLst / 100.0) || abs(current - currentLst) > 1 || round(position * 10) != round(positionLst * 10)) {
			fprintf(logFile, "%d %.1f %d %.1f\r\n", millis(), voltage / 1000.0, current, position);
			printf("%d %.1f %d %.1f\n", millis(), voltage / 1000.0, current, position);
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

void opcontrol() {
	Controller ctrler(E_CONTROLLER_MASTER);
	ADILineSensor ballSensor(7);
	punMtr1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	//punMtr2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	punMtr1.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
	//punMtr2.set_encoder_units(E_MOTOR_ENCODER_DEGREES);

	int shots = 0;
	bool ball = false;
	uint32_t ballOnTime = 0;

	Task logTask(logPuncher);

	uint32_t timeoutTime = millis() + 100;
	setPuncher(-20);
	while (fabs(punMtr1.get_actual_velocity()) < 15 && millis() < timeoutTime) {
		delay(10);
	}
	timeoutTime = millis() + 1300;
	while (fabs(punMtr1.get_actual_velocity()) > 10 && millis() < timeoutTime) {
		delay(10);
	}
	punMtr1.tare_position();
	//punMtr2.tare_position();
	setPuncher(0);

	movePuncher(PUN_OFFSET + PUN_HOLD);
	PunState punState = PunState::Load;
	uint32_t punWaitEnd = 0;

	uint32_t nextLCD = 0;

	while (true) {
		if (ballSensor.get_value() < PUN_BALL_THRESH) {
			ballOnTime = millis();
			ball = true;
		}

		if (millis() >= ballOnTime + PUN_BALL_OFF_TIME) {
			ball = false;
		}

		switch (punState) {
			case PunState::Load:
				if (fabs(punMtr1.get_position() - (PUN_OFFSET + (shots * PUN_TPR) + PUN_HOLD)) <= (4 * PUN_RATIO)) {
					setPuncher(5);
					punState = PunState::Hold;
				}
				if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
					movePuncher(PUN_OFFSET + (++shots * PUN_TPR));
					printf("%d Shot start\n", millis());
					punState = PunState::ShotStart;
				}
				break;

			case PunState::Hold:
				if (fabs(punMtr1.get_position() - (PUN_OFFSET + (shots * PUN_TPR) + PUN_HOLD)) > (6 * PUN_RATIO)) {
					movePuncher(PUN_OFFSET + (shots * PUN_TPR) + PUN_HOLD);
					punState = PunState::Load;
				}
				if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
					movePuncher(PUN_OFFSET + (++shots * PUN_TPR));
					printf("%d Shot start\n", millis());
					punState = PunState::ShotStart;
				}
				break;

			case PunState::ShotStart:
				if (punMtr1.get_position() < PUN_OFFSET + (shots * PUN_TPR) - PUN_NO_RETURN && punMtr1.get_position() > PUN_OFFSET + (shots * PUN_TPR) - PUN_BALL_CHK_START && !ball) {
					movePuncher(PUN_OFFSET + (--shots * PUN_TPR) + PUN_HOLD);
					printf("%d Shot failure, no ball\n", millis());
					ctrler.rumble(" .");
					punState = PunState::Load;
				}
				else if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_A) && punMtr1.get_position() < PUN_OFFSET + (shots * PUN_TPR) - PUN_NO_RETURN) {
					movePuncher(PUN_OFFSET + (--shots * PUN_TPR) + PUN_HOLD);
					printf("%d Shot failure, canceled\n", millis());
					punState = PunState::Load;
				}
				else if (PUN_OFFSET + (shots * PUN_TPR) - punMtr1.get_position() < 1) {
					punWaitEnd = millis() + PUN_WAIT_TIME;
					printf("%d Shot end\n", millis());
					punState = PunState::ShotWait;
				}
				break;

			case PunState::ShotWait:
				if (millis() >= punWaitEnd) {
					movePuncher(PUN_OFFSET + (shots * PUN_TPR) + PUN_HOLD);
					punState = PunState::Load;
				}
				break;
		}

		if (millis() >= nextLCD) {
			nextLCD = millis() + 100;
			ctrler.print(2, 0, "%d   ", (int)punMtr1.get_temperature());
		}

		/*if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)) {
			movePuncher(PUN_OFFSET + (shots * PUN_TPR) + PUN_HOLD);
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
			movePuncher(PUN_OFFSET + (++shots * PUN_TPR));
		}

		if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_A) && punMtr1.get_position() < PUN_OFFSET + (shots * PUN_TPR) - 50) {
			movePuncher(PUN_OFFSET + (--shots * PUN_TPR) + PUN_HOLD);
		}

		if (punMtr1.get_position() < PUN_OFFSET + (shots * PUN_TPR) - 50 && punMtr1.get_position() > PUN_OFFSET + (shots * PUN_TPR) - 150 && !ball) {
			movePuncher(PUN_OFFSET + (--shots * PUN_TPR) + PUN_HOLD);
			ctrler.rumble(" .");
		}*/

		delay(5);
	}
}
