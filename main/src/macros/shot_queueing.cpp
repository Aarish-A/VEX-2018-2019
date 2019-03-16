#include "shot_queueing.hpp"

char game_side = 'R';

pilons::Task shot_queue_handle_task("Shot Request Handle", shot_queue_handle, shot_task_cleanup);

volatile Shot_Pos front_SP = {0, 0, 0};
volatile Shot_Pos platform_SP = {0, 0, 0};
volatile Shot_Pos back_SP = {0, 0, 0};

volatile Shot_Pos auto_back_mid_SP = {0, 0, 0};
volatile Shot_Pos auto_back_far_SP = {0, 0, 0};

Field_Position field_position = Field_Position::FRONT;
std::deque<Shot_Target> shot_queue;

void trigger_shot_queue() {
  if (!shot_queue_handle_task.running()) shot_queue_handle_task.start_task();
}

void make_shot_request(uint8_t shot_height, Turn_Direction direction, bool trigger_shot) {
  if (shot_queue.size() == 2) shot_queue.pop_back();
  switch(field_position) {
    case Field_Position::FRONT:
      if (direction == Turn_Direction::STRAIGHT) shot_queue.push_back({shot_height});
      break;
    case Field_Position::RED_PF:
      if (game_side == 'R') {
        if (direction == Turn_Direction::LEFT) shot_queue.push_back({shot_height, {-28 - FLAG_WIDTH, 94}});
        else if (direction == Turn_Direction::RIGHT) shot_queue.push_back({shot_height, {22 - FLAG_WIDTH, 94}});
      } else if (game_side == 'B') {
        if (direction == Turn_Direction::LEFT) shot_queue.push_back({shot_height, {-31, 94}});
        else if (direction == Turn_Direction::RIGHT) shot_queue.push_back({shot_height, {18, 94}});
      }
      break;
    case Field_Position::BLUE_PF:
      if (game_side == 'R') {
        if (direction == Turn_Direction::LEFT) shot_queue.push_back({shot_height, {-24 - FLAG_WIDTH, 94}});
        else if (direction == Turn_Direction::RIGHT) shot_queue.push_back({shot_height, {25 - FLAG_WIDTH, 94}});
      } else if (game_side == 'B') {
        if (direction == Turn_Direction::LEFT) shot_queue.push_back({shot_height, {-27, 94}});
        else if (direction == Turn_Direction::RIGHT) shot_queue.push_back({shot_height, {22, 94}});
      }
      break;
    case Field_Position::BACK:
      if (direction == Turn_Direction::STRAIGHT) shot_queue.push_back({shot_height});
      break;
  }

  if (trigger_shot) trigger_shot_queue();
}

void shot_queue_handle(void* param) {
  for (int i = 0; i < shot_queue.size(); i++) {
    Shot_Target temp_target = shot_queue[i];

    if (field_position != Field_Position::FRONT) {
      // Move to flag... this is going to be asynchronous
    }
    angler.move_to(temp_target.angler_target);
    angler.wait_for_target_reach();
    puncher.shoot();
    puncher.wait_for_shot_finish();
  }
  shot_queue_handle_task.stop_task();
}

void shot_task_cleanup() {
  puncher.cancel_shot();
  shot_queue.clear();
  angler.move_to(Angler::PICKUP_POSITION);
  intake.intake();
}
