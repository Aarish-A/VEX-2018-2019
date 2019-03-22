#include "main.h"
#include "config.hpp"
#include "macros/auto.hpp"
#include "macros/shot_queueing.hpp"

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


  // // Auto
  // capper.move_to_flag_flip();
  // single_shot(front_SP.top);
  // drive_turn_sync(FixedAngleTarget(37_deg));
  // angler.move_to(Angler::CAP_PICKUP_POSITION);
  // intake.intake();
  // drive_move_async(23.5_in, 37_deg);
  // drive.wait_for_distance(21.5_in);
  // drive_move_async(-13.5_in, 37_deg);
  // drive.wait_for_distance(-4_in);
  // angler.move_to(Angler::PICKUP_POSITION);
  // intake.stop();
  // drive.wait_for_distance(-12_in);
  // capper.move_to_pickup();
  // drive.wait_for_stop();
  // drive_move_async(14_in, 37_deg);
  // drive.wait_for_distance(12_in);
  // capper.pickup_cap();
  // drive.wait_for_stop();
  // intake.intake();
  // drive_move_async(-24_in, 37_deg);
  // drive.wait_for_distance(-4_in);
  // capper.move_to_cap_flip(true, 50);
  // drive.wait_for_stop();
  // drive_turn_async(FixedAngleTarget(4_deg));
  // drive.wait_for_angle(22_deg);
  // single_shot(front_SP.mid);
  // drive_turn_sync(FixedAngleTarget(86_deg));
  // angler.move_to(Angler::PICKUP_POSITION);
  //
  // drive_move_async(36_in, 86_deg, false, 80);
  // drive.wait_for_distance(25.5_in);
  // capper.move_to_flag_flip(35);
  // drive.wait_for_stop();
  // drive_turn_async(FixedAngleTarget(12_deg));
  // intake.intake();
  // drive.wait_for_angle(18_deg);
  // single_shot(front_SP.top);
  // drive_move_async(33_in,12_deg);
  // capper.move_to_flag_flip();
  // drive.wait_for_distance(1.5_in);
  // single_shot(front_SP.top);
  // drive.wait_for_stop();
  // drive_move_sync(-10_in,10.5_deg);
  // drive_turn_sync(FixedAngleTarget(50_deg));
  // drive_move_sync(-54_in,50_deg);
  // drive_turn_sync(FixedAngleTarget(-6_deg));
  // drive_move_sync(-52_in,-6_deg);
  // drive_turn_sync(FixedAngleTarget(84_deg));
  //
  // cap_on_pole();
  // drive_move_async(6_in, 0_deg, true, 115);
  // angler.move_to(Angler::PICKUP_POSITION);
  // drive.wait_for_distance(2_in);
  // capper.move_to_cap_flip(false);
  // drive.wait_for_stop();
  // drive_turn_sync(FixedAngleTarget(-15_deg));
  // intake.intake();
  // angler.move_to(Angler::PICKUP_POSITION);
  // drive_move_async(27_in,-15_deg, false);
  // drive.wait_for_distance(25.0_in);
  // drive_move_async(13.5_in, -15_deg, true, 45);
  // drive.wait_for_distance(13_in);
  // drive.wait_for_stop();
  //
  //
  // drive_move_sync(-13.5_in,-15_deg);
  // capper.pickup_cap();
  // angler.move_to(Angler::CAP_PICKUP_POSITION);
  // drive_turn_sync(FixedAngleTarget(45_deg));
  // drive_move_sync(17_in,45_deg);
  // drive_move_sync(-15_in,45_deg);
  // intake.stop();
  // angler.move_to(Angler::PICKUP_POSITION);
  // capper.move_to_pickup();
  // drive_move_async(12_in,45_deg);
  // drive.wait_for_distance(8_in);
  // capper.pickup_cap();
  // drive.wait_for_stop();
  // drive_move_async(-5_in, 45_deg);
  // drive.wait_for_distance(-5.5_in);
  // sweep_turn(FixedAngleTarget(-90_deg), -4_in, false, 0_in, false);
  // cap_on_pole();
  // drive_move_sync(12_in, 0_deg);
  // capper.move_to_pickup();
  // drive_turn_sync(FixedAngleTarget(103_deg));
  // drive_move_async(38_in, 103_deg);
  // drive.wait_for_distance(32_in);
  // capper.pickup_cap();
  // drive_turn_sync(FixedAngleTarget(72_deg));
  // drive_move_sync(42.0_in, 74_deg);
  // drive_turn_sync(FixedAngleTarget(-90_deg));
  // drive_move_sync(-9.0_in, -90_deg);
  cap_on_pole();

  drive_move_async(8_in, 0_deg);
  drive.wait_for_distance(3.0_in);
  capper.move_to_cap_flip(true, 200);
  intake.intake();
  drive.wait_for_stop();
  drive_turn_async(FixedAngleTarget(86.0_deg));
  drive.wait_for_angle(60_deg);
  double_shot(front_SP.top, front_SP.mid);
  capper.pickup_cap();
  drive_turn_sync(FixedAngleTarget(18_deg));
  drive_move_async(32.5_in);
  drive.wait_for_distance(31.0_in);
  drive_move_async(-23.5_in, 86_deg);
  drive.wait_for_stop();
  // drive.wait_for_distance(-12_in);
  // capper.move_to_pickup();
  // drive.wait_for_stop();
  // drive_move_async(14_in, 37_deg);
  // drive.wait_for_distance(12_in);
  // capper.pickup_cap();
  // drive.wait_for_stop();


  // move_drive_new(6_in, 0_deg);
  //   intake.move(127);
  //   going_up = 3;
  //   raise_cap();
  //   turn_vel_new(FixedAngleTarget(83_deg));
  //   going_up = 2;
  //   raise_cap();
  //   move_drive_new(11.5_in, 83_deg);
  //   auto_set_first_shot(front_SP.top+15);
  //   while (auto_set_shot) pros::delay(10);
  //   going_up = 3;
  //   raise_cap();
  //   auto_set_second_shot(front_SP.mid+15);
  //   while (auto_set_shot) pros::delay(10);
  //   turn_vel_new(FixedAngleTarget(0_deg));



  // Auto End
  uint32_t auto_finished_time = pros::millis() - autonomous_time;
  master.print(2, 0, "Time: %d", auto_finished_time);
  pros::delay(60);
  pilons::Task::stop_all_tasks();
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
