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
void programming_skills_28_points();
void programming_skills_30_points();
void driver_skills();

void autonomous() {
  // Auto Start
  printf("Started Auto!\n");
  uint32_t autonomous_time = pros::millis();
  Subsystem::enable_all();
  auto_update_task.start_task();
  drive.reset_global_angle();

  drive_move_async(38_in, 0_deg);
  capper.move_to_velocity(Capper::CAP_FLIP_POSITION + 12 * Capper::GEAR_RATIO, 120);
  drive.wait_for_distance(15_in);
  angler.move_to(Angler::PICKUP_POSITION);
  drive.wait_for_distance(36_in);
  intake.intake();
  drive_move_sync(9_in, 0_deg, true, 50);
  drive_move_sync(-27_in, 0_deg);
  drive_turn_async(FixedAngleTarget(-50_deg));
  drive.wait_for_angle(-30_deg);
  double_shot(front_SP.top, front_SP.mid);
  drive.wait_for_stop();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(-70.0_deg));
  capper.move_to_flag_flip(180);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_move_sync(13_in, -70.0_deg);
  drive_move_async(-9_in, -70.0_deg);
  drive.wait_for_distance(-6);
  intake.stop();
  angler.move_to(Angler::PICKUP_POSITION);
  capper.move_to_pickup();
  drive_move_sync(6_in, -70.0_deg);
  drive.wait_for_distance(4.0_in);
  capper.pickup_cap();
  drive.wait_for_stop();
  pros::delay(200);
  drive_move_sync(-30_in, -70.0_deg);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  drive_move_sync(-49.0_in, -90_deg, true, 200, -90);
  drive_turn_sync(FixedAngleTarget(0_deg));
  cap_on_pole();

  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_sync(26_in, 0_deg);
  drive_turn_sync(FixedAngleTarget(28_deg));
  capper.move_to_velocity(47.5 * Capper::GEAR_RATIO, 180);
  drive_move_sync(15_in, 28_deg);
  drive_move_async(-12_in, 28_deg);
  drive.wait_for_distance(-8_in);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.stop();
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(10_in, 28_deg);
  drive.wait_for_distance(8_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  drive_move_sync(-17_in, 28_deg);
  pros::delay(250);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  intake.intake();
  drive_move_sync(-6_in, -90_deg, false, 200, -30, false);
  cap_on_pole();
  angler.move_to(25);

  drive_move_sync(5_in, 0_deg);
  drive_turn_async(FixedAngleTarget(-7.8_deg));
  double_shot(front_SP.top, front_SP.mid + 20);
  drive.wait_for_stop();
  capper.move_to_velocity(47.5 * Capper::GEAR_RATIO, 180);
  drive_turn_sync(FixedAngleTarget(91.5_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_sync(39_in, 93.5_deg);

  drive_move_async(-11_in, 93.5_deg);
  drive.wait_for_distance(-8_in);
  angler.move_to(20);
  intake.stop();
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(11_in, 92.5_deg);
  drive.wait_for_distance(9_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  intake.intake();

  drive_turn_sync(FixedAngleTarget(75_deg));
  drive_move_sync(31_in, 75_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  cap_on_pole();

  drive_move_sync(5_in,0_deg);
  angler.move_to(front_SP.top - 15);
  drive_turn_async(FixedAngleTarget(4.5_deg));
  double_shot(front_SP.top - 15, 0);
  drive.wait_for_stop();
  drive_turn_sync(FixedAngleTarget(0_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_async(24.5_in, 0_deg);
  drive.wait_for_distance(7_in);
  capper.move_to_velocity(30 * Capper::GEAR_RATIO, 200);
  drive.wait_for_stop();
  drive_move_sync(-7_in,0_deg);
  //drive_move_sync(18.5_in,4.5_deg);
//  angler.move_to(Angler::CAP_PICKUP_POSITION);

  drive_turn_sync(FixedAngleTarget(-88_deg));

  capper.move_to_velocity(56 * Capper::GEAR_RATIO, 200);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.intake();
  drive_move_async(15.5_in, -88_deg);
  drive.wait_for_distance(14.5_in);
  drive_move_async(-2.5_in, -88_deg, true, 80);
  capper.move_to_power(28 * Capper::GEAR_RATIO, -100);
  drive.wait_for_stop();
  pros::delay(50);
  drive_move_sync(-5_in, -88_deg);
  intake.stop();
  capper.move_to_pickup();
  drive_move_async(9.5_in, -88_deg);
  drive.wait_for_distance(5.0_in);
  capper.pickup_cap();
  pros::delay(250);
  drive.wait_for_stop();
  intake.intake();
  drive_turn_sync(FixedAngleTarget(-79_deg));
  drive_move_sync(-33_in, -82_deg, false);
  cap_on_pole();
  drive_move_sync(38_in,0_deg);
  drive_turn_sync(FixedAngleTarget(55_deg));
  single_shot(30);
  drive_turn_sync(FixedAngleTarget(101.5_deg));
  single_shot(front_SP.mid);
  drive_turn_sync(FixedAngleTarget(90_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_move_sync(15_in, 90_deg);
  angler.move_to(Angler::PICKUP_POSITION);
  capper.move_to_flag_flip();
  climb_on_platform();
  drive_turn_sync(FixedAngleTarget(-90_deg));
  climb_on_platform();

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

void programming_skills_28_points() {
  drive_move_async(38_in, 0_deg);
  capper.move_to_velocity(Capper::CAP_FLIP_POSITION + 5 * Capper::GEAR_RATIO, 120);
  drive.wait_for_distance(15_in);
  angler.move_to(Angler::PICKUP_POSITION);
  drive.wait_for_distance(36_in);
  intake.intake();
  drive_move_sync(9_in, 0_deg, true, 50);
  drive_move_sync(-27_in, 0_deg);
  drive_turn_async(FixedAngleTarget(-50_deg));
  drive.wait_for_angle(-30_deg);
  double_shot(front_SP.top, front_SP.mid);
  drive.wait_for_stop();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(-70.0_deg));
  capper.move_to_flag_flip(180);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_move_sync(15_in, -70.0_deg);
  drive_move_async(-9_in, -70.0_deg);
  drive.wait_for_distance(-6);
  intake.stop();
  angler.move_to(25);
  capper.move_to_pickup();
  drive_move_sync(6_in, -70.0_deg);
  drive.wait_for_distance(4.0_in);
  capper.pickup_cap();
  drive.wait_for_stop();
  pros::delay(200);
  drive_move_sync(-30_in, -70.0_deg);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  drive_move_sync(-48.0_in, -90_deg, true, 200, -90);
  drive_turn_sync(FixedAngleTarget(0_deg));
  cap_on_pole();

  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_sync(26_in, 0_deg);
  drive_turn_sync(FixedAngleTarget(28_deg));
  capper.move_to_velocity(47.5 * Capper::GEAR_RATIO, 180);
  drive_move_sync(15_in, 28_deg);
  drive_move_async(-12_in, 28_deg);
  drive.wait_for_distance(-8_in);
  angler.move_to(20);
  intake.stop();
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(10_in, 28_deg);
  drive.wait_for_distance(8_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  drive_move_sync(-17_in, 28_deg);
  pros::delay(250);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  intake.intake();
  drive_move_sync(-6_in, -90_deg, false, 200, -30, false);
  cap_on_pole();
  angler.move_to(25);

  drive_move_sync(5_in, 0_deg);
  drive_turn_async(FixedAngleTarget(-7.8_deg));
  double_shot(front_SP.top, front_SP.mid + 20);
  drive.wait_for_stop();
  capper.move_to_velocity(47.5 * Capper::GEAR_RATIO, 180);
  drive_turn_sync(FixedAngleTarget(93.5_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_sync(39_in, 93.5_deg);

  drive_move_async(-11_in, 93.5_deg);
  drive.wait_for_distance(-8_in);
  angler.move_to(20);
  intake.stop();
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(11_in, 93.5_deg);
  drive.wait_for_distance(9_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  intake.intake();

  drive_turn_sync(FixedAngleTarget(75_deg));
  drive_move_sync(31_in, 75_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  cap_on_pole();

  angler.move_to(front_SP.top - 15);
  drive_move_async(23.5_in, 0_deg);
  drive.wait_for_distance(5_in);
  capper.move_to_velocity(25 * Capper::GEAR_RATIO, 200);
  drive.wait_for_stop();
  drive_turn_async(FixedAngleTarget(4.5_deg));
  double_shot(front_SP.top - 15, front_SP.mid + 5);
  drive.wait_for_stop();
  drive_turn_sync(FixedAngleTarget(-88_deg));

  capper.move_to_velocity(56 * Capper::GEAR_RATIO, 200);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.intake();
  drive_move_async(15.5_in, -88_deg);
  drive.wait_for_distance(14.5_in);
  drive_move_async(-2.5_in, -88_deg, true, 80);
  capper.move_to_power(28 * Capper::GEAR_RATIO, -100);
  drive.wait_for_stop();
  pros::delay(50);
  drive_move_sync(-5_in, -88_deg);
  intake.stop();
  capper.move_to_pickup();
  drive_move_async(9.5_in, -88_deg);
  drive.wait_for_distance(5.0_in);
  capper.pickup_cap();
  pros::delay(250);
  drive.wait_for_stop();
  intake.intake();
  drive_turn_sync(FixedAngleTarget(-76_deg));
  drive_move_sync(-33_in, -76_deg, false);
}

void programming_skills_30_points() {
  capper.move_to_flag_flip();
  single_shot(front_SP.top);
  drive_turn_sync(FixedAngleTarget(37_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_async(23.5_in, 37_deg);
  drive.wait_for_distance(21.5_in);
  drive_move_async(-13.5_in, 37_deg);
  drive.wait_for_distance(-4_in);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.stop();
  drive.wait_for_distance(-12_in);
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(14_in, 37_deg);
  drive.wait_for_distance(12_in);
  capper.pickup_cap();
  drive.wait_for_stop();
  intake.intake();
  drive_move_async(-24_in, 37_deg);
  drive.wait_for_distance(-4_in);
  capper.move_to_cap_flip(true, 50);
  drive.wait_for_stop();
  drive_turn_async(FixedAngleTarget(4_deg));
  drive.wait_for_angle(22_deg);
  single_shot(front_SP.mid);
  drive_turn_sync(FixedAngleTarget(86_deg));
  angler.move_to(Angler::PICKUP_POSITION);

  drive_move_async(36_in, 86_deg, false, 80);
  drive.wait_for_distance(25.5_in);
  capper.move_to_flag_flip(35);
  drive.wait_for_stop();
  drive_turn_async(FixedAngleTarget(12_deg));
  intake.intake();
  drive.wait_for_angle(18_deg);
  single_shot(front_SP.top);
  drive_move_async(33_in,12_deg);
  capper.move_to_flag_flip();
  drive.wait_for_distance(1.5_in);
  single_shot(front_SP.top);
  drive.wait_for_stop();
  drive_move_sync(-10_in,10.5_deg);
  drive_turn_sync(FixedAngleTarget(50_deg));
  drive_move_sync(-54_in,50_deg);
  drive_turn_sync(FixedAngleTarget(-6_deg));
  drive_move_sync(-52_in,-6_deg);
  drive_turn_sync(FixedAngleTarget(84_deg));

  cap_on_pole();
  drive_move_async(6_in, 0_deg, true, 115);
  angler.move_to(Angler::PICKUP_POSITION);
  drive.wait_for_distance(2_in);
  capper.move_to_cap_flip(false);
  drive.wait_for_stop();
  drive_turn_sync(FixedAngleTarget(-15_deg));
  intake.intake();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_move_async(27_in,-15_deg, false);
  drive.wait_for_distance(25.0_in);
  drive_move_async(13.5_in, -15_deg, true, 45);
  drive.wait_for_distance(13_in);
  drive.wait_for_stop();


  drive_move_sync(-13.5_in,-15_deg);
  capper.pickup_cap();
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(45_deg));
  drive_move_sync(17_in,45_deg);
  drive_move_sync(-15_in,45_deg);
  intake.stop();
  angler.move_to(Angler::PICKUP_POSITION);
  capper.move_to_pickup();
  drive_move_async(12_in,45_deg);
  drive.wait_for_distance(8_in);
  capper.pickup_cap();
  drive.wait_for_stop();
  drive_move_async(-6_in, 45_deg);
  drive.wait_for_distance(-5.5_in);
  sweep_turn(FixedAngleTarget(-90_deg), -4_in, false, 0_in, false);
  cap_on_pole();
  drive_move_sync(12_in, 0_deg);
  capper.move_to_pickup();
  drive_turn_sync(FixedAngleTarget(103_deg));
  drive_move_async(38_in, 103_deg);
  drive.wait_for_distance(32_in);
  capper.pickup_cap();
  drive_turn_sync(FixedAngleTarget(72_deg));
  drive_move_sync(42.0_in, 74_deg);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  drive_move_sync(-9.0_in, -90_deg);

  cap_on_pole();

  drive_move_async(8_in, 0_deg);
  drive.wait_for_distance(3.0_in);
  capper.move_to_cap_flip(true, 200);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.intake();
  drive.wait_for_stop();
  drive_turn_async(FixedAngleTarget(87.0_deg));
  drive.wait_for_angle(65_deg);
  double_shot(front_SP.top + 10, front_SP.mid);
  capper.pickup_cap();
  drive_turn_sync(FixedAngleTarget(16.5_deg));

  drive_move_async(33.5_in, 16.5_deg);
  capper.move_to_flag_flip(150);
  angler.move_to(Angler::PICKUP_POSITION);
  drive.wait_for_distance(24_in);
  capper.move_to_velocity(36 * Capper::GEAR_RATIO, 200);
  drive.wait_for_distance(32.0);
  capper.move_to_power(28 * Capper::GEAR_RATIO, -120);
  drive_move_sync(-6.0_in, 16.5_deg, true, 40);
  intake.stop();
  capper.move_to_pickup();
  pros::delay(500);
  drive_move_async(10.0_in, 16.5_deg);
  drive.wait_for_distance(6.0_in);
  capper.pickup_cap();
  pros::delay(200);
  intake.intake();
  drive.wait_for_stop();
  pros::delay(400);
  drive_move_sync(-22.0_in, 16.5_deg);
  drive_turn_sync(FixedAngleTarget(90_deg));
  intake.stop();
  drive_move_sync(-14_in, 90_deg, false);
  cap_on_pole();
  drive_move_sync(6_in, 0_deg);
  drive_turn_sync(FixedAngleTarget(-36.0_deg));
  // drive.wait_for_angle(-15_deg);
  single_shot(70);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_move_sync(21.5_in, -38.5_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  drive_move_sync(7.25_in, 0_deg, false);

  climb_on_platform();
  drive_turn_sync(FixedAngleTarget(-90_deg));
  climb_on_platform();
}

void driver_skills() {

}
