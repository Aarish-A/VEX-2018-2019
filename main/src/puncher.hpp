#pragma once
#include "main.h"
#include "config.hpp"
#include "shot_select.hpp"
#include "util.hpp"
#include "logs.hpp"
#include "controls.hpp"

enum class PunState {
  CalA, //0
  CalB, //1
  CalWait, //2
  Loading, //3
  Loaded, //4
  Cancel, //5
  Pull_Back, //6
  Bolt_Wait, //7
  FatalError //8
};

constexpr double PUN_RATIO = 5.0/3.0;
constexpr double PUN_OFFSET = 50 * PUN_RATIO;
constexpr double PUN_HOLD = 180 * PUN_RATIO; // Relative to the slip point
constexpr double PUN_TPR = 360 * PUN_RATIO; // Relative to the slip point
constexpr double PUN_NO_RETURN = 50 * PUN_RATIO; // Back from the slip point
constexpr double PUN_BALL_CHK_START[2] = {PUN_TPR - (PUN_HOLD + 150), PUN_TPR - (PUN_HOLD + 150)}; // Back from the slip point
constexpr uint32_t PUN_WAIT_TIME = 100;
constexpr int PUN_BALL_THRESH = 2700;
constexpr uint32_t PUN_BALL_OFF_TIME = 100;
constexpr int PUN_HOLD_PWR = 5;

constexpr int PUN_LOAD_WAIT_TIME = 150;

extern PunState pun_state;
extern PunState pun_state_last;
extern int pun_state_change_time;

extern int pun_shots;
extern bool pun_ball;

extern bool auto_set_shot;
extern double auto_angler_target;

void pun_init();
void pun_set(int power);
void pun_move(double position, int32_t velocity = 200);
void pun_cal();
void pun_handle();
void pun_fatal_disable();

void pun_state_change(PunState state);

//extern int shot_num; //Either 0, 1 or 2; Used to keep track of completed shot requests
