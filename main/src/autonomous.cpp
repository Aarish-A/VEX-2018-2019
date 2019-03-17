#include "main.h"
#include "config.hpp"
#include "macros/auto.hpp"

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

uint32_t autoStartTime;

void auto_red_front();
void auto_red_back();
void auto_red_back_no_second_shot();
void auto_blue_front();
void auto_blue_back();
void auto_blue_back_no_second_shot();
void programming_skills();

void autonomous() {
  // Auto Start
  printf("Started Auto!\n");
  uint32_t autonomous_time = pros::millis();
  Subsystem::enable_all();
  auto_update_task.start_task();
  drive.reset_global_angle();

  // Auto
  // drive_move_sync(40_in, 0_deg);

  printf("here0\n");
  angler.move_to(Angler::PICKUP_POSITION);

  printf("here1\n");
  drive_turn_async(FixedAngleTarget(90_deg));
  pros::delay(10000);
  printf("here2\n");
  drive.wait_for_angle(2_deg);
  printf("here3, %f\n", drive.get_error());
  single_shot(5);
  drive.wait_for_stop();


  // Auto End
  pilons::Task::stop_all_tasks();
  uint32_t auto_finished_time = pros::millis() - autonomous_time;
  master.print(2, 0, "Time: %d", auto_finished_time);
}


void auto_red_front() {

}

void auto_red_back() {

}

void auto_red_back_no_second_shot() {

}

void auto_blue_front() {

}

void auto_blue_back() {

}

void auto_blue_back_no_second_shot() {

}

void programming_skills() {

}

void driver_skills() {

}
