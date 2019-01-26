#include "main.h"
#include "auto.hpp"
#include "config.hpp"
#include "puncher.hpp"
#include "shot_select.hpp"

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
    pun_handle();
    pros::delay(1);
  }
}
void autonomous() {
  pros::Task((pros::task_fn_t)auto_update);

  uint32_t autoStartTime = millis();
  setDriveVel(0);
  delay(10);

  //1 Pick up balls
  angler.move(ANGLER_PU_POS);
  intake.move(127);
  double cap_dis = 35.0_in;
  move_drive_rel(cap_dis, 200);
  delay(200);

  //2 Back up turn and shoot
  move_drive_rel(-cap_dis, 200);
  turn_vel(new FixedAngleTarget(-80.0_deg), (200/90_deg));
  intake.move(0);
  auto_set_shot = true;
  while (auto_set_shot) pros::delay(10);
  log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));


  //flatten_against_wall(true, true);

}
