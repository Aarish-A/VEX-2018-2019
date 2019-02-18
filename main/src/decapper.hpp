#pragma once
#include "main.h"
#include "config.hpp"
#include "controls.hpp"
#include "button.hpp"
#include "auto.hpp"
#include "tracking.hpp"

constexpr double DECAPPER_RATIO = 5.0/1.0;

constexpr double DECAPPER_BOT_POS = 0;
constexpr double DECAPPER_MID_POS = 380;
constexpr double DECAPPER_TOP_POS = 695;

constexpr int DECAPPER_BOT_HOLD_POW = -4;
constexpr int DECAPPER_MID_HOLD_POW =  8;
enum class Decapper_States {
  Bot,
  Mid,
  Top,
  Idle
};
extern pros::Task *decapper_cap_task;
extern Decapper_States decapper_state;
extern Decapper_States decapper_state_last;
extern double decapper_targ;
extern int decapper_state_change_time;

void set_decapper_targ_state(Decapper_States state, double targ);
void set_decapper_pow_state(Decapper_States state, double pow);
void set_decapper_state(Decapper_States state);
void decapper_set(int power);
void decapper_move(double pos, int32_t velocity = 200);
void decapper_cal();
void decapper_cap(void *param);
void decapper_stop_cap_task(bool stop_motor = true);
void decapper_start_cap_task();
void decapper_handle();
