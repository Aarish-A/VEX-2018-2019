#pragma once
#include "main.h"
#include "../config.hpp"
#include "../libraries/logs.hpp"
#include "../libraries/task.hpp"
#include "../libraries/util.hpp"
#include "shot_queueing.hpp"
#include "../subsystems/drive.hpp"

constexpr double MECANUM_DRIVE_WIDTH = 13.20;
constexpr double EDGE_TO_TRACKING_WHEEL = 1.25;
constexpr double DRIVE_EDGE_TO_MECANUM = 2.50;

extern pilons::Task auto_update_task;
extern pilons::Task move_alg_task;
extern pilons::Task cap_on_pole_task;

void start_auto_update_task();
void stop_auto_update_task();
void stop_move_alg_task();

void auto_update(void* _params);

/* Shots */
void single_shot(double targ, bool wait = true);
void double_shot(double targ1, double targ2, bool wait = true);

/* Capping */
void cap_on_pole();
void cap_on_pole_task_function(void* _param);
void cap_on_pole_stop_function();

/* Drive */
void climb_on_platform();

struct drive_move_params {
  double dist_target = 0;
  double angle_target = 1000;
  bool brake = true;
  uint8_t max_power = 200;
  int8_t start_power = 0;
  bool decel = true;
};
extern drive_move_params drive_move_param;

void drive_move(void* _params);
void drive_move_async(double dist_target, double angle_target = 1000, bool brake = true, uint8_t max_power = 200, int8_t start_power = 0, bool decel = true);
void drive_move_sync(double dist_target, double angle_target = 1000, bool brake = true, uint8_t max_power = 200, int8_t start_power = 0, bool decel = true);

struct drive_turn_params {
  const AngleTarget& target;
};
extern drive_turn_params* drive_turn_param;

extern FixedAngleTarget fixed_angle_target;
extern PointAngleTarget point_angle_target;
extern bool fixed_target;

void drive_turn(void* _params);
// void drive_turn_async(const AngleTarget& target);
// void drive_turn_sync(const AngleTarget& target);
void drive_turn_async(FixedAngleTarget target);
void drive_turn_sync(FixedAngleTarget target);
void drive_turn_async(PointAngleTarget target);
void drive_turn_sync(PointAngleTarget target);

void sweep_turn(const AngleTarget& target, double radius, bool forwards, double post_distance = 8_in, bool clockwise = true, bool brake = true, int max_power = 200);
void drive_turn_side(const AngleTarget& target, double kP, double offset, bool forwards);

void auto_update_task_stop_function();
void drive_task_stop_function();
