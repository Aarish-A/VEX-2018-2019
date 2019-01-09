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
  while (true) {
    pos.update();
    pros::delay(2);
  }
}

void autonomous() {
  pros::Task auto_update_task ((pros::task_fn_t)auto_update, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Auto_Update_Task");
  set_handled_vars(); //Make sure all handled vars are reset to false
  shot_req_handled_num = 0; //Make sure we start handling shot requests from index 0


    //setDrive(0, -60, 0);
    pos.reset(0,0,0);
    setDrive(0,-60, 0);
    pros::delay(200);
    do {
      printf("%d Reset Back Up(%f, %f, %f) Vel(%f, %f, %f) VeelLoc(%f, %f)\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.aVel, pos.velLocal.x, pos.velLocal.y);
      pros::delay(10);
    } while (pos.velLocal.y < -1); //aVel < -0.1);
    setDrive(0, -20, 0);


  pos.reset(0, 0, 0);
  /*
  drive_set(127);
  while (pos.y<1.5) pros::delay(10);
  */

  /*
  move_drive(pos.y+3, 200, false);
  turn_vel(new PointAngleTarget({48, 120}));
  */
  //turn_vel_side(new PointAngleTarget({-48, 120}), (200/50_deg));

}
