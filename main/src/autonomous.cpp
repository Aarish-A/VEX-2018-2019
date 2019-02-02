#include "main.h"
#include "auto.hpp"
#include "config.hpp"
#include "puncher.hpp"
#include "shot_select.hpp"
#include "angler.hpp"
#include "drive.hpp"

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

void shoot_flip_cap_on_45()
{
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(17.5_in,200);
  //pros::delay(30000); //delete
  pros::delay(750);
  move_drive_rel(-8.8_in,200,true);

  //Flip
  intake.move(-70);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  move_drive_rel_simple(15_in,70, false);
  //pros::delay(0);
  move_drive_rel(-15_in,200, false);

  //Shoot
  intake.move(0);
  auto_set_first_shot(front_SP.top);
  while (auto_set_shot) pros::delay(10);
  pros::delay(100);
  auto_set_second_shot(front_SP.mid+4);
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
  move_drive_rel(-6_in,200);
  delay(500);
  angler_move(ANGLER_CAP_FLIP_POS);
  intake.move(-127);
  move_drive_rel(16_in,200,true);
  intake.move(127);
  //2 Back up turn and shoot
  double first_flag_pos = front_SP.top+28;
  auto_set_angler_target(first_flag_pos);
  move_drive_rel(-(cap_dis-6), 200);
  turn_vel( FixedAngleTarget(-87.0_deg), (200/90_deg));
  intake.move(0);

  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
  printf("Done first shot");
  auto_set_second_shot(front_SP.mid+7);
  while (auto_set_shot) pros::delay(10);
  printf("Done second shot");
  angler_move(ANGLER_PU_POS,100);
  move_drive_rel(6.5_in,200);
  turn_vel( FixedAngleTarget(-41_deg), (200/90_deg));
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(17.5_in,200);
  pros::delay(750);
  move_drive_rel(-8.8_in,200,true);
  auto_set_first_shot(front_SP.top+29);
  while (auto_set_shot) pros::delay(10);
  auto_set_second_shot(front_SP.mid+4);
  while (auto_set_shot) pros::delay(10);
  auto_set_angler_target(ANGLER_CAP_FLIP_POS);
  intake.move(-70);
  move_drive_rel(15_in,200,false);
  move_drive_rel(-25_in,200);
  turn_vel(FixedAngleTarget(-90_deg), (200/90_deg));
  move_drive_rel(39_in,200);
  move_drive_rel(-110_in,200,false);
  flatten_against_wall(false, true);
  pos.reset(84_in, pos.y, -90_deg);
  pros::delay(1000);
  move_drive_rel(4_in,200);
  turn_vel(FixedAngleTarget(0_deg), (200/90_deg));
  flatten_against_wall(false, true);
  delay(200);
  intake.move(0);
  pos.reset(pos.x, 0, 0_deg);
  pros::delay(1000);
  angler_move(ANGLER_CAP_PU_POS,100);
  intake.move(127);
  move_drive_rel(37_in,200);
  drive_set(75,0,0);
  pros::delay(750);
  drive_set(0,0,0);
  drive_fl.tare_position();
  pos.reset(pos.x,pos.y,0);
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
  move_drive_rel(-16_in,200);
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
}

