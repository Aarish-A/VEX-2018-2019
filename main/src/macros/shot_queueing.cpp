#include "shot_queueing.hpp"

pilons::Task shot_queue_handle_task("Shot Request Handle", shot_queue_handle, shot_task_cleanup);
pros::Mutex shot_mutex;

volatile Shot_Pos front_SP = {160, 60, 0};
volatile Shot_Pos platform_SP = {0, 0, 0};
volatile Shot_Pos back_SP = {0, 0, 0};

volatile Shot_Pos auto_back_mid_SP = {0, 0, 0};
volatile Shot_Pos auto_back_far_SP = {0, 0, 0};
uint32_t time_start = pros::millis();
Field_Position field_position = Field_Position::FRONT;
std::vector<Shot_Target> shot_queue;

void shot_queue_init() {
  shot_queue.reserve(5);
}

void trigger_shot_queue() {
  if (!shot_queue_handle_task.running()) shot_queue_handle_task.start_task();
}

void make_shot_request(uint8_t shot_height, Turn_Direction direction, Field_Position target_field_pos, bool trigger_shot) {
  if (target_field_pos == field_position) {
    if (shot_queue.size() == 2) shot_queue.pop_back();

    vector flag_position = {0, 0};
    bool turning = true;

    switch(field_position) {
      case Field_Position::FRONT:
        if (direction == Turn_Direction::STRAIGHT) turning = false; //shot_queue.push_back({shot_height});
        break;
      case Field_Position::RED_PF:
        if (game_side == 'R') {
          if (direction == Turn_Direction::LEFT) flag_position = {-31, 94}; //shot_queue.push_back({shot_height, {-31, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {18, 94}; //shot_queue.push_back({shot_height, {18, 94}});
        } else if (game_side == 'B') {
          if (direction == Turn_Direction::LEFT) flag_position = {-31 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {-31 + FLAG_WIDTH, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {18 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {18 + FLAG_WIDTH, 94}});
        }
        break;
      case Field_Position::BLUE_PF:
        if (game_side == 'R') {
          if (direction == Turn_Direction::LEFT) flag_position = {-27.5, 94}; // shot_queue.push_back({shot_height, {-27.5, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5, 94}; //shot_queue.push_back({shot_height, {19.5, 94}});
        } else if (game_side == 'B') {
          if (direction == Turn_Direction::LEFT) flag_position = {-27.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {-27.5 + FLAG_WIDTH, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {19.5 + FLAG_WIDTH, 94}});
        }
        break;
      case Field_Position::BACK:
        if (direction == Turn_Direction::STRAIGHT) turning = false; //shot_queue.push_back({shot_height});
        else if (game_side == 'R') {
          if (direction == Turn_Direction::LEFT) flag_position = {-45, 123}; // shot_queue.push_back({shot_height, {-27.5, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {45, 123}; //shot_queue.push_back({shot_height, {19.5, 94}});
        } else if (game_side == 'B') {
          if (direction == Turn_Direction::LEFT) flag_position = {-27.5, 123}; // shot_queue.push_back({shot_height, {-27.5, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5, 123}; //shot_queue.push_back({shot_height, {19.5, 94}});
        }
        break;
    }

    // Shot_Target temp = {shot_height, flag_position, turning};
    if (shot_mutex.take(3)) shot_queue.emplace_back(shot_height, flag_position, turning);//shot_queue.push_back(temp);
    shot_mutex.give();
  }
  // shot_mutex.give();

  if (trigger_shot) trigger_shot_queue();
}

void change_field_position(Field_Position new_field_pos) {
  field_position = new_field_pos;
  shot_queue.clear();
}

void shot_queue_handle(void* param) {
  Field_Position temp_field_pos = field_position;
  for (int i = 0; i < shot_queue.size(); i++) {
      while(!shot_mutex.take(3)) pros::delay(1);
    // if (shot_mutex.take(5)) {
      Shot_Target temp_target(0);
      if (i < shot_queue.size() && i >= 0) temp_target = shot_queue.at(i);
      else {
        shot_mutex.give();
        break;
      }
      shot_mutex.give();
      uint32_t start_time = pros::millis();
      printf(">>>>>Started queue handle: %d\n", start_time);
      // printf("\n%d Start Shot Queue handle \n\n", pros::millis());
      time_start = pros::millis();
      if (temp_target.turning) {
        if (i == 0) {
          if(field_position==Field_Position::RED_PF || field_position==Field_Position::BLUE_PF){
          drive.set_power(0, 10, 0);
          pros::delay(150);
          pos.reset(0, 0, 0);
          drive.set_vel(0);
          pros::delay(20);
          printf("target: x: %f, y: %f\n", temp_target.flag_position.x, temp_target.flag_position.y);
          drive_move_sync(-4_in);
          }
          else {
            drive.set_power(0, -10, 0);
            pros::delay(150);
            pos.reset(0, 0, 0);
            drive.set_vel(0);
            pros::delay(20);
            printf("target: x: %f, y: %f\n", temp_target.flag_position.x, temp_target.flag_position.y);
            drive_move_sync(4_in);
          }
          drive_turn_sync(PointAngleTarget(temp_target.flag_position));
          // drive_turn_side(PointAngleTarget(temp_target.flag_position), (200/60_deg), 0, false);
        } else if (i == 1) {
          drive_turn_sync(PointAngleTarget(temp_target.flag_position));
        }
      }
      if (temp_field_pos == Field_Position::BACK && !temp_target.turning) drive_move_sync(4_in);
      angler.move_to(temp_target.angler_target);
    //  drive.lock();
      angler.wait_for_target_reach();
      uint32_t puncher_time = pros::millis();
      puncher.shoot();
      puncher.wait_for_shot_finish();
      printf("shot time: %d", (pros::millis()-puncher_time));
      //drive.unlock();
      // drive.wait_for_stop();
      printf(">>>>>>%d Stop Shot Queue handle %d\n", pros::millis(), pros::millis() - start_time);
    // }


    // shot_mutex.take(TIMEOUT_MAX);
    // Shot_Target temp_target = shot_queue.at(i);
    // shot_mutex.give();
    // uint32_t start_time = pros::millis();
    // printf(">>>>>Started queue handle: %d\n", start_time);
    // // printf("\n%d Start Shot Queue handle \n\n", pros::millis());
    // time_start = pros::millis();
    // if (temp_target.turning) {
    //   if (i == 0) {
    //     drive.set_power(0, 10, 0);
		// 	  pros::delay(150);
    //     pos.reset(0, 0, 0);
    //     drive.set_vel(0);
    //     pros::delay(20);
    //     printf("target: x: %f, y: %f\n", temp_target.flag_position.x, temp_target.flag_position.y);
    //     drive_move_sync(-4_in);
    //     drive_turn_sync(PointAngleTarget(temp_target.flag_position));
    //     // drive_turn_side(PointAngleTarget(temp_target.flag_position), (200/60_deg), 0, false);
    //   } else if (i == 1) {
    //     drive_turn_sync(PointAngleTarget(temp_target.flag_position));
    //   }
    // }
    // if (temp_field_pos == Field_Position::BACK && !temp_target.turning) drive_move_sync(2_in);
    // angler.move_to(temp_target.angler_target);
    // angler.wait_for_target_reach();
    // puncher.shoot();
    // puncher.wait_for_shot_finish();
    // // drive.wait_for_stop();
    // printf(">>>>>>%d Stop Shot Queue handle %d\n", pros::millis(), pros::millis() - start_time);
  }
  shot_queue_handle_task.stop_task();
}

void shot_task_cleanup() {
  if (puncher.shooting()) puncher.cancel_shot();
  shot_queue.clear();
  printf("Time taken: %d\n",pros::millis() - time_start);
  angler.move_to(Angler::PICKUP_POSITION);
  intake.intake();
}
