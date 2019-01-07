#pragma once
#include "main.h"
#include "config.hpp"
#include "shot_select.hpp"

enum class PunState { Load, Hold, ShotStart, ShotWait, FatalError };

constexpr double PUN_RATIO = 1.0;
constexpr double PUN_OFFSET = 45 * PUN_RATIO;
constexpr double PUN_HOLD = 180 * PUN_RATIO; // Relative to the slip point
constexpr double PUN_TPR = 360 * PUN_RATIO; // Relative to the slip point
constexpr double PUN_NO_RETURN = 50 * PUN_RATIO; // Back from the slip point
constexpr double PUN_BALL_CHK_START = PUN_TPR - (PUN_HOLD + 5); // Back from the slip point
constexpr uint32_t PUN_WAIT_TIME = 100;
constexpr int PUN_BALL_THRESH = 2200;
constexpr uint32_t PUN_BALL_OFF_TIME = 50;
constexpr int PUN_HOLD_PWR = 12;

extern PunState pun_state;
extern int pun_shots;
extern bool pun_ball;

extern bool auto_set_shot;

void pun_init();
void pun_set(int power);
void pun_move(double position, int32_t velocity = 200);
void pun_cal();
void pun_handle();
void pun_fatal_disable();

extern int shot_num; //Either 0, 1 or 2; Used to keep track of completed shot requests
