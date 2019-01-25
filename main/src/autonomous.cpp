#include "main.h"
#include "auto.hpp"
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

using namespace pros;

void auto_update(void* param) {
  pos.reset();
  while (true)
  {
    pos.update();
    pros::delay(1);
  }
}
void autonomous() {
  pros::Task((pros::task_fn_t)auto_update);
  pros::delay(10);

  
  //flatten_against_wall(true, true);

}
