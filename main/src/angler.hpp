#pragma once
#include "main.h"
#include "util.hpp"
#include "config.hpp"
#include "puncher.hpp"
#include "shot_select.hpp"

constexpr double ANGLER_BALL = 13;
constexpr int ANGLER_DZ = 30;

constexpr double ANGLER_PU_POS = 85;//ANGLER_BALL * 7;

constexpr double ANGLER_CAP_PU_POS = 215;

void angler_init();
void angler_set(int power);
void angler_move(double position, int32_t velocity = 200);
void angler_cal();
void angler_handle();
