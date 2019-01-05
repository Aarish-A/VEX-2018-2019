#include "main.h"
#include "config.hpp"
#include "auto.hpp"

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

using namespace pros;

void autonomous() {
  uint32_t autoStartTime = millis();
  resetGlobalAngle();
  setDriveVel(0);
  delay(10);

	angler.move(-30);
	delay(100);
	while (angler.get_actual_velocity() < -10) delay(10);
	angler.tare_position();
	angler.move_absolute(17 * 7, 100);
  delay(100);

  moveDrive(20.0_in, 200);
  return;

  moveDrive(36.0_in, 200);
  delay(1000);
  moveDrive(-40.0_in, 200);
  turnDrive(-90.0_deg, 200);
  moveDrive(45.0_in, 200);
  moveDrive(-45.0_in, 200);
  turnDrive(-45.0_deg, 200);
  moveDrive(22.0_in, 200);

  printf("Auto time: %d\n", millis() - autoStartTime);

  /*
  setDrive(0, -20, 0);
  delay(300);
  moveDrive(10.0_in, 150);
  turnDrive(-90.0_deg, 150);
  moveDrive(-30.0_in, 150);
  turnDrive(30.0_deg, 150);
  moveDrive(15.0_in, 150);
  turnDrive(-60.0_deg, 150);
  moveDrive(15.0_in, 150);
  turnDrive(210.0_deg, 150);
  moveDrive(-4.75_in, 150);
  turnDrive(-90.0_deg, 150);
  printf("%f\n", (DRIVE_DIA * M_PI * (driveFL.get_position() - driveBR.get_position() + driveBL.get_position() - driveFR.get_position())) / (2 * DRIVE_TPR * (DRIVE_LENGTH + DRIVE_WIDTH)) * 180 - M_PI);
  moveDrive(-9.0_in, 150, false);
  setDrive(0, -20, 0);
  delay(500);
  setDrive(0, 0, 0);
  */
}
