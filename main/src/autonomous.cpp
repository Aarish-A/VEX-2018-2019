#include "main.h"
#include "puncher.hpp"
#include "drive.hpp"
#include "tracking.hpp"
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

void auto_update() {
  pos.reset(0,0,0);
  while (true) {
    pos.update();
    pun_handle();
    //log("%d pos(%f, %f, %f) (%f, %f)\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), enc_l.get_value(), enc_r.get_value());
    pros::delay(1);
  }
}

void autonomous() {
  pros::Task auto_update_task ((pros::task_fn_t)auto_update, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Auto_Update_Task");

  pos.reset(0,0,0);
  flatten_against_wall(true, true);
  pros::delay(250);
  pos.reset(0, 0, 0);
  log_ln("%d Start Back", pros::millis());
  setDrive(0, -60, 0);
  while (pos.y > -2_in) pros::delay(10);
  log_ln("%d Done Back %f", pros::millis(), pos.y);
  turn_vel(new PointAngleTarget({-29_in, 89_in}), (200/70_deg), 0);
  auto_set_shot = true;
  while (auto_set_shot) pros::delay(10);
  log_ln("%d After wait a:%f", pros::millis(), RAD_TO_DEG(pos.a));
  turn_vel(new PointAngleTarget({19_in, 89_in}), (200/70_deg), 0);
  auto_set_shot = true;
  while (auto_set_shot) pros::delay(10);
  log_ln("%d After wait a:%f", pros::millis(), RAD_TO_DEG(pos.a));
  //setDrive(0, -15, 0);
  /*
  pos.reset(0, 0, 0);
  turn_vel(new PointAngleTarget({-10, 0}), (200/60_deg), 0);
  pros::delay(1000);
  */


  //Drive Handle 1/
  /*
  log_ln("%d S1 Turn to face %d, %d ", pros::millis(), 48, 128);
  turn_vel_side(new PointAngleTarget({48, 127}), (200/50_deg));

  pros::delay(1000);
  log( " >> %d (%f, %f, %f) \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a) );

  log_ln("%d S1 Turn to face %d, %d ", pros::millis(), -48, 127);
  turn_vel_side(new PointAngleTarget({-48, 127}), (200/50_deg));

  pros::delay(1000);
  log( " >> %d (%f, %f, %f) \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a) );


  drive_set(127);
  while (pos.y<1.5) pros::delay(10);
  */

  /*
  move_drive(pos.y+3, 200, false);
  turn_vel(new PointAngleTarget({48, 120}));
  */
  //turn_vel_side(new PointAngleTarget({-48, 120}), (200/50_deg));

}
