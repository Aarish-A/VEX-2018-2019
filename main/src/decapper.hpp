#pragma once
#include "main.h"
#include "config.hpp"
constexpr double DECAPPER_PICKUP = 90;
constexpr double DECAPPER_CAPPING = 730;
constexpr double DECAPPER_DECAPLOW = 1200;
constexpr double DECAPPER_DECAPPING = (DECAPPER_DECAPLOW-120);
enum class Decapper_States {Idle, Lowering, Pickup, Capping, Decap_Low, Decapping};

extern Decapper_States decapper_state;
extern Decapper_States decapper_state_last;
extern int decapper_state_change_time;

void set_decapper_state(Decapper_States state);
void decapper_set(int power);
void decapper_move(double pos, int32_t velocity = 200);
void decapper_reset();
void decapper_handle();
