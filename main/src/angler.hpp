#pragma once
#include "main.h"
#include "config.hpp"

constexpr double ANGLER_BALL = 13;

void angler_init();
void angler_set(int power);
void angler_move(double position, int32_t velocity = 200);
void angler_cal();
void angler_handle();
