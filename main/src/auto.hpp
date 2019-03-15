#pragma once
#include "main.h"
#include "config.hpp"
#include "logs.hpp"
#include "libraries/task.hpp"

extern pilons::Task auto_update_task;
extern pilons::Task move_alg_task;

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

void drive_move(void* _params);
void drive_move_async(drive_move_params params);
void drive_move_sync(drive_move_params params);

struct drive_turn_params {
  const AngleTarget& target;
};

void drive_turn(void* _params);
void drive_turn_async(drive_turn_params params);
void drive_move_sync(drive_turn_params params);

void auto_update_task_stop_function();
void drive_task_stop_function();
