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
void auto_set_angler_target(double target) {
  auto_angler_target = target;
  angler_move(target);
  log_ln(LOG_AUTO, "%d Angler Move to %f. CurPos:%f", millis(), auto_angler_target, angler.get_position());
}

void auto_set_first_shot(double angler_target) {
  shot_req_handled_num = 0;
  auto_set_shot = true;
  auto_set_angler_target(angler_target);
}
void auto_set_second_shot(double angler_target) {
  shot_req_handled_num = 1;
  auto_set_shot = true;
  auto_set_angler_target(angler_target);
}

void auto_update(void* param) {
  pos.reset();
  if (pun_state != PunState::Loaded) pun_state_change(PunState::Loading);
  while (true)
  {
    //if (is_disabled) printf(" >>> %d IN AUTO_UPDATE IN DISABLED\n", millis());
    //pos.update();
    pun_handle();
    pros::delay(10);
  }
}
void autonomous() {
  is_disabled = false;
  shot_req_handle_stop_task();
  pros::Task((pros::task_fn_t)auto_update);

  uint32_t autoStartTime = millis();
  setDriveVel(0);
  delay(10);
// if(autoPoti.get_value()>1600)
if (current_auto_routine == 0 && game_side == 'R')
{

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
  printf("Auto is done");
  ctrler.print(2,0,"Auto Time: %d",millis()-autoStartTime);


  shot_req_handled_num = 0;
}
else if (current_auto_routine == 1 && game_side == 'R')
{
  log_ln(LOG_AUTO, "%d Angler Start move: %d", millis(), angler.get_position());
  angler_move(ANGLER_PU_POS, 100);
  intake.move(127);
  double cap_dis = 43.0_in;
  move_drive_rel(cap_dis, 200);
  delay(200);
  move_drive_rel(-22_in,200);
  double first_flag_pos = front_SP.top+10;
  auto_set_angler_target(first_flag_pos);
  intake.move(0);
  turn_vel( FixedAngleTarget(-72_deg), (200/90_deg));
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  auto_set_second_shot(front_SP.mid+40);
  while (auto_set_shot) pros::delay(10);
  intake.move(127);
  angler_move(ANGLER_CAP_PU_POS,100);
  move_drive_rel(-26.5_in, 200);
  turn_vel( FixedAngleTarget(0_deg), (200/90_deg));
  move_drive_rel(27_in, 200);
  pros::delay(250);
  move_drive_rel(-8_in,200);
  first_flag_pos = front_SP.top+10;
  auto_set_angler_target(first_flag_pos);
  drive_fl.tare_position();
  drive_set(-75,0,0);
  while(fabs(drive_fl.get_position())<100){delay(10);}
  drive_set(0,0,0);
  turn_vel( FixedAngleTarget(-57_deg), (200/90_deg));
  move_drive_rel(10_in,200);
  auto_set_first_shot(first_flag_pos);
  while (auto_set_shot) pros::delay(10);
  auto_set_second_shot(front_SP.mid+60);
  while (auto_set_shot) pros::delay(10);
  ctrler.print(2,0,"Auto T: %d",millis()-autoStartTime);
  //2 Back up turn and shoot
}
}
