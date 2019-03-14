#pragma once
#include "main.h"
#include "config.hpp"
#include "logs.hpp"

extern pros::Task* auto_update_task;
extern pros::Task* move_alg_task;

void start_auto_update_task();
void stop_auto_update_task();
void stop_move_alg_task();

void auto_update(void* _params);

struct drive_move_params {
  double dist_target = 0;
  double angle_target = 1000;
  bool brake = true;
  uint8_t max_power = 200;
};

void drive_move_async(drive_move_params params);
void drive_move_sync(drive_move_params params);

struct drive_turn_params {
  const AngleTarget& target;
};

void drive_turn_async(drive_turn_params params);
void drive_move_sync(drive_turn_params params);
