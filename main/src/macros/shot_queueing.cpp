#include "shot_queueing.hpp"

pilons::Task shot_queue_handle_task("Shot Request Handle", shot_queue_handle, shot_task_cleanup);
pros::Mutex shot_mutex;

volatile Shot_Pos front_SP = {160, 60, 0};
volatile Shot_Pos platform_SP = {0, 0, 0};
volatile Shot_Pos back_SP = {0, 0, 0};

volatile Shot_Pos auto_back_mid_SP = {0, 0, 0};
volatile Shot_Pos auto_back_far_SP = {0, 0, 0};
Field_Position field_position = Field_Position::FRONT;
std::deque<Shot_Target> shot_queue;

void trigger_shot_queue() {
  if (!shot_queue_handle_task.running()) {
    shot_queue_handle_task.start_task();
    log_ln(MACRO, " >>>>>> TRIGGER SHOT \n\n\n");
  }
}

void make_shot_request(uint8_t shot_height, Turn_Direction direction, Field_Position target_field_pos) {
  if (target_field_pos != field_position) return;

  if (shot_queue.size() == 2) shot_queue.pop_back();

  log_ln(MACRO, " >>>> START MAKING SHOT REQUEST | SIZE: %d \n", shot_queue.size());

  vector flag_position = {0, 0};
  bool turning = true;

  switch(field_position) {
    case Field_Position::FRONT:
      if (direction == Turn_Direction::STRAIGHT) turning = false; //shot_queue.push_back({shot_height});
      break;
    case Field_Position::RED_PF:
      if (game_side == 'R') {
        if (direction == Turn_Direction::LEFT) flag_position = {-33, 94}; //shot_queue.push_back({shot_height, {-31, 94}});
        else if (direction == Turn_Direction::RIGHT) flag_position = {12.5, 94}; //shot_queue.push_back({shot_height, {18, 94}});
        else if (direction == Turn_Direction::FAR) flag_position = {78, 94};
      } else if (game_side == 'B') {
        if (direction == Turn_Direction::LEFT) flag_position = {-31 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {-31 + FLAG_WIDTH, 94}});
        else if (direction == Turn_Direction::RIGHT) flag_position = {15.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {18 + FLAG_WIDTH, 94}});
      }
      break;
    case Field_Position::BLUE_PF:
      if (game_side == 'R') {
        if (direction == Turn_Direction::LEFT) flag_position = {-27.5, 94}; //78 shot_queue.push_back({shot_height, {-27.5, 94}});
        else if (direction == Turn_Direction::RIGHT) flag_position = {17.5, 94}; //shot_queue.push_back({shot_height, {19.5, 94}});
        else if (direction == Turn_Direction::FAR) flag_position = {-78,94};
      } else if (game_side == 'B') {
        if (direction == Turn_Direction::LEFT) flag_position = {-27.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {-27.5 + FLAG_WIDTH, 94}});
        else if (direction == Turn_Direction::RIGHT) flag_position = {17.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {19.5 + FLAG_WIDTH, 94}});
      }
      break;
    case Field_Position::BACK:
      if (direction == Turn_Direction::STRAIGHT) {
        flag_position = {0 , 123}; //shot_queue.push_back({shot_height});
        turning = shot_queue.size() > 0 ? true : false;
      }
      else if (game_side == 'R') {
        if (direction == Turn_Direction::LEFT) flag_position = {-48, 123}; // shot_queue.push_back({shot_height, {-27.5, 94}});
        else if (direction == Turn_Direction::RIGHT) flag_position = {48, 123}; //shot_queue.push_back({shot_height, {19.5, 94}});
      } else if (game_side == 'B') {
        if (direction == Turn_Direction::LEFT) flag_position = {-48 + FLAG_WIDTH, 123}; // shot_queue.push_back({shot_height, {-27.5, 94}});
        else if (direction == Turn_Direction::RIGHT) flag_position = {48 + FLAG_WIDTH, 123}; //shot_queue.push_back({shot_height, {19.5, 94}});
      }
      break;
  }

  // Shot_Target temp = {shot_height, flag_position, turning};
  if (shot_mutex.take(3)) {
    shot_queue.emplace_back(shot_height, flag_position, turning, direction);//shot_queue.push_back(temp);
    log_ln(MACRO, "Added to shot queue: %d, (%f, %f), %s | Size: %d  \n\n\n >>>", shot_height, flag_position.x, flag_position.y, turning ? "Turning" : "Not Turning", shot_queue.size());
  }
  shot_mutex.give();
}

