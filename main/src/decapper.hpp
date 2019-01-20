#pragma once
#include "main.h"
#include "config.hpp"
#include "controls.hpp"
#include "button.hpp"
#include "auto.hpp"
#include "tracking.hpp"

constexpr double DECAPPER_PICKUP = 90;
constexpr double DECAPPER_CAPPING = 1150;
constexpr double DECAPPER_CAPPING_HOLD = 1100;
constexpr double DECAPPER_DECAPLOW = 1300;
constexpr double DECAPPER_DECAPPING = (DECAPPER_DECAPLOW-120);
enum class Decapper_States {
  Idle,
  Lowering,
  Pickup,
  Capping,
  Capping_Hold,
  Decap_Low,
  Decapping
};
extern pros::Task *decapper_cap_task;
extern Decapper_States decapper_state;
extern Decapper_States decapper_state_last;
extern int decapper_state_change_time;

void set_decapper_state(Decapper_States state);
void decapper_set(int power);
void decapper_move(double pos, int32_t velocity = 200);
void decapper_cal();
void decapper_cap(void *param);
void decapper_stop_cap_task(bool stop_motor = true);
void decapper_start_cap_task();
void decapper_handle();
