#include "shot_queueing.hpp"

pilons::Task shot_queue_handle_task("Shot Request Handle", shot_queue_handle, shot_task_cleanup);

volatile Shot_Pos front_SP = {160, 60, 0};
volatile Shot_Pos platform_SP = {0, 0, 0};
volatile Shot_Pos back_SP = {0, 0, 0};

volatile Shot_Pos auto_back_mid_SP = {0, 0, 0};
volatile Shot_Pos auto_back_far_SP = {0, 0, 0};

Field_Position field_position = Field_Position::FRONT;
std::deque<Shot_Target> shot_queue;

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
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5, 24}; //shot_queue.push_back({shot_height, {19.5, 94}});
        } else if (game_side == 'B') {
          if (direction == Turn_Direction::LEFT) flag_position = {-27.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {-27.5 + FLAG_WIDTH, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5 + FLAG_WIDTH, 94}; //shot_queue.push_back({shot_height, {19.5 + FLAG_WIDTH, 94}});
        }
        break;
      case Field_Position::BACK:
        if (direction == Turn_Direction::STRAIGHT) turning = false; //shot_queue.push_back({shot_height});
        else if (game_side == 'R') {
          if (direction == Turn_Direction::LEFT) flag_position = {-27.5, 94}; // shot_queue.push_back({shot_height, {-27.5, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5, 24}; //shot_queue.push_back({shot_height, {19.5, 94}});
        } else if (game_side == 'B') {
          if (direction == Turn_Direction::LEFT) flag_position = {-27.5, 94}; // shot_queue.push_back({shot_height, {-27.5, 94}});
          else if (direction == Turn_Direction::RIGHT) flag_position = {19.5, 24}; //shot_queue.push_back({shot_height, {19.5, 94}});
        }
        break;
    }

    shot_queue.push_back({shot_height, flag_position, turning});
  }

  if (trigger_shot) trigger_shot_queue();
}

void shot_queue_handle(void* param) {
  Field_Position temp_field_pos = field_position;
  for (int i = 0; i < shot_queue.size(); i++) {
    Shot_Target temp_target = shot_queue[i];

    if (temp_target.turning) {
      if (i == 0) {
        drive.set_power(0, 10, 0);
			  pros::delay(150);
        pos.reset(0, 0, 0);
        drive.set_vel(0);
        pros::delay(20);
        printf("target: x: %f, y: %f\n", temp_target.flag_position.x, temp_target.flag_position.y);
        drive_turn_side(PointAngleTarget(temp_target.flag_position), (200/60_deg), 0, false);
      } else if (i == 1) {
        drive_turn_sync(PointAngleTarget(temp_target.flag_position));
      }
    }
    angler.move_to(temp_target.angler_target);
    angler.wait_for_target_reach();
    puncher.shoot();
    puncher.wait_for_shot_finish();
  }
  shot_queue_handle_task.stop_task();
}

void shot_task_cleanup() {
  if (puncher.shooting()) puncher.cancel_shot();
  shot_queue.clear();
  angler.move_to(Angler::PICKUP_POSITION);
  intake.intake();
}