void change_field_position(Field_Position new_field_pos) {
  field_position = new_field_pos;
  shot_queue.clear();
}

void shot_queue_handle(void* param) {
  // pos.reset(0, 0, 0);
  // drive_move_async(-5_in);
  // drive.wait_for_stop();
  // pos.reset(0, 0, 0);
  // drive_turn_async(FixedAngleTarget(0_deg));
  // drive.wait_for_stop();

  Field_Position temp_field_pos = field_position;
  uint32_t macro_start_time = pros::millis();
  Turn_Direction last_turn_direction = Turn_Direction::STRAIGHT;
  log_ln(MACRO, "STARTED SHOT QUEUE HANDLING");

  for (int i = 0; i < shot_queue.size(); i++) {
    // log_ln(MACRO, "     >>>> shot_queue_handle_intern num: %d | size: %d", i, shot_queue.size());
    while(!shot_mutex.take(3)) pros::delay(1);
    Shot_Target temp_target(0);
    if (i < shot_queue.size() && i >= 0) temp_target = shot_queue.at(i);
    else { shot_mutex.give(); break; }
    shot_mutex.give();
    uint32_t shot_start_time = pros::millis();
    log_ln(MACRO, "Started shot %d", i + 1);

    if (i == 0) {
      if (temp_field_pos == Field_Position::RED_PF || temp_field_pos == Field_Position::BLUE_PF) {
        drive.set_power(0, 10, 0);
        pros::delay(100);
        pos.reset(0, 0, 0);
        drive.set_power(0);
        drive_move_sync(-6_in);
      } else if (temp_field_pos == Field_Position::BACK) {
        drive.set_power(0, -10, 0);
        pros::delay(100);
        pos.reset(0, 0, 0);
        drive.set_power(0);
        if (temp_target.turning) {
           drive_move_sync(4_in);
        }
        else {
          //log_ln(MOVE, "vexOS ");
          drive_move_async(4_in);
          pros::delay(20); // DO NOT DELETE THIS DELAY THIS IS REALLY IMPORTANT, WILL DRY SHOOT IF U DELETE!!!! @ZAIN @ANJALEE @STRAUSS @ANYONE ELSE THAT READS THIS
        }
      }
    }
    intake.stop();
    angler.move_to(temp_target.angler_target);
    if (temp_target.turning && (i == 1 ? temp_target.turn_direction != last_turn_direction : true)) {
      drive_turn_sync(PointAngleTarget(temp_target.flag_position));
      // while(fabs(drive.get_error()) > 10_deg) pros::delay(2);
      pros::delay(20); // DO NOT DELETE THIS DELAY THIS IS REALLY IMPORTANT, WILL DRY SHOOT IF U DELETE!!!! @ZAIN @ANJALEE @STRAUSS @ANYONE ELSE THAT READS THIS
    }
    drive.lock();
    // angler.wait_for_target_reach();
    // while(fabs(angler.get_error()) / 7 > 15) pros::delay(2);
    // log_ln(MACRO, "Started shot %d", i + 1);
    while(fabs(angler.get_error()) / 7 > 20) pros::delay(2);
    puncher.wait_for_ball_drop();
    puncher.shoot();
    puncher.wait_for_shot_finish();
    log_ln(MACRO, "Finished shot %d", i + 1);

    drive.unlock();
    drive.wait_for_stop();
    // log_ln(MACRO, "FINISHED SHOT %d, TOOK %d MS", i + 1, pros::millis() - shot_start_time);
    last_turn_direction = temp_target.turn_direction;
  }
  log_ln(MACRO, "FINISHED SHOT QUEUE HANDLING, TOOK %d MS", pros::millis() - macro_start_time);
  shot_queue_handle_task.stop_task();
}

void shot_task_cleanup() {
  if (puncher.shooting()) puncher.cancel_shot();
  shot_queue.clear();
  angler.move_to(Angler::PICKUP_POSITION);
  if (!menu_enabled) intake.intake();
}
