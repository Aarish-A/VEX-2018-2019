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
void auto_red_front_park();
void auto_red_front();
void auto_red_back();
void auto_red_back_park();
void auto_red_back_no_second_shot();
void auto_blue_front();
void auto_blue_front_park();
void auto_blue_back();
void auto_blue_back_no_second_shot();
void programming_skills_29_points();
void programming_skills_30_points();
void driver_skills();

vector flags_blue[] = {{17,11}, {17,11}, {66.5,11}, {66.5,11}, {114,11}, {114,11}};


void autonomous() {
  // Auto Start
  printf("%d Started Auto!\n", pros::millis());
  uint32_t autonomous_time = pros::millis();
  Subsystem::enable_all();
  auto_update_task.start_task();
  drive.reset_global_angle();
  angler.move_to(Angler::PICKUP_POSITION);
  puncher.reset();
  // drive_turn_sync(53.0_deg);
  if(game_side == 'R')
  {
    flags_blue[0] = {17,11};
    flags_blue[1] = {17,11};
    flags_blue[2] = {66.5,11};
    flags_blue[3] = {66.5,11};
    flags_blue[4] = {114,11};
    flags_blue[5] = {114,11};
  }
  if(game_side == 'B')
  {
    flags_blue[0] = {114,11};
    flags_blue[1] = {114,11};
    flags_blue[2] = {66.5,11};
    flags_blue[3] = {66.5,11};
    flags_blue[4] = {17,11};
    flags_blue[5] = {17,11};
  }

  // angler.move_to(Angler::CAP_PICKUP_POSITION);
  // intake.intake();
  // pros::delay(500);
  // drive_move_sync(40_in, 0_deg);

  switch(auto_routine) {
    case Auto_Routines::FRONT:
      if (game_side == 'R') auto_red_front();
      if (game_side == 'B') auto_blue_front();
      break;
    case Auto_Routines::BACK:
      if (game_side == 'R') auto_red_back();
      if (game_side == 'B') auto_blue_back();
      break;
    case Auto_Routines::FRONT_PARK:
      if (game_side == 'R') auto_red_front_park();
      if (game_side == 'B') auto_blue_front_park();
      break;
    case Auto_Routines::PROGRAMMING_SKILLS:
      programming_skills_30_points();
      // if (game_side == 'R') programming_skills_29_points();
      // if (game_side == 'B') programming_skills_30_points();
      break;
    case Auto_Routines::DRIVER_SKILLS:
      puncher.reset();
      drive_move_async(37_in, 0_deg);
      capper.move_to_velocity(35 * Capper::GEAR_RATIO, 120);
      drive.wait_for_distance(12_in);
      angler.move_to(Angler::PICKUP_POSITION);
      drive.wait_for_distance(27_in);
      intake.intake();
      drive.wait_for_distance(31_in);
      drive_move_sync(14.5_in, 0_deg, true, 140);
      drive_move_sync(-40_in, 0_deg);
      drive_turn_async(FixedAngleTarget(-86.5_deg));
      drive.wait_for_angle(-59_deg);
      double_shot(shot_positions[G_FRONT_TOP],shot_positions[G_FRONT_MID]);
      capper.move_to_velocity(75 * Capper::GEAR_RATIO, 200);
      master.rumble("-");
      break;
    case Auto_Routines::NUM_OF_ELEMENTS:
      break;
  }


  // drive_move_async(43_in, 0_deg);
  // capper.move_to_velocity(34 * Capper::GEAR_RATIO, 120);
  // angler.move_to(Angler::PICKUP_POSITION);
  // intake.intake();
  // drive.wait_for_stop();
  // drive_move_sync(-40.0_in,0_deg);
  // drive_turn_async(FixedAngleTarget(-84.5_deg));
  // double_shot(front_SP.top,front_SP.mid);
  // capper.move_to_velocity(75 * Capper::GEAR_RATIO, 200);
  // master.rumble("-");

  // Auto End
  uint32_t auto_finished_time = pros::millis() - autonomous_time;
  master.write_line(2, "Time: %d", auto_finished_time);
  log_ln(AUTO,"Auto Time %d", auto_finished_time);
  if (auto_routine!=Auto_Routines::DRIVER_SKILLS) pros::delay(1000);
  /* Turn off Motors */
  intake.off();
  drive.set_power(0);
  // Kill all tasks
  pilons::Task::stop_all_tasks();
}

