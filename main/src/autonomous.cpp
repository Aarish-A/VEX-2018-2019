#include "main.h"
#include "auto.hpp"
#include "config.hpp"
#include "puncher.hpp"
#include "shot_select.hpp"
#include "angler.hpp"
#include "drive.hpp"
#include "gui.hpp"

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
uint32_t autoStartTime;

void auto_red_front();
void auto_red_back();
void auto_blue_front();
void auto_blue_back();
void programming_skills();

void autonomous() {
  is_disabled = false;
  shot_req_handle_stop_task();
  auto_update_start_task();
  log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
  autoStartTime = millis();
  setDriveVel(0);
  delay(10);
  log_ln(LOG_AUTO, "   --- %d START AUTO --- \n", pros::millis());
  log_ln(LOG_AUTO, " >>> %d PUN TEMP: %f", pros::millis(), puncherLeft.get_temperature());
  pos.reset();
  resetGlobalAngle();
  log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  printf("%d RAISE ANGLER \n", pros::millis());

  if (game_side == 'R') {
    switch(menu_auto_route) {
      case auto_front: auto_red_front(); break;
      case auto_back: auto_red_back(); break;
      case auto_skills: programming_skills(); break;
      case number_of_auto_routes: break;
    }
  } else if (game_side == 'B') {
    switch(menu_auto_route) {
      case auto_front: auto_blue_front(); break;
      case auto_back: auto_blue_back(); break;
      case auto_skills: programming_skills(); break;
      case number_of_auto_routes: break;
    }
  }

  ctrler.print(2,0,"Auto T: %d   ",millis()-autoStartTime);
  log_ln(LOG_AUTO, "%d Auto Done in %dms", pros::millis(), pros::millis()-autoStartTime);
  auto_update_stop_task();
}

