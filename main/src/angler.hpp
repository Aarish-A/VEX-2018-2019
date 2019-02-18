#pragma once
#include "main.h"
#include "util.hpp"
#include "config.hpp"
#include "intake.hpp"
#include "logs.hpp"
#include "controls.hpp"
#include "shot_select.hpp"


void angler_init();
void angler_set(int power);
void angler_move(double position, int32_t velocity = 200);
void angler_cal();
void angler_handle();
