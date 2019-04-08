#pragma once

#include "main.h"
#include "../config.hpp"
#include "../menu.hpp"
#include "../libraries/task.hpp"
#include "../libraries/util.hpp"
#include "auto.hpp"
#include <deque>

constexpr double FLAG_WIDTH = 12.0;

extern pilons::Task shot_queue_handle_task;

struct Shot_Pos {
  uint8_t top;
  uint8_t mid;
  uint8_t bot;
};

extern volatile Shot_Pos front_SP;
extern volatile Shot_Pos platform_SP;
extern volatile Shot_Pos back_SP;

extern volatile Shot_Pos auto_back_mid_SP;
extern volatile Shot_Pos auto_back_far_SP;


// enum Shot_Positions {
//   FRONT_TOP_SP,
//   FRONT_MID_SP,
//   PF_TOP_SP,
//   PF_MID_SP,
//   BACK_TOP_SP,
//   BACK_MID_SP,
//
//   A_BACK_MID_TOP_SP,
//   A_BACK_MID_MID_SP,
//   A_BACK_FAR_TOP_SP,
//   A_BACK_FAR_BOT_SP
// };

enum class Turn_Direction {
  LEFT,
  STRAIGHT,
  RIGHT
};

enum class Field_Position {
  FRONT,
  RED_PF,
  BLUE_PF,
  BACK
};

extern Field_Position field_position;

struct Shot_Target {
  uint8_t angler_target;
  vector flag_position = {0, 0};
  bool turning = false;
};

extern std::deque<Shot_Target> shot_queue;

void trigger_shot_queue();
void make_shot_request(uint8_t shot_height, Turn_Direction direction, Field_Position target_field_pos, bool trigger_shot = false);
void change_field_position(Field_Position new_field_pos);
void shot_queue_handle(void* param);
void shot_task_cleanup();