void auto_red_front() {
  //1 Pick up balls
  log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  log_ln(LOG_AUTO, "%d Move FW: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  delay(200);
  log_ln(LOG_AUTO, "%d WAIT 200: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));

  //2 Back up turn and shoot
  double first_flag_pos = front_SP.top;
  auto_set_angler_target(first_flag_pos);
  move_drive_rel(-(cap_dis-6), 200);
  log_ln(LOG_AUTO, "%d BACK UP: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
  ////pros::delay(30000); //deletes
  turn_vel( FixedAngleTarget(-87.0_deg), (200/90_deg),0);
  intake.move(0);

  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  printf("Done first shot");
  pros::delay(150);
  auto_set_second_shot(front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  printf("Done second shot");
  angler_move(ANGLER_PU_POS,100);

  drive_set(-80, 0, 0);
  pros::delay(750);
  resetGlobalAngle(); //HElLO ANJALEE, 0 IS NOW FACING THE FLAGS
  drive_fl.tare_position();
  delay(100);
  drive_set(80, 0, 0);
  while(fabs(drive_fl.get_position()) < 120) pros::delay(5);
  drive_set(0, 0, 0);

  // move_drive_rel(1.5_in,200);
  //
  // //pros::delay(30000); //delete
  //
  //
  //
  turn_vel_side(FixedAngleTarget(45_deg), (200/60_deg),0,true);
  //
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(16_in,200);
  // //pros::delay(30000); //delete
  pros::delay(250);
  move_drive_rel(-12_in,200,true);
  intake.move(0);
  turn_vel( FixedAngleTarget(51.0_deg), (200/50_deg),0);
  auto_set_first_shot(auto_front_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  auto_set_second_shot(auto_front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  intake.move(-127);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  move_drive_rel_simple(15_in,70, false);
  //pros::delay(0);
  //move_drive_rel(-15_in,200, false);
  // angler_move(ANGLER_CAP_PU_POS,100);
  // turn_vel_side(FixedAngleTarget(180_deg), (200/40_deg),0,true);
  // drive_fl.tare_position();
  // setDrive(-80, 0, 0);
  // while(fabs(drive_fl.get_position())<135) delay(10);
  // setDrive(0, 0, 0);
  // move_drive_rel_simple(15, 200, false);
  log_ln(LOG_AUTO, " > %d Done second shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
  //flatten_against_wall(true, true);

  shot_req_handled_num = 0;
}

void auto_red_back() {
    //1 PU
    log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
    angler_move(ANGLER_PU_POS, 100);
    intake.move(127);
    double cap_dis = 43.0_in;
    move_drive_rel(cap_dis, 200, true);

    //2 Turn and shoot
    move_drive_rel(-8.5_in,200);
    double first_flag_pos = auto_mid_flag_SP.top;
    auto_set_angler_target(first_flag_pos);
    turn_vel(FixedAngleTarget(-70.0_deg), 127/60_deg, -6.2_deg, 7_deg, false);//-69.2_deg hits the flag. Turn range is -68.2-70
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(150);
    auto_set_second_shot(auto_mid_flag_SP.mid);
    while (auto_set_shot) pros::delay(10);
    turn_vel(FixedAngleTarget(0_deg), 200/120_deg, 0_deg);
    angler_move(ANGLER_CAP_PU_POS + 50, 100);
    drive_set(80, 0, 0);
    delay(100);
    while(fabs(drive_fl.get_actual_velocity()) > 1) pros::delay(5);
    resetGlobalAngle();
    drive_fl.tare_position();
    drive_set(-80, 0, 0);
    while(fabs(drive_fl.get_position()) < 45) pros::delay(5);
    drive_set(0, 0, 0);
    angler_move(ANGLER_CAP_PU_POS, 100);
    move_drive_rel(6.0_in, 200);

    pros::delay(250);

    move_drive_rel(-10_in,200);

    log_ln(LOG_AUTO, "%d Before Strafe - A:%f \n", millis(), RAD_TO_DEG(getGlobalAngle()));
    auto_set_angler_target(first_flag_pos);
    delay(100);
    drive_fl.tare_position();
    drive_set(-75,0,0);
    log_ln(LOG_AUTO, ">>%d Start Strafe FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    while(fabs(drive_fl.get_position())<70){delay(10);}
    log_ln(LOG_AUTO, ">>%d Strafe Done Main LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    drive_set(0, 0, 0);
    drive_set(0);
    log_ln(LOG_AUTO, ">>%d Strafe Done Break LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    delay(50);
    log_ln(LOG_AUTO, ">>%d Strafe Done Rest LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    //6 shoot
    turn_vel( FixedAngleTarget(-58.5_deg), (200/90_deg));

    first_flag_pos = auto_far_flag_SP.top;
    auto_set_angler_target(first_flag_pos);
    //while(pros::millis() - autoStartTime < 13500) pros::delay(5);
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(250);
    auto_set_second_shot(auto_far_flag_SP.mid);
    while (auto_set_shot) pros::delay(10);
}

void auto_blue_front() {
  //1 Pick up balls
  log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  log_ln(LOG_AUTO, "%d Move FW: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  delay(200);
  log_ln(LOG_AUTO, "%d WAIT 200: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));

  //2 Back up turn and shoot
  double first_flag_pos = front_SP.top;
  auto_set_angler_target(first_flag_pos);
  move_drive_rel(-(cap_dis-3), 200);
  log_ln(LOG_AUTO, "%d BACK UP: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
  ////pros::delay(30000); //deletes
  turn_vel( FixedAngleTarget(87.0_deg), (200/90_deg),0);
  intake.move(0);

  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  printf("Done first shot");
  pros::delay(150);
  auto_set_second_shot(front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  printf("Done second shot");
  angler_move(ANGLER_PU_POS,100);

  drive_set(80, 0, 0);
  pros::delay(750);
  resetGlobalAngle(); //HElLO ANJALEE, 0 IS NOW FACING THE FLAGS
  drive_fl.tare_position();
  delay(100);
  drive_set(-80, 0, 0);
  while(fabs(drive_fl.get_position()) < 120) pros::delay(5);
  drive_set(0, 0, 0);

  // move_drive_rel(1.5_in,200);
  //
  // //pros::delay(30000); //delete
  //
  //
  //
  intake.move(127);
  turn_vel_side(FixedAngleTarget(-45_deg), (200/60_deg),0,true);
  //
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(13_in,200);
  // //pros::delay(30000); //delete
  pros::delay(500);
  move_drive_rel(-12_in,200,true);
  intake.move(0);
  turn_vel( FixedAngleTarget(-47.5_deg), (200/60_deg),0);
  auto_set_first_shot(auto_front_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  auto_set_second_shot(auto_front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  intake.move(-127);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  move_drive_rel_simple(15_in,70, false);
  //pros::delay(0);
  //move_drive_rel(-15_in,200, false);
  // angler_move(ANGLER_CAP_PU_POS,100);
  // turn_vel_side(FixedAngleTarget(180_deg), (200/40_deg),0,true);
  // drive_fl.tare_position();
  // setDrive(-80, 0, 0);
  // while(fabs(drive_fl.get_position())<135) delay(10);
  // setDrive(0, 0, 0);
  // move_drive_rel_simple(15, 200, false);
  log_ln(LOG_AUTO, " > %d Done second shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
  //flatten_against_wall(true, true);

  shot_req_handled_num = 0;
}

void auto_blue_back() {
    //1 PU
    log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
    angler_move(ANGLER_PU_POS, 100);
    intake.move(127);
    double cap_dis = 43.0_in;
    move_drive_rel(cap_dis, 200, true);

    //2 Turn and shoot
    move_drive_rel(-6.5_in,200);
    double first_flag_pos = auto_mid_flag_SP.top;
    auto_set_angler_target(first_flag_pos);
    turn_vel(FixedAngleTarget(75.0_deg), 127/60_deg, 6.2_deg, 7_deg, false);//-69.2_deg hits the flag. Turn range is -68.2-70
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(150);
    auto_set_second_shot(auto_mid_flag_SP.mid);
    while (auto_set_shot) pros::delay(10);
    turn_vel(FixedAngleTarget(0_deg), 200/120_deg, 0_deg);
    angler_move(ANGLER_CAP_PU_POS + 50, 100);
    drive_set(-80, 0, 0);
    delay(100);
    log_ln(LOG_AUTO, "%d before velocity move",pros::millis());
    while(fabs(drive_fl.get_actual_velocity()) > 1) pros::delay(5);
    log_ln(LOG_AUTO, "%d after velocity move",pros::millis());
    resetGlobalAngle();
    drive_fl.tare_position();
    drive_set(80, 0, 0);
    while(fabs(drive_fl.get_position()) < 45) pros::delay(5);
    drive_set(0, 0, 0);
    angler_move(ANGLER_CAP_PU_POS, 100);
    move_drive_rel(4.0_in, 200);

    pros::delay(250);

    move_drive_rel(-10_in,200);

    log_ln(LOG_AUTO, "%d Before Strafe - A:%f \n", millis(), RAD_TO_DEG(getGlobalAngle()));
    auto_set_angler_target(first_flag_pos);
    delay(100);
    drive_fl.tare_position();
    drive_set(75,0,0);
    log_ln(LOG_AUTO, ">>%d Start Strafe FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    while(fabs(drive_fl.get_position())<70){delay(10);}
    log_ln(LOG_AUTO, ">>%d Strafe Done Main LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    drive_set(0, 0, 0);
    drive_set(0);
    log_ln(LOG_AUTO, ">>%d Strafe Done Break LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    delay(50);
    log_ln(LOG_AUTO, ">>%d Strafe Done Rest LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    //6 shoot
    turn_vel( FixedAngleTarget(61_deg), (200/90_deg));
    first_flag_pos = auto_far_flag_SP.top;
    //while(pros::millis() - autoStartTime < 13100) pros::delay(5);
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(1000);
    auto_set_second_shot(auto_far_flag_SP.mid);
    while (auto_set_shot) pros::delay(10);
}

void programming_skills() {
  // SKILLS Runs

  // 1 PU and Flip
  angler_move(ANGLER_CAP_PU_POS, 100);
  intake.move(-80);
  move_drive_rel(43.0_in, 200);
  move_drive_rel(-5_in,200);
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  move_drive_rel(5_in,200);
  move_drive_rel(-42.0_in, 200, false);
  flatten_against_wall(false, true);
  resetGlobalAngle();
  move_drive_rel(5_in,200);
  turn_vel(FixedAngleTarget(-42.0_deg), 200/90_deg, 0_deg, true);
  auto_set_first_shot(skills_front_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(400);
  auto_set_second_shot(skills_front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  angler_move(ANGLER_PU_POS,100);
  // 3 Cap ball pickup
  turn_vel(FixedAngleTarget(-45.0_deg), 200/90_deg, 0_deg, true);
  angler_move(ANGLER_CAP_PU_POS, 100);
  move_drive_rel(25_in, 200);
  pros::delay(750);
  move_drive_rel(-8.8_in,200,true);

  //Flip
  intake.move(-70);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  move_drive_rel_simple(15_in,70, false);
  //pros::delay(0);
  move_drive_rel(-24_in,200, false);
  turn_vel(FixedAngleTarget(88.5_deg), 200/90_deg, 0_deg, true);
  // drive_fl.tare_position();
  // //delay(1000);
  // drive_set(-70, 0, 0);
  // printf("I am here aarish");
  // while(fabs(drive_fl.get_position()) < 85) pros::delay(5);
  // drive_set(0, 0, 0);
  // angler_move(ANGLER_PU_POS,100);
  // 4 Backup and shoot
  auto_set_angler_target(ANGLER_PU_POS);
  intake.move(127);
  move_drive_rel(65_in, 200, false);
  flatten_against_wall(true, true);
  resetGlobalAngle();
  move_drive_rel(-3.75_in,200);
  drive_fl.tare_position();
  drive_set(-70, 0, 0);
  printf("I am here aarish");
  while(fabs(drive_fl.get_position()) < 65) pros::delay(5);
  drive_set(0, 0, 0);
  turn_vel(FixedAngleTarget(90.0_deg), 200/90_deg, 0_deg, true);
  drive_fl.tare_position();
  drive_set(-70, 0, 0);
  printf("I am hfere aarish");
  while(fabs(drive_fl.get_position()) < 65) pros::delay(5); // GERE
  drive_set(0, 0, 0);
  flatten_against_wall(true, true);
  resetGlobalAngle();
  move_drive_rel(-8_in,200);
  turn_vel(FixedAngleTarget(90.0_deg), 200/90_deg, 0_deg, true);
  auto_set_first_shot(skills_corner_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(400);
  auto_set_second_shot(skills_corner_SP.mid);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  turn_vel(FixedAngleTarget(180_deg), (200/90_deg));

  drive_fl.tare_position();
  drive_set(-50, 0, 0);
  printf("I am here aarish");
  while(fabs(drive_fl.get_position()) < 30) pros::delay(5); // GERE
  drive_set(0, 0, 0);
  // flatten_against_wall(true, true);
  // resetGlobalAngle();
  // move_drive_rel(-4_in,200);
  // turn_vel(FixedAngleTarget(180_deg), (200/90_deg));
  intake.move(127);
  auto_set_angler_target(ANGLER_CAP_PU_POS);
  move_drive_rel(25.5_in,200);
  drive_set(75,0,0);
  pros::delay(750);
  drive_set(0,0,0);
  drive_fl.tare_position();
  resetGlobalAngle();
  pros::delay(250);
  drive_set(-75,0,0);
  printf("Encoder before: %f\n",drive_fl.get_position());
  while(fabs(drive_fl.get_position()) < 50) {delay(10);}
  drive_set(0,0,0);
  printf("Encoder after: %f\n",drive_fl.get_position());
  move_drive_rel(5_in,200);
  move_drive_rel(-10_in,200);
  angler_move(ANGLER_CAP_FLIP_POS);
  delay(750);
  intake.move(-90);
  log_ln(LOG_AUTO, "Intake turned on at -90 speed");
  move_drive_rel(15.5_in,200,false);
  move_drive_rel(-13.5_in,200);
  drive_fl.tare_position();
  drive_set(-75,0,0);
  log_ln(LOG_AUTO, ">>%d Start Strafe FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  while(fabs(drive_fl.get_position())<70){delay(10);}
  log_ln(LOG_AUTO, ">>%d Strafe Done Main LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  drive_set(0, 0, 0);
  drive_set(0);
  log_ln(LOG_AUTO, ">>%d Strafe Done Break LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  delay(50);
  log_ln(LOG_AUTO, ">>%d Strafe Done Rest LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  //6 shoot

  turn_vel( FixedAngleTarget(-59.5_deg), (200/90_deg));
  auto_set_first_shot(skills_back_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(400);
  auto_set_second_shot(skills_back_SP.bot);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  turn_vel(FixedAngleTarget(-90.0_deg), 200/90_deg, 0_deg, true);

  angler_move(ANGLER_CAP_PU_POS);
  intake.move(-80);
  move_drive_rel(20_in,200);
  turn_vel(FixedAngleTarget(0_deg), 200/90_deg, 0_deg, true);
  move_drive_rel(7.0_in,200);
  move_drive_rel(-2.0_in,200);
  intake.move(127);
  angler_move(ANGLER_PU_POS);
  move_drive_rel(7_in,200);
  move_drive_rel(-9_in,200);
  drive_set(-75,0,0);
  delay(750);
  drive_fl.tare_position();
  resetGlobalAngle();
  pros::delay(250);
  drive_set(75,0,0);
  printf("Encoder before: %f\n",drive_fl.get_position());
  while(fabs(drive_fl.get_position()) < 58) delay(10);
  drive_set(0,0,0);
  turn_vel(FixedAngleTarget(-58.0_deg), 200/85_deg, 0_deg, true);
  auto_set_first_shot(skills_back_SP.mid);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  turn_vel(FixedAngleTarget(-90_deg), 200/90_deg, 0_deg, true);

  intake.move(0);
  angler_move(ANGLER_CAP_PU_POS);
  pros::delay(250);
  flatten_against_wall(true, false);
  resetGlobalAngle();

  if (left_platform_sensor.get_value() > 1750) {
    drive_set(50, 10, 0);
    while(left_platform_sensor.get_value() > 1750) pros::delay(5);
    drive_set(-10, 10, 0);
  } else if (right_platform_sensor.get_value() > 1750) {
    drive_set(-50, 10, 0);
    while(right_platform_sensor.get_value() > 1750) pros::delay(5);
    drive_br.tare_position();
    while(drive_br.get_position() > -40) pros::delay(5);
    drive_set(10, 10, 0);
  } else if (right_platform_sensor.get_value() < 1750 && left_platform_sensor.get_value() < 1750) {
    drive_br.tare_position();
    drive_set(-50, 10, 0);
    while(drive_br.get_position() > -40) pros::delay(5);
    drive_set(10, 10, 0);
  }
  pros::delay(100);
  drive_set(0);

  drive_br.tare_position();
  for(int i = 40; i < 100; i++) {
    setDrive(0, i, 0);
    pros::delay(3);
  }
  while(drive_br.get_position() < 430) pros::delay(5);
  setDrive(0, -20, 0);
  pros::delay(150);
  setDrive(0, 0, 0);
  turn_vel(FixedAngleTarget(85.0_deg), 200/80_deg, 0, 0, 0, 100);

  flatten_against_wall(true, true, 17);
  if (right_platform_sensor.get_value() > 1500) {
    drive_set(-50, 10, 0);
    while(right_platform_sensor.get_value() > 1500) pros::delay(5);
    drive_set(10, 10, 0);
    pros::delay(100);
  } else if (left_platform_sensor.get_value() > 1500) {
    drive_set(50, 10, 0);
    while(left_platform_sensor.get_value() > 1500) pros::delay(5);
    drive_set(-10, 10, 0);
    pros::delay(100);
  }
  drive_set(0);

  drive_br.tare_position();
  for(int i = 40; i < 100; i++) {
    setDrive(0, i, 0);
    pros::delay(3);
  }
  while(drive_br.get_position() < 400) pros::delay(5);
  setDrive(0, -20, 0);
  pros::delay(150);
  setDrive(0, 0, 0);

}

void auto_red_back_mid_first() {
    //1 PU
    log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
    angler_move(ANGLER_PU_POS, 100);
    intake.move(127);
    double cap_dis = 43.0_in;
    move_drive_rel(cap_dis, 200, true);

    //2 Turn and shoot
    move_drive_rel(-7.5_in,200);
    double first_flag_pos = auto_SP.top-30;
    auto_set_angler_target(first_flag_pos);
    turn_vel(FixedAngleTarget(-72.0_deg), 127/60_deg, -6.2_deg, 7_deg, false);//-69.2_deg hits the flag. Turn range is -68.2-70
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(150);
    auto_set_second_shot(auto_SP.mid-30);
    while (auto_set_shot) pros::delay(10);
    turn_vel(FixedAngleTarget(0_deg), 200/120_deg, 0_deg);
    angler_move(ANGLER_CAP_PU_POS + 50, 100);
    drive_set(80, 0, 0);
    while(fabs(drive_fl.get_actual_velocity()) > 1) pros::delay(5);
    resetGlobalAngle();
    drive_fl.tare_position();
    drive_set(-80, 0, 0);
    while(fabs(drive_fl.get_position()) < 45) pros::delay(5);
    drive_set(0, 0, 0);
    angler_move(ANGLER_CAP_PU_POS, 100);
    move_drive_rel(5.0_in, 200);

    pros::delay(250);

    move_drive_rel(-10_in,200);

    log_ln(LOG_AUTO, "%d Before Strafe - A:%f \n", millis(), RAD_TO_DEG(getGlobalAngle()));
    first_flag_pos = auto_SP.top;
    auto_set_angler_target(first_flag_pos);
    delay(100);
    drive_fl.tare_position();
    drive_set(-75,0,0);
    log_ln(LOG_AUTO, ">>%d Start Strafe FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    while(fabs(drive_fl.get_position())<70){delay(10);}
    log_ln(LOG_AUTO, ">>%d Strafe Done Main LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    drive_set(0, 0, 0);
    drive_set(0);
    log_ln(LOG_AUTO, ">>%d Strafe Done Break LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    delay(50);
    log_ln(LOG_AUTO, ">>%d Strafe Done Rest LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    //6 shoot
    turn_vel( FixedAngleTarget(-59.5_deg), (200/90_deg));

    //while(pros::millis() - autoStartTime < 13600) pros::delay(5);
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(250);
    auto_set_second_shot(auto_SP.mid);
    while (auto_set_shot) pros::delay(10);
}

void auto_red_back_far_first() {
  log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  delay(200);
  move_drive_rel(-12_in,200);
  double first_flag_pos = front_SP.top;
  auto_set_angler_target(first_flag_pos);
  intake.move(0);
  turn_vel( FixedAngleTarget(-52_deg), (200/90_deg));
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  pros::delay(150);
  auto_set_second_shot(front_SP.mid+50);
  while (auto_set_shot) pros::delay(10);
  intake.move(127);
  angler_move(ANGLER_CAP_PU_POS,100);
  turn_vel( FixedAngleTarget(-90_deg), (200/70_deg));
  move_drive_rel(-25.5_in, 200);
  turn_vel( FixedAngleTarget(0_deg), (200/90_deg));
  move_drive_rel(12_in, 200);
  pros::delay(250);
  move_drive_rel(-8_in,200);
  first_flag_pos = front_SP.top-15;
  auto_set_angler_target(first_flag_pos);
  drive_fl.tare_position();
  drive_set(-75,0,0);
  while(fabs(drive_fl.get_position())<100){delay(10);}
  drive_set(0,0,0);
  turn_vel( FixedAngleTarget(-78_deg), (200/90_deg));
  move_drive_rel(10_in,200);
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  pros::delay(150);
  auto_set_second_shot(front_SP.mid+60);
  while (auto_set_shot) pros::delay(10);
}

void auto_blue_front() {
  //1 Pick up balls
  log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  log_ln(LOG_AUTO, "%d Move FW: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  delay(200);
  log_ln(LOG_AUTO, "%d WAIT 200: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));

  //2 Back up turn and shoot
  double first_flag_pos = front_SP.top;
  auto_set_angler_target(first_flag_pos);
  move_drive_rel(-(cap_dis-6), 200);
  log_ln(LOG_AUTO, "%d BACK UP: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
  ////pros::delay(30000); //deletes
  turn_vel( FixedAngleTarget(87.0_deg), (200/90_deg),0);
  intake.move(0);

  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  printf("Done first shot");
  pros::delay(150);
  auto_set_second_shot(front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  printf("Done second shot");
  angler_move(ANGLER_PU_POS,100);

  drive_set(80, 0, 0);
  pros::delay(750);
  resetGlobalAngle(); //HElLO ANJALEE, 0 IS NOW FACING THE FLAGS
  drive_fl.tare_position();
  delay(100);
  drive_set(-80, 0, 0);
  while(fabs(drive_fl.get_position()) < 120) pros::delay(5);
  drive_set(0, 0, 0);

  // move_drive_rel(1.5_in,200);
  //
  // //pros::delay(30000); //delete
  //
  //
  //
  turn_vel_side(FixedAngleTarget(-45_deg), (200/60_deg),0,true);
  //
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(16_in,200);
  // //pros::delay(30000); //delete
  pros::delay(500);
  move_drive_rel(-12_in,200,true);
  intake.move(0);
  turn_vel( FixedAngleTarget(-49.0_deg), (200/40_deg),0);
  auto_set_first_shot(front_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  auto_set_second_shot(front_SP.mid+4);
  while (auto_set_shot) pros::delay(10);
  intake.move(-127);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  move_drive_rel_simple(15_in,70, false);
  //pros::delay(0);
  //move_drive_rel(-15_in,200, false);
  // angler_move(ANGLER_CAP_PU_POS,100);
  // turn_vel_side(FixedAngleTarget(180_deg), (200/40_deg),0,true);
  // drive_fl.tare_position();
  // setDrive(-80, 0, 0);
  // while(fabs(drive_fl.get_position())<135) delay(10);
  // setDrive(0, 0, 0);
  // move_drive_rel_simple(15, 200, false);
  log_ln(LOG_AUTO, " > %d Done second shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
  //flatten_against_wall(true, true);

  shot_req_handled_num = 0;
}

void auto_blue_front_park() {
  // SKILLS Runs

  // 1 PU and Flip
  angler_move(ANGLER_CAP_PU_POS, 100);
  intake.move(-80);
  move_drive_rel(43.0_in, 200);
  move_drive_rel(-5_in,200);
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  move_drive_rel(5_in,200);
  move_drive_rel(-42.0_in, 200, false);
  flatten_against_wall(false, true);
  resetGlobalAngle();
  move_drive_rel(5_in,200);
  turn_vel(FixedAngleTarget(42.0_deg), 200/90_deg, 0_deg, true);
  auto_set_first_shot(skills_front_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(400);
  auto_set_second_shot(skills_front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  angler_move(ANGLER_PU_POS,100);
  // 3 Cap ball pickup
  turn_vel(FixedAngleTarget(45.0_deg), 200/90_deg, 0_deg, true);
  angler_move(ANGLER_CAP_PU_POS, 100);
  move_drive_rel(25_in, 200);
  pros::delay(750);
  move_drive_rel(-8.8_in,200,true);

  //Flip
  intake.move(-70);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  move_drive_rel_simple(15_in,70, false);
  //pros::delay(0);
  move_drive_rel(-24_in,200, false);
  turn_vel(FixedAngleTarget(-88.5_deg), 200/90_deg, 0_deg, true);
  // drive_fl.tare_position();
  // //delay(1000);
  // drive_set(-70, 0, 0);
  // printf("I am here aarish");
  // while(fabs(drive_fl.get_position()) < 85) pros::delay(5);
  // drive_set(0, 0, 0);
  // angler_move(ANGLER_PU_POS,100);
  // 4 Backup and shoot
  auto_set_angler_target(ANGLER_PU_POS);
  intake.move(127);
  move_drive_rel(65_in, 200, false);
  flatten_against_wall(true, true);
  resetGlobalAngle();
  move_drive_rel(-3.75_in,200);
  drive_fl.tare_position();
  drive_set(70, 0, 0);
  printf("I am here aarish");
  while(fabs(drive_fl.get_position()) < 65) pros::delay(5);
  drive_set(0, 0, 0);
  turn_vel(FixedAngleTarget(-90.0_deg), 200/90_deg, 0_deg, true);
  drive_fl.tare_position();
  drive_set(70, 0, 0);
  printf("I am here aarish");
  while(fabs(drive_fl.get_position()) < 65) pros::delay(5); // GERE
  drive_set(0, 0, 0);
  flatten_against_wall(true, true);
  resetGlobalAngle();
  move_drive_rel(-8_in,200);
  turn_vel(FixedAngleTarget(-90.0_deg), 200/90_deg, 0_deg, true);
  auto_set_first_shot(skills_corner_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(400);
  auto_set_second_shot(skills_corner_SP.mid);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  turn_vel(FixedAngleTarget(-180_deg), (200/90_deg));

  drive_fl.tare_position();
  drive_set(50, 0, 0);
  printf("I am here aarish");
  while(fabs(drive_fl.get_position()) < 30) pros::delay(5); // GERE
  drive_set(0, 0, 0);
  // flatten_against_wall(true, true);
  // resetGlobalAngle();
  // move_drive_rel(-4_in,200);
  // turn_vel(FixedAngleTarget(180_deg), (200/90_deg));
  intake.move(127);
  auto_set_angler_target(ANGLER_CAP_PU_POS);
  move_drive_rel(25.5_in,200);
  drive_set(-75,0,0);
  pros::delay(750);
  drive_set(0,0,0);
  drive_fl.tare_position();
  resetGlobalAngle();
  pros::delay(250);
  drive_set(75,0,0);
  printf("Encoder before: %f\n",drive_fl.get_position());
  while(fabs(drive_fl.get_position()) < 50) {delay(10);}
  drive_set(0,0,0);
  printf("Encoder after: %f\n",drive_fl.get_position());
  move_drive_rel(5_in,200);
  move_drive_rel(-10_in,200);
  angler_move(ANGLER_CAP_FLIP_POS);
  delay(750);
  intake.move(-90);
  log_ln(LOG_AUTO, "Intake turned on at -90 speed");
  move_drive_rel(15.5_in,200,false);
  move_drive_rel(-13.5_in,200);
  drive_fl.tare_position();
  drive_set(75,0,0);
  log_ln(LOG_AUTO, ">>%d Start Strafe FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  while(fabs(drive_fl.get_position())<70){delay(10);}
  log_ln(LOG_AUTO, ">>%d Strafe Done Main LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  drive_set(0, 0, 0);
  drive_set(0);
  log_ln(LOG_AUTO, ">>%d Strafe Done Break LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  delay(50);
  log_ln(LOG_AUTO, ">>%d Strafe Done Rest LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
  //6 shoot

  turn_vel( FixedAngleTarget(59.5_deg), (200/90_deg));
  auto_set_first_shot(skills_back_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(400);
  auto_set_second_shot(skills_back_SP.bot);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  turn_vel(FixedAngleTarget(90.0_deg), 200/90_deg, 0_deg, true);

  angler_move(ANGLER_CAP_PU_POS);
  intake.move(-80);
  move_drive_rel(20_in,200);
  turn_vel(FixedAngleTarget(0_deg), 200/90_deg, 0_deg, true);
  move_drive_rel(7.0_in,200);
  move_drive_rel(-2.0_in,200);
  intake.move(127);
  angler_move(ANGLER_PU_POS);
  move_drive_rel(7_in,200);
  move_drive_rel(-9_in,200);
  drive_set(75,0,0);
  delay(750);
  drive_fl.tare_position();
  resetGlobalAngle();
  pros::delay(250);
  drive_set(-75,0,0);
  printf("Encoder before: %f\n",drive_fl.get_position());
  while(fabs(drive_fl.get_position()) < 58) delay(10);
  drive_set(0,0,0);
  turn_vel(FixedAngleTarget(58.0_deg), 200/85_deg, 0_deg, true);
  auto_set_first_shot(skills_back_SP.mid);
  while (auto_set_shot) pros::delay(10);
  pros::delay(250);
  turn_vel(FixedAngleTarget(90_deg), 200/90_deg, 0_deg, true);

  intake.move(0);
  angler_move(ANGLER_CAP_PU_POS);
  pros::delay(250);
  flatten_against_wall(true, false);
  resetGlobalAngle();

  if (left_platform_sensor.get_value() > 1750) {
    drive_set(50, 10, 0);
    while(left_platform_sensor.get_value() > 1750) pros::delay(5);
    drive_br.tare_position();
    while(drive_br.get_position() < 40) pros::delay(5);
    drive_set(-10, 10, 0);
  } else if (right_platform_sensor.get_value() > 1750) {
    drive_set(-50, 10, 0);
    while(right_platform_sensor.get_value() > 1750) pros::delay(5);
    drive_set(10, 10, 0);
  } else if (right_platform_sensor.get_value() < 1750 && left_platform_sensor.get_value() < 1750) {
    drive_br.tare_position();
    drive_set(50, 10, 0);
    while(drive_br.get_position() < 40) pros::delay(5);
    drive_set(10, 10, 0);
  }
  pros::delay(100);
  drive_set(0);

  drive_br.tare_position();
  for(int i = 40; i < 100; i++) {
    setDrive(0, i, 0);
    pros::delay(3);
  }
  while(drive_br.get_position() < 430) pros::delay(5);
  setDrive(0, -20, 0);
  pros::delay(150);
  setDrive(0, 0, 0);
  turn_vel(FixedAngleTarget(-85.0_deg), 200/80_deg, 0, 0, 0, 100);

  flatten_against_wall(true, true, 17);
  if (right_platform_sensor.get_value() > 1500) {
    drive_set(-50, 10, 0);
    while(right_platform_sensor.get_value() > 1500) pros::delay(5);
    drive_set(10, 10, 0);
    pros::delay(100);
  } else if (left_platform_sensor.get_value() > 1500) {
    drive_set(50, 10, 0);
    while(left_platform_sensor.get_value() > 1500) pros::delay(5);
    drive_set(-10, 10, 0);
    pros::delay(100);
  }
  drive_set(0);

  drive_br.tare_position();
  for(int i = 40; i < 100; i++) {
    setDrive(0, i, 0);
    pros::delay(3);
  }
  while(drive_br.get_position() < 400) pros::delay(5);
  setDrive(0, 20, 0);
  pros::delay(150);
  setDrive(0, 0, 0);

}

void auto_blue_back_mid_first() {
    //1 PU
    log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
    angler_move(ANGLER_PU_POS, 100);
    intake.move(127);
    double cap_dis = 43.0_in;
    move_drive_rel(cap_dis, 200, true);

    //2 Turn and shoot
    move_drive_rel(-7.5_in,200);
    double first_flag_pos = auto_SP.top-30;
    auto_set_angler_target(first_flag_pos);
    turn_vel(FixedAngleTarget(75.0_deg), 127/60_deg, 6.2_deg, 7_deg, false);//-69.2_deg hits the flag. Turn range is -68.2-70
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(150);
    auto_set_second_shot(auto_SP.mid-30);
    while (auto_set_shot) pros::delay(10);
    turn_vel(FixedAngleTarget(0_deg), 200/120_deg, 0_deg);
    angler_move(ANGLER_CAP_PU_POS + 50, 100);
    drive_set(-80, 0, 0);
    while(fabs(drive_fl.get_actual_velocity()) > 1) pros::delay(5);
    resetGlobalAngle();
    drive_fl.tare_position();
    drive_set(80, 0, 0);
    while(fabs(drive_fl.get_position()) < 45) pros::delay(5);
    drive_set(0, 0, 0);
    angler_move(ANGLER_CAP_PU_POS, 100);
    move_drive_rel(5.0_in, 200);

    pros::delay(250);

    move_drive_rel(-10_in,200);

    log_ln(LOG_AUTO, "%d Before Strafe - A:%f \n", millis(), RAD_TO_DEG(getGlobalAngle()));
    first_flag_pos = auto_SP.top;
    auto_set_angler_target(first_flag_pos);
    delay(100);
    drive_fl.tare_position();
    drive_set(75,0,0);
    log_ln(LOG_AUTO, ">>%d Start Strafe FL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    while(fabs(drive_fl.get_position())<70){delay(10);}
    log_ln(LOG_AUTO, ">>%d Strafe Done Main LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    drive_set(0, 0, 0);
    drive_set(0);
    log_ln(LOG_AUTO, ">>%d Strafe Done Break LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    delay(50);
    log_ln(LOG_AUTO, ">>%d Strafe Done Rest LoopFL: %f, BL: %f, FR: %f, BR %f | Angle: %f", millis(), drive_fl.get_position(), drive_bl.get_position(), drive_fr.get_position(), drive_br.get_position(), RAD_TO_DEG(getGlobalAngle()));
    //6 shoot
    turn_vel( FixedAngleTarget(59.5_deg), (200/90_deg));

    while(pros::millis() - autoStartTime < 13600) pros::delay(5);
    auto_set_first_shot(first_flag_pos);
    while (auto_set_shot) pros::delay(10);
    pros::delay(250);
    auto_set_second_shot(auto_SP.mid);
    while (auto_set_shot) pros::delay(10);
}

void auto_blue_back_far_first() {
  log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  delay(200);
  move_drive_rel(-12_in,200);
  double first_flag_pos = front_SP.top;
  auto_set_angler_target(first_flag_pos);
  intake.move(0);
  turn_vel( FixedAngleTarget(56_deg), (200/90_deg));
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  pros::delay(150);
  auto_set_second_shot(front_SP.mid+50);
  while (auto_set_shot) pros::delay(10);
  intake.move(127);
  angler_move(ANGLER_CAP_PU_POS,100);
  turn_vel( FixedAngleTarget(90_deg), (200/70_deg));
  move_drive_rel(-25.5_in, 200);
  turn_vel( FixedAngleTarget(0_deg), (200/90_deg));
  move_drive_rel(12_in, 200);
  pros::delay(250);
  move_drive_rel(-8_in,200);
  first_flag_pos = front_SP.top-15;
  auto_set_angler_target(first_flag_pos);
  drive_fl.tare_position();
  drive_set(75,0,0);
  while(fabs(drive_fl.get_position())<100){delay(10);}
  drive_set(0,0,0);
  turn_vel( FixedAngleTarget(78_deg), (200/90_deg));
  move_drive_rel(10_in,200);
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  pros::delay(150);
  auto_set_second_shot(front_SP.mid+60);
  while (auto_set_shot) pros::delay(10);
}

void programming_skills() {
  //1 Pick up balls
  log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  delay(200);

  //2 Back up turn and shoot
  double first_flag_pos = front_SP.top;
  auto_set_angler_target(first_flag_pos);
  move_drive_rel(-(cap_dis-6), 200);
  turn_vel( FixedAngleTarget(-87.0_deg), (200/90_deg));
  intake.move(0);

  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  printf("Done first shot");
  pros::delay(150);
  auto_set_second_shot(front_SP.mid+35);
  while (auto_set_shot) pros::delay(10);
  printf("Done second shot");
  angler_move(ANGLER_PU_POS,100);
  move_drive_rel(6.5_in,200);

  turn_vel( FixedAngleTarget(-40_deg), (200/90_deg));
  shoot_flip_cap_on_45();

  move_drive_rel(-25_in,200);
  turn_vel(FixedAngleTarget(-90_deg), (200/90_deg));
  move_drive_rel(39_in,200);
  move_drive_rel(-115_in,200,false);
  flatten_against_wall(false, true);
  pos.a = -90_deg;
  pros::delay(1000);
  move_drive_rel(4_in,200);
  turn_vel(FixedAngleTarget(0_deg), (200/90_deg));
  setDrive(0, -40, 0);
  delay(500);
  flatten_against_wall(false, true);
  delay(200);
  intake.move(0);
  pos.a = 0_deg;
  pros::delay(1000);
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(37_in,200);
  drive_set(75,0,0);
  pros::delay(750);
  drive_set(0,0,0);
  drive_fl.tare_position();
  pos.a = 0_deg;
  pros::delay(250);
  drive_set(-75,0,0);
  printf("Encoder before: %f\n",drive_fl.get_position());
  while(drive_fl.get_position()>-38){delay(10);}
  drive_set(0,0,0);
  printf("Encoder after: %f\n",drive_fl.get_position());
  move_drive_rel(5_in,200);
  move_drive_rel(-10_in,200);
  angler_move(ANGLER_CAP_FLIP_POS);
  delay(750);
  intake.move(-80);
  move_drive_rel(14_in,200,false);
  move_drive_rel(-13.5_in,200);
  first_flag_pos = front_SP.top+34;
  auto_set_angler_target(first_flag_pos);
  intake.move(127);
  turn_vel(FixedAngleTarget(-59.5_deg), (200/90_deg));
  pros::delay(250);
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  // pros::delay(750);
  auto_set_second_shot(front_SP.mid);
  while (auto_set_shot) pros::delay(10);
  angler_move(ANGLER_PU_POS);
  turn_vel( FixedAngleTarget(-90_deg), (200/90_deg));
  move_drive_rel(23_in,200);
  turn_vel( FixedAngleTarget(0_deg), (200/90_deg));
  move_drive_rel(12_in,200);
  move_drive_rel(-6_in,200);
  delay(750);
  angler_move(ANGLER_CAP_FLIP_POS);
  intake.move(-80);
  move_drive_rel(16_in,200,false);
  move_drive_rel(-14.5_in,200);
  intake.move(127);
  drive_set(-75,0,0);
  pros::delay(1000);
  drive_fl.tare_position();
  pos.reset(pos.x,pos.y,0);
  drive_set(75,0,0);
  while(drive_fl.get_position()<75){delay(10);}
  drive_set(0,0,0);
  turn_vel( FixedAngleTarget(-57.5_deg), (200/90_deg));
  auto_set_angler_target(front_SP.mid+48);
  while (!pun_ball) pros::delay(5);
  auto_set_first_shot(front_SP.mid+48);
  while (auto_set_shot) pros::delay(10);
  delay(200);
  turn_vel( FixedAngleTarget(0_deg), (200/90_deg));
  drive_set(-75,0,0);
  pros::delay(1000);
  drive_fl.tare_position();
  pos.reset(pos.x,pos.y,0);
  drive_set(75,0,0);
  while(drive_fl.get_position()<100){delay(10);}
  drive_set(0,0,0);
  turn_vel( FixedAngleTarget(-90_deg), (200/90_deg));
  angler_move(ANGLER_CAP_PU_POS);
  flatten_against_wall(true, true);
  delay(500);
  move_drive_rel_simple(13_in,200);
  delay(500);
  pos.reset(0,0,0);
  turn_vel_auto( FixedAngleTarget(90_deg), (200/90_deg));
  flatten_against_wall(true, true);
  move_drive_rel_simple(14_in,200);
  log_ln(LOG_AUTO, " > %d Done second shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
  printf("Auto is done");
}

void autonomous() {
  is_disabled = false;
  shot_req_handle_stop_task();
  auto_update_start_task();
  log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
  autoStartTime = millis();
  setDriveVel(0);
  delay(10);
  log_ln(LOG_AUTO, "   --- %d START AUTO --- \n", pros::millis());
  log_ln(LOG_AUTO, " >>> %d PUN TEMP: %f", pros::millis(), puncherLeft.get_temperature());
  pos.reset();
  resetGlobalAngle();
  log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));

  printf("%d RAISE ANGLER \n", pros::millis());

  //Test Code
  //angler_move(ANGLER_CAP_PU_POS,100);
  //move_drive_rel(27_in, 100);
  //while (fabs(angler.get_position()-ANGLER_CAP_PU_POS) > 4) pros::delay(10);
  /*printf("%d FW \n", pros::millis());
  int max_speed = 100;
  for (int i = 0; i <= max_speed; i++) {
    setDriveVel(0, i, 0);
    printf("    >%d %d: A:%f \n", millis(), i, RAD_TO_DEG(getGlobalAngle()));
    pros::delay(5);
  }
  printf("%d HOLD \n", pros::millis());
  u_int32_t curT = millis();
  while (millis() < curT+700)
  {
    printf("    >%d A:%f \n", millis(), RAD_TO_DEG(getGlobalAngle()));
    pros::delay(10);
  }
  printf("%d BW \n", pros::millis());
  for (int i = max_speed; i >= 0; i--) {
    setDriveVel(0, i, 0);
    printf("    >%d %d: A:%f \n", i, millis(), RAD_TO_DEG(getGlobalAngle()));
    pros::delay(16);
  }
  printf("%d Done \n", pros::millis());
  setDrive(0);
  */

  /*
  resetGlobalAngle();

  double left_per = -1;
  double right_per = -0.1;
  double target = -20_deg;
  double kP = 350;
  double error;

  do {
    error = fabs(target - getGlobalAngle());
    double base_pow = error * kP + 10;
    if (base_pow > 60) base_pow = 60;
    int left_pow = round(base_pow * left_per);
    int right_pow = round(base_pow * right_per);

    drive_fl.move(left_pow);
    drive_bl.move(left_pow);
    drive_fr.move(right_pow);
    drive_br.move(right_pow);

    printf("%f %f\n", RAD_TO_DEG(error), base_pow);

    delay(1);
  } while (fabs(error) > 0.6_deg);

  double left = -15 * left_per;
  if (fabs(left) < 5) left = 5 * sgn(left);
  double right = -15 * right_per;
  if (fabs(right) < 5) right = 5 * sgn(right);
  drive_fl.move(left);
  drive_bl.move(left);
  drive_fr.move(right);
  drive_br.move(right);
  delay(100);
  drive_set(0);
  delay(1000);
  printf("%f\n", RAD_TO_DEG(getGlobalAngle()));

  return;
  */

  if (game_side == 'R') {
    switch(menu_auto_route) {
      case auto_front: auto_red_front(); break;
      case auto_back: auto_red_back_mid_first(); break;
      case auto_skills: auto_red_front_park(); break;
    }
  } else if (game_side == 'B') {
    switch(menu_auto_route) {
      case auto_front: auto_blue_front(); break;
      case auto_back: auto_blue_front_park(); break;
      case auto_skills: auto_red_front_park(); break;
    }
  }

  ctrler.print(2,0,"Auto T: %d   ",millis()-autoStartTime);
  log_ln(LOG_AUTO, "%d Auto Done in %dms", pros::millis(), pros::millis()-autoStartTime);
  auto_update_stop_task();
}