void autonomous() {
  is_disabled = false;
  shot_req_handle_stop_task();
  auto_update_start_task();
  log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
  uint32_t autoStartTime = millis();
  setDriveVel(0);
    delay(10);
  log_ln(LOG_AUTO, "   --- %d START AUTO --- \n", pros::millis());
  log_ln(LOG_AUTO, " >>> %d PUN TEMP: %f", pros::millis(), puncherLeft.get_temperature());
  pos.reset();
  resetGlobalAngle();
  // if(autoPoti.get_value()>1600)
  log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
  if (game_side == 'R')
  {
    switch(current_auto_routine)
    {
      case auto_routines::FRONT:
      {
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
        double first_flag_pos = front_SP.top-30;
        auto_set_angler_target(first_flag_pos);
        move_drive_rel(-(cap_dis-6), 200);
        log_ln(LOG_AUTO, "%d BACK UP: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
        log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
        ////pros::delay(30000); //deletes
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

        //pros::delay(30000); //delete



        turn_vel(FixedAngleTarget(-40_deg), (200/90_deg));
        shoot_flip_cap_on_45();

        log_ln(LOG_AUTO, " > %d Done second shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
        log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
        printf("Auto is done");
        ctrler.print(2,0,"Auto T: %d",millis()-autoStartTime);
        //flatten_against_wall(true, true);

        shot_req_handled_num = 0;
        break;
      }
      case auto_routines::FRONT_PARK:
      {
        programming_skills();
        /*
        //1 Shoot first ball
        log_ln(LOG_AUTO, "%d 0 L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
        log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
        angler_move(front_SP.top, 200);
        move_drive_rel(3, 50);
        log_ln(LOG_AUTO, "%d L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
        turn_vel(FixedAngleTarget(-45.0_deg), (200/90_deg));


        auto_set_first_shot(front_SP.top);
        //2 Drive to cap & Scrape
        angler_move(ANGLER_CAP_PU_POS,100);
        intake.move(127);
        double cap_dis = 24;
        move_drive_rel(cap_dis, 200);
        pros::delay(750);

        //3 Back up shoot first ball
        double flip_b_w = -8.5;
        move_drive_rel(flip_b_w, 200);
        auto_set_first_shot(front_SP.mid);

        //4 Flip
        intake.move(-70);
        auto_set_angler_target(ANGLER_CAP_FLIP_POS);
        double flip_f_w = 15;
        move_drive_rel_simple(flip_f_w, 70, false);
        intake.move(0);
        //pros::delay(0);

        //5 BU & PU balls
        log_ln(LOG_AUTO, "%d 5) L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
        move_drive_rel(-(cap_dis + flip_b_w + flip_f_w), 200, false);
        angler_move(ANGLER_PU_POS, 100);
        intake.move(127);
        turn_vel(FixedAngleTarget(0), (200/90_deg));
        cap_dis = 43.0_in;
        move_drive_rel(cap_dis, 200);
        log_ln(LOG_AUTO, "%d Move FW: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
        delay(200);
        log_ln(LOG_AUTO, "%d WAIT 200: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));

        //6 Back up turn and shoot
        double first_flag_pos = front_SP.top-30;
        auto_set_angler_target(first_flag_pos);
        move_drive_rel(-(cap_dis-6), 200);
        log_ln(LOG_AUTO, "%d BACK UP: L:%d, R:%d, Drive Angle:%f", millis(), enc_l.get_value(), enc_r.get_value(), RAD_TO_DEG(getGlobalAngle()));
        log_ln(LOG_AUTO, "%d Drive Angle:%f", millis(), RAD_TO_DEG(getGlobalAngle()));
        ////pros::delay(30000); //deletes
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
        */
        /*
        double cap_dis = 43.0_in;
        move_drive_rel(cap_dis, 200);
        delay(200);

        //2 Back up turn and shoot
        double first_flag_pos = front_SP.top;
        auto_set_angler_target(first_flag_pos);
        move_drive_rel(-(cap_dis-4), 200);
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
        angler_move(ANGLER_PU_POS, 200);
        intake.move(127);

        // Flip Bottom Flag
        turn_vel( FixedAngleTarget(-90.0_deg), (200/70_deg));
        double bot_flag_dis = 41;
        move_drive_rel(bot_flag_dis, 200, false);
        pros::delay(250);
        move_drive_rel(-bot_flag_dis, 200, true);
        turn_vel( FixedAngleTarget(-45_deg), (200/90_deg));

        //Back up and low park

        move_drive_rel(-(bot_flag_dis + 28), 200);
        intake.move(0);
        angler_move(ANGLER_PARK_POS);
        turn_vel(FixedAngleTarget(0_deg), (200/90_deg));
        move_drive_rel_simple(30, 200, false);

        */
        break;
      }
      case auto_routines::BACK_MID_FIRST:
      {
        log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
        angler_move(ANGLER_PU_POS, 100);
        intake.move(127);
        double cap_dis = 43.0_in;
        move_drive_rel(cap_dis, 200);
        delay(200);
        move_drive_rel(-22_in,200);
        double first_flag_pos = auto_SP.top-30;
        auto_set_angler_target(first_flag_pos);
        intake.move(0);
        turn_vel( FixedAngleTarget(-69.5_deg), (200/90_deg));
        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        pros::delay(150);
        auto_set_second_shot(auto_SP.mid-30);
        while (auto_set_shot) pros::delay(10);
        intake.move(127);
        angler_move(ANGLER_CAP_PU_POS,100);
        move_drive_rel(-26.5_in, 200);
        turn_vel( FixedAngleTarget(0_deg), (200/90_deg));
        move_drive_rel(27_in, 200);
        pros::delay(250);
        move_drive_rel(-8_in,200);
        first_flag_pos = auto_SP.top;
        auto_set_angler_target(first_flag_pos);
        drive_fl.tare_position();
        drive_set(-75,0,0);
        while(fabs(drive_fl.get_position())<100){delay(10);}
        drive_set(0,0,0);
        turn_vel( FixedAngleTarget(-56_deg), (200/90_deg));
        move_drive_rel(10_in,200);
        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        pros::delay(150);
        auto_set_second_shot(auto_SP.mid);
        while (auto_set_shot) pros::delay(10);
        ctrler.print(2,0,"Auto T: %d, %d",millis()-autoStartTime, puncherLeft.get_temperature());
        break;
      }
      case auto_routines::BACK_FAR_FIRST:
      {
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
        ctrler.print(2,0,"Auto T: %d",millis()-autoStartTime);
        break;
      }
    }
  }
  else if (game_side == 'B')
  {
    switch(current_auto_routine)
    {
      case auto_routines::FRONT:
      {
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
        turn_vel( FixedAngleTarget(87.0_deg), (200/90_deg));
        intake.move(0);

        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
        printf("Done first shot");
        pros::delay(150);
        auto_set_second_shot(front_SP.mid+20);
        while (auto_set_shot) pros::delay(10);
        printf("Done second shot");
        angler_move(ANGLER_PU_POS,100);
        move_drive_rel(6.5_in,200);

        //Turn towwards cap
        turn_vel( FixedAngleTarget(45_deg), (200/90_deg));
        shoot_flip_cap_on_45();

        log_ln(LOG_AUTO, " > %d Done second shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
        log_ln(LOG_AUTO, "%d done turn shoot (%f, %f, %f)", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
        printf("Auto is done");
        ctrler.print(2,0,"Auto T: %d",millis()-autoStartTime);
        //flatten_against_wall(true, true);

        shot_req_handled_num = 0;
        break;
      }
      case auto_routines::FRONT_PARK:
      {
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
        turn_vel( FixedAngleTarget(87.0_deg), (200/90_deg));
        intake.move(0);

        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        log_ln(LOG_AUTO, " > %d Done first shot | angler:%f targ:%f |(%f, %f, %f)", millis(), angler.get_position(), auto_angler_target, pos.x, pos.y, RAD_TO_DEG(pos.a));
        printf("Done first shot");
        pros::delay(150);
        auto_set_second_shot(front_SP.mid+20);
        while (auto_set_shot) pros::delay(10);
        printf("Done second shot");
        angler_move(ANGLER_PU_POS,200);
        intake.move(127);

        // Flip Bottom Flag
        turn_vel( FixedAngleTarget(90.0_deg), (200/70_deg));
        double bot_flag_dis = 41;
        move_drive_rel(bot_flag_dis, 200, false);
        pros::delay(250);
        move_drive_rel(-bot_flag_dis, 200, true);
        turn_vel( FixedAngleTarget(45_deg), (200/90_deg));

        //Back up and low park
        /*
        move_drive_rel(-(bot_flag_dis + 28), 200);
        intake.move(0);
        angler_move(ANGLER_PARK_POS);
        turn_vel(FixedAngleTarget(0_deg), (200/90_deg));
        move_drive_rel_simple(30, 200, false);
        */
        break;
      }
      case auto_routines::BACK_MID_FIRST:
      {
        log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
        angler_move(ANGLER_PU_POS, 100);
        intake.move(127);
        double cap_dis = 43.0_in;
        move_drive_rel(cap_dis, 200);
        delay(200);
        move_drive_rel(-22_in,200);
        double first_flag_pos = auto_SP.top-40;
        auto_set_angler_target(first_flag_pos);
        intake.move(0);
        turn_vel( FixedAngleTarget(71_deg), (200/90_deg));
        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        pros::delay(150);
        auto_set_second_shot(auto_SP.mid-30);
        while (auto_set_shot) pros::delay(10);
        intake.move(127);
        angler_move(ANGLER_CAP_PU_POS,100);
        move_drive_rel(-26.5_in, 200);
        turn_vel( FixedAngleTarget(1.5_deg), (200/90_deg));
        move_drive_rel(27_in, 200);
        pros::delay(250);
        move_drive_rel(-8_in,200);
        first_flag_pos = auto_SP.top;
        auto_set_angler_target(first_flag_pos);
        drive_fl.tare_position();
        drive_set(75,0,0);
        while(fabs(drive_fl.get_position())<100){delay(10);}
        drive_set(0,0,0);
        turn_vel( FixedAngleTarget(57_deg), (200/90_deg));
        move_drive_rel(10_in,200);
        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        pros::delay(150);
        auto_set_second_shot(auto_SP.mid);
        while (auto_set_shot) pros::delay(10);
        ctrler.print(2,0,"Auto T: %d",millis()-autoStartTime);
        break;
      }
      case auto_routines::BACK_FAR_FIRST:
      {
        log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
        angler_move(ANGLER_PU_POS, 100);
        intake.move(127);
        double cap_dis = 43.0_in;
        move_drive_rel(cap_dis, 200);
        delay(200);
        move_drive_rel(-12_in,200);
        double first_flag_pos = auto_SP.top;
        auto_set_angler_target(first_flag_pos);
        delay(200);
        intake.move(0);
        turn_vel( FixedAngleTarget(55_deg), (200/90_deg));
        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        pros::delay(150);
        auto_set_second_shot(front_SP.mid+50);
        while (auto_set_shot) pros::delay(10);
        intake.move(127);
        angler_move(ANGLER_CAP_PU_POS,100);
        turn_vel( FixedAngleTarget(90_deg), (200/70_deg));
        move_drive_rel(-25.5_in, 200);
        turn_vel( FixedAngleTarget(1.5_deg), (200/90_deg));
        move_drive_rel(12_in, 200);
        pros::delay(250);
        move_drive_rel(-8_in,200);
        first_flag_pos = front_SP.top-15;
        auto_set_angler_target(first_flag_pos);
        drive_fl.tare_position();
        drive_set(75,0,0);
        while(fabs(drive_fl.get_position())<100){delay(10);}
        drive_set(0,0,0);
        turn_vel( FixedAngleTarget(79_deg), (200/90_deg));
        move_drive_rel(10_in,200);
        auto_set_first_shot(first_flag_pos);
        while (auto_set_shot) pros::delay(10);
        pros::delay(150);
        auto_set_second_shot(front_SP.mid+60);
        while (auto_set_shot) pros::delay(10);
        ctrler.print(2,0,"Auto T: %d",millis()-autoStartTime);
        break;
      }
    }
  }
  log_ln(LOG_AUTO, "%d Auto Done in %dms", pros::millis(), pros::millis()-autoStartTime);
  auto_update_stop_task();
}
