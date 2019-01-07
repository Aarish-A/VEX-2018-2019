#include "main.h"
#include "puncher.hpp"
#include "drive.hpp"
#include "tracking.hpp"
#include "config.hpp"

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

void auto_update() {
  while (true) {
    pos.update();
    pros::delay(2);
  }
}

void autonomous() {
  pros::Task auto_update_task ((pros::task_fn_t)auto_update, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Auto_Update_Task");
  while (true) {
    printf("%d %f %f %f\n", pros::millis(), pos.x, pos.y, pos.a);;
    pros::delay(10);
  }
  //pos.reset();
  //auto_set_shot = true;

}