void auto_red_front_park()
{
  pos.reset(57,10,0);
  intake.intake();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_move_async(43_in, 0_deg);
  drive.wait_for_distance(40_in);
  drive_move_sync(-36_in,0_deg,true);
  angler.move_to(shot_positions[(int)SP::G_FRONT_TOP]);
  drive_turn_async(FixedAngleTarget(-81_deg)); //-81
  drive.wait_for_angle(-20_deg);
  double_shot(shot_positions[(int)SP::G_FRONT_TOP], shot_positions[(int)SP::G_FRONT_MID]);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(30_deg));
  drive_move_async(23_in,30_deg);
  drive.wait_for_distance(21_in);
  drive_move_sync(-20_in,30_deg);
  drive_turn_sync(FixedAngleTarget(90_deg));
  drive_move_sync(23.5_in,90_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  drive_move_sync(14_in,0_deg,false);
  climb_on_platform();
  drive.flatten_against_wall(true, true);
  drive.reset_global_angle();
  drive.set_power(55, 15, 0);
  while (right_platform_sensor.get_value() < 2000) pros::delay(5);
  drive.set_power(-55, 15,0);
  while (right_platform_sensor.get_value() > 1900) pros::delay(5);
  drive.set_power(0, 0, 0);
  drive_turn_sync(FixedAngleTarget(-47_deg));
  single_shot(shot_positions[(int)SP::G_FRONT_TOP]);
}
void auto_red_front() {
  intake.intake();
  angler.move_to(Angler::CAP_FLIP_POSITION);
  drive_move_async(43_in, 0_deg);
  drive.wait_for_distance(42.5_in);
  drive_move_sync(-36_in,0_deg,true);
  angler.move_to(shot_positions[(int)SP::G_FRONT_TOP]);
  for(int i = 0; i <= 1; i++)
  {
    switch(flag_config[i]) {
      case Flags::LEFT_TOP:
      drive_turn_async(FixedAngleTarget(-86_deg)); //-81
      if(i==0)drive.wait_for_angle(-60_deg);
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::LEFT_MID:
      drive_turn_async(FixedAngleTarget(-86_deg)); //-81
      if(i==0)drive.wait_for_angle(-40_deg);
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_TOP:
      drive_turn_sync(FixedAngleTarget(-43.5_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_MID:
      drive_turn_sync(FixedAngleTarget(-43.5_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      default:
      break;
    }
  }
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(30_deg));
  drive_move_sync(21.5_in,30_deg);
  pros::delay(100);
  drive_move_sync(-7.5_in,30_deg);
  drive_turn_sync(FixedAngleTarget(-68_deg));
  drive_move_sync(20_in,-68_deg);
  printf("%d Picked ball off cap \n", pros::millis());

  //Flip
  pros::delay(100);
  drive_move_sync(-9_in, -68_deg);
  angler.move_to(Angler::CAP_FLIP_POSITION+10);
  pros::delay(500);
  printf("%d Done ball raise wait \n", pros::millis());
  intake.outtake();
  drive_move_sync(15_in, -68_deg);
  double top_flag_pos = shot_positions[(int)SP::G_FRONT_TOP]+20;
  angler.move_to(top_flag_pos+20);
  drive_move_sync(-17_in,-68_deg);

  for(int i = 2; i <= 3; i++)
  {
    switch(flag_config[i]) {
      case Flags::LEFT_TOP:
      if(flag_config[i-1]!=Flags::LEFT_MID)drive_move_sync(-20_in);
      drive_turn_sync(FixedAngleTarget(-102_deg)); //-81
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::LEFT_MID:
      if(flag_config[i-1]!=Flags::LEFT_TOP)drive_move_sync(-20_in);
      drive_turn_sync(FixedAngleTarget(-102_deg)); //-81
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_TOP:
      drive_turn_sync(FixedAngleTarget(-55_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_MID:
      drive_turn_sync(FixedAngleTarget(-55_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      default:
      break;
    }
  }
  //Turn and shoot
  // drive_turn_sync(PointAngleTarget(flags_front_red[1]));
  // double_shot(top_flag_pos, shot_positions[(int)SP::G_FRONT_MID]+25);

}

void auto_red_back() {
  int cap_dis = 47_in;
    //Pickup
    intake.intake();
    angler.move_to(Angler::CAP_FLIP_POSITION);
    drive_move_sync(cap_dis, 0_deg);

    //Shoot
    drive_move_sync(-(cap_dis-14_in), 0_deg);

    vector pos = {23, 104.5};
    for(int i = 0; i <= 1; i++) {
      if(flag_config[i] != Flags::NO_FLAG)
      {
        drive_turn_sync(FixedAngleTarget(flag_angle(flags_blue[(int)flag_config[i]], pos))); //-81
        single_shot(shot_positions[A_SHOT_1 + i]);
      }
    }


    //Flatten wall
    drive_turn_sync(FixedAngleTarget(-90_deg));
    drive_move_sync(-20_in, -90_deg,false);
    printf(" \n\n >>>> %d auto done back up | %d %d \n", pros::millis(), enc_l.get_value(), enc_r.get_value());
    drive.flatten_against_wall(false, true);
    pros::delay(100);
    drive.reset_global_angle();
    printf("%d auto RESET: %d %d\n", pros::millis(), enc_l.get_value(), enc_r.get_value());

    //Get balls off back cap
    double mid_flag_pos = 140, top_flag_pos = 215;
    drive_move_sync(6_in, 0_deg);
    angler.move_to(Angler::CAP_PICKUP_POSITION);
    drive_turn_sync(FixedAngleTarget(90_deg));
    drive_move_sync(23_in, 90_deg);
    drive_move_sync(3.5_in, 90_deg, true, 80);
    pros::delay(100);
    drive_move_sync(-7_in, 90_deg);
    pros::delay(700);


    pos = {45.5,129.5};
    for(int i = 2; i <= 3; i++) {
      if(flag_config[i] != Flags::NO_FLAG)
      {
        drive_turn_sync(FixedAngleTarget(flag_angle(flags_blue[(int)flag_config[i]], pos, 90))); //-81
        single_shot(shot_positions[A_SHOT_1 + i]);
      }
    }
    printf("%d Stopped | red\n", pros::millis());

}

void auto_red_back_park() {
  int cap_dis = 47_in;
  //Pickup
  intake.intake();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_move_sync(cap_dis, 0_deg);

  //Shoot
  drive_move_sync(-(cap_dis-14_in), 0_deg);
  intake.off();
  drive_turn_sync(FixedAngleTarget(-65.5_deg)); //IF
  single_shot(30);

  //Get ball of PF
  intake.intake();
  drive_move_sync(10_in, -65_deg);
  drive_move_sync(-7_in, -65_deg);

  //Turn  and get ball off of capper

  //Back up and get on PF




}

void auto_red_back_no_second_shot() {
  cap_on_pole();
  intake.stop();
  capper.move_to_pickup();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_move_sync(26_in, 0_deg);
  drive_turn_sync(FixedAngleTarget(28_deg));
  angler.move_to(Angler::PICKUP_POSITION);
  capper.wait_for_target_reach();
  drive_move_async(17_in, 28_deg);
  drive.wait_for_distance(8_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  drive_move_sync(-20_in, 28_deg);
  pros::delay(250);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  intake.intake();
  drive_move_sync(-6_in, -90_deg, false, 200, -30, false);
  cap_on_pole();
  angler.move_to(25);
  drive_move_async(5_in);
  drive.wait_for_distance(4_in);
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_turn_sync(FixedAngleTarget(91.5_deg));
  angler.move_to(20);
  intake.stop();
  drive_move_async(45_in, 91.5_deg);
  drive.wait_for_distance(38_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  intake.intake();

  drive_turn_sync(FixedAngleTarget(75_deg));
  drive_move_sync(24_in, 75_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  cap_on_pole();

  drive_move_sync(5_in,0_deg);
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
  angler.move_to(Angler::CAP_FLIP_POSITION);
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
}

void auto_blue_front() {
  intake.intake();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_move_async(43_in, 0_deg);
  drive.wait_for_distance(42.5_in);
  drive_move_sync(-36_in,0_deg,true);
  angler.move_to(shot_positions[(int)SP::G_FRONT_TOP]);
  for(int i = 0; i <= 1; i++)
  {
    switch(flag_config[i]) {
      case Flags::RIGHT_TOP:
      drive_turn_async(FixedAngleTarget(84_deg)); //-81
      if(i==0)drive.wait_for_angle(60_deg);
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::RIGHT_MID:
      drive_turn_async(FixedAngleTarget(84_deg)); //-81
      if(i==0)drive.wait_for_angle(60_deg);
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_TOP:
      drive_turn_sync(FixedAngleTarget(42_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_MID:
      drive_turn_sync(FixedAngleTarget(42_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      default:
      break;
    }
  }
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(-30_deg));
  drive_move_sync(21.5_in,-30_deg);
  pros::delay(100);
  drive_move_sync(-7.5_in,-30_deg);
  drive_turn_sync(FixedAngleTarget(68_deg));
  drive_move_sync(21.5_in,68_deg);
  printf("%d Picked ball off cap \n", pros::millis());

  //Flip
  pros::delay(100);
  drive_move_sync(-9_in, 68_deg);
  angler.move_to(Angler::CAP_FLIP_POSITION+10);
  pros::delay(500);
  printf("%d Done ball raise wait \n", pros::millis());
  intake.outtake();
  drive_move_sync(18_in, 68_deg);
  double top_flag_pos = shot_positions[(int)SP::G_FRONT_TOP]+20;
  angler.move_to(top_flag_pos+20);

  for(int i = 2; i <= 3; i++)
  {
    switch(flag_config[i]) {
      case Flags::LEFT_TOP:
      if(flag_config[i-1]!=Flags::LEFT_MID)drive_move_sync(-20_in);
      drive_turn_sync(FixedAngleTarget(102_deg)); //-81
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::LEFT_MID:
      if(flag_config[i-1]!=Flags::LEFT_TOP)drive_move_sync(-20_in);
      drive_turn_sync(FixedAngleTarget(102_deg)); //-81
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_TOP:
      drive_turn_sync(FixedAngleTarget(40_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      case Flags::MID_MID:
      drive_turn_sync(FixedAngleTarget(40_deg));
      single_shot(shot_positions[A_SHOT_1 + i]);
      break;
      default:
      break;
    }
  }
  //Turn and shoot
  // drive_turn_sync(PointAngleTarget(flags_front_red[1]));
  // double_shot(top_flag_pos, shot_positions[(int)SP::G_FRONT_MID]+25);

}
void auto_blue_front_park() {
  intake.intake();
  angler.move_to(Angler::PICKUP_POSITION);
  drive_move_async(43_in, 0_deg);
  drive.wait_for_distance(40_in);
  drive_move_sync(-36_in,0_deg,true);
  angler.move_to(front_SP.top);
  drive_turn_async(FixedAngleTarget(83_deg));
  drive.wait_for_angle(25_deg);
  double_shot(front_SP.top, front_SP.mid);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(-30_deg));
  drive_move_async(23_in,-30_deg);
  drive.wait_for_distance(21_in);
  drive_move_sync(-20_in,-30_deg);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  drive_move_sync(24_in,-90_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  drive_move_sync(16.5_in,0_deg,false);
  climb_on_platform();
  drive.flatten_against_wall(true, true);
  drive.reset_global_angle();
  drive_turn_sync(FixedAngleTarget(53_deg));
  single_shot(front_SP.top-50);
}
void auto_blue_back() {

  int cap_dis = 47_in;
    //Pickup
    intake.intake();
    angler.move_to(Angler::CAP_FLIP_POSITION);
    drive_move_sync(cap_dis, 0_deg);

    //Shoot
    drive_move_sync(-(cap_dis-14_in), 0_deg);

    vector pos = {23, 104.5};
    for(int i = 0; i <= 1; i++) {
      drive_turn_sync(FixedAngleTarget(flag_angle_blue(flags_blue[(int)flag_config[i]], pos))); //-81
      single_shot(shot_positions[A_SHOT_1 + i]);
    }


    //Flatten wall
    drive_turn_sync(FixedAngleTarget(90_deg));
    drive_move_sync(-20_in, 90_deg,false);
    printf(" \n\n >>>> %d auto done back up | %d %d \n", pros::millis(), enc_l.get_value(), enc_r.get_value());
    drive.flatten_against_wall(false,true);
    drive.reset_global_angle();
    printf("%d auto RESET: %d %d\n", pros::millis(), enc_l.get_value(), enc_r.get_value());

    //Get balls off back cap
    double mid_flag_pos = 140, top_flag_pos = 215;
    drive_move_sync(6_in, 0_deg);
    angler.move_to(Angler::CAP_PICKUP_POSITION);
    drive_turn_sync(FixedAngleTarget(-90_deg));
    drive_move_sync(26_in, -90_deg);
    pros::delay(200);
    drive_move_sync(-7_in, -90_deg);
    pros::delay(700);

    pos = {45.5,129.5};
    for(int i = 2; i <= 3; i++) {
      drive_turn_sync(FixedAngleTarget(flag_angle_blue(flags_blue[(int)flag_config[i]], pos, 90))); //-81
      single_shot(shot_positions[A_SHOT_1 + i]);
    }

    printf("%d Stopped | red\n", pros::millis());

}

void auto_blue_back_no_second_shot() {

}

void programming_skills_29_points() {
  drive_move_async(38_in, 0_deg);
  capper.move_to_velocity(32 * Capper::GEAR_RATIO, 120);
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
  drive_move_sync(14_in, -70.0_deg);
  drive_move_async(-10_in, -70.0_deg);
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
}

void programming_skills_28_points() {
  drive_move_async(47_in, 0_deg);
  capper.move_to_velocity(Capper::CAP_FLIP_POSITION * Capper::GEAR_RATIO, 120);
  drive.wait_for_distance(15_in);
  angler.move_to(Angler::PICKUP_POSITION);
  drive.wait_for_distance(36_in);
  intake.intake();
  drive.wait_for_stop();
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
  angler.move_to(Angler::PICKUP_POSITION);
  puncher.reset();
  drive_move_async(37_in, 0_deg);
  capper.move_to_velocity(35 * Capper::GEAR_RATIO, 120);
  drive.wait_for_distance(12_in);
  angler.move_to(Angler::PICKUP_POSITION);
  drive.wait_for_distance(27_in);
  intake.intake();
  drive.wait_for_distance(31_in);
  drive_move_sync(14.5_in, 0_deg, true, 140);
  drive_move_sync(-27.5_in, 0_deg);
  drive_turn_async(FixedAngleTarget(-52.5_deg));
  // drive.wait_for_angle(-30_deg);
  drive.wait_for_angle(-30_deg);
  double_shot(shot_positions[S_FRONT_MID], shot_positions[S_FRONT_TOP]);
  drive.wait_for_stop();
  angler.move_to(Angler::PICKUP_POSITION);
  capper.move_to_flag_flip(180);
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_turn_sync(FixedAngleTarget(-66.0_deg));
  drive_move_sync(15.5_in, -66.0_deg);
  drive_move_async(-13_in, -66.0_deg);
  drive.wait_for_distance(-9_in);
  intake.stop();
  angler.move_to(Angler::PICKUP_POSITION);
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(12_in, -66.0_deg);
  drive.wait_for_distance(6.0_in);
  capper.pickup_cap();
  drive.wait_for_stop();
  drive_move_sync(-22_in, -66_deg, false);
  drive_turn_sync(-90_deg);
  drive_move_sync(-54.0_in, -90_deg, true, 200, 0, true, true);

  drive_turn_sync(0_deg);
  drive_move_async(-8, 0_deg, false);
  drive.wait_for_distance(-5_in);
  drive_move_task.stop_task();
  cap_on_pole();
//
//
//  // yeyaee
//
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_sync(26_in, 0_deg);
  drive_turn_sync(FixedAngleTarget(27_deg));
  capper.move_to_velocity(47.5 * Capper::GEAR_RATIO, 180);
  drive_move_sync(15_in, 27_deg);
  drive_move_async(-12_in, 27_deg);
  drive.wait_for_distance(-8_in);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.stop();
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(10_in, 27_deg);
  drive.wait_for_distance(8_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  drive_move_sync(-17_in, 27_deg);
  drive_turn_sync(FixedAngleTarget(-90_deg));
  intake.intake();
  drive_move_sync(-6_in, -90_deg, false, 200, -30, false);
  cap_on_pole();
  angler.move_to(shot_positions[S_BACK_MID]);

  drive_move_sync(5_in, 0_deg);
  drive_turn_sync(FixedAngleTarget(-7.5_deg));
  double_shot(shot_positions[S_BACK_MID], shot_positions[S_BACK_TOP]);
  drive.wait_for_stop();

  drive_turn_sync(FixedAngleTarget(100_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  capper.move_to_velocity(47.5 * Capper::GEAR_RATIO, 180);
  drive_move_sync(20_in, 100_deg);
  drive_turn_sync(FixedAngleTarget(90_deg));
  drive_move_sync(17.3_in, 90_deg);
  drive_move_async(-14_in, 90_deg);
  drive.wait_for_distance(-9_in);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.stop();
  capper.move_to_pickup();
  drive.wait_for_stop();
  drive_move_async(14_in, 90_deg);
  drive.wait_for_distance(12_in);
  capper.pickup_cap(true);
  drive.wait_for_stop();
  intake.intake();

  drive_turn_sync(FixedAngleTarget(75_deg));
  drive_move_sync(31_in, 75_deg);
  drive_turn_sync(FixedAngleTarget(0_deg));
  cap_on_pole();
//
  drive_move_async(5_in,0_deg);
  angler.move_to(shot_positions[S_BACK_TOP]);
  drive.wait_for_distance(3.5_in);
  single_shot(shot_positions[S_BACK_TOP],false);
  drive_turn_async(FixedAngleTarget(4.0_deg));
  drive.wait_for_stop();
  while(puncher.shooting()) pros::delay(2);
  single_shot(shot_positions[S_BACK_BOT]);
  drive_turn_sync(FixedAngleTarget(0_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  intake.intake();
  drive_move_async(24.8_in, 0_deg);
  drive.wait_for_distance(7_in);
  capper.move_to_velocity(30 * Capper::GEAR_RATIO, 200);
  drive.wait_for_stop();
  drive_move_sync(-7_in,0_deg);
  //drive_move_sync(18.5_in,4.5_deg);
//  angler.move_to(Angler::CAP_PICKUP_POSITION);

  drive_turn_sync(FixedAngleTarget(-88_deg));

  capper.move_to_velocity(56 * Capper::GEAR_RATIO, 200);
  angler.move_to(37);
  intake.intake();
  drive_move_sync(15.5_in, -88_deg);
  drive_move_sync(-5.5_in, -88_deg, true, 100);
  pros::delay(200);
  capper.move_to_power(30 * Capper::GEAR_RATIO, -100);
  pros::delay(150);
  // drive.wait_for_stop();
  drive_move_sync(-5_in, -88_deg);
  intake.stop();
  capper.move_to_pickup();
  drive_move_async(9.5_in, -88_deg);
  drive.wait_for_distance(5.0_in);
  capper.pickup_cap();
  pros::delay(250);
  drive.wait_for_stop();
  intake.intake();
  drive_turn_sync(FixedAngleTarget(-75_deg));
  drive_move_sync(-33_in, -75_deg, false);
  cap_on_pole();
  drive_move_sync(38_in,0_deg);
  drive_turn_async(FixedAngleTarget(53.5_deg));
  drive.wait_for_angle(40_deg);
  single_shot(shot_positions[S_FINAL_LOW]);
  drive.wait_for_stop();
  drive_turn_async(FixedAngleTarget(100_deg));
  drive.wait_for_angle(80_deg);
  single_shot(shot_positions[S_FINAL_MID]);
  drive.wait_for_stop();
  drive_turn_sync(FixedAngleTarget(90_deg));
  angler.move_to(Angler::CAP_PICKUP_POSITION);
  drive_move_sync(15_in, 90_deg);
  angler.move_to(Angler::PICKUP_POSITION);
  capper.move_to_flag_flip();
  climb_on_platform();
  drive.wait_for_distance(5.5);
  printf("drive error:%f",drive.get_error());
  drive_move_task.stop_task();
  drive_turn_sync(FixedAngleTarget(-90_deg));
  climb_on_platform();
  drive.wait_for_distance(5.5);
  drive_move_task.stop_task();
}

void driver_skills() {

}
