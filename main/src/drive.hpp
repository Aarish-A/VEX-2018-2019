#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "shot_select.hpp"

constexpr int DRIVE_TURN_DZ = 30;
constexpr int DRIVE_DZ = 10;

void drive_set(int x, int y, int a);
void drive_set(int pow);
void drive_set_vel(int x, int y, int a);
void sdrive_set_vel(int vel);
void drive_init();
void drive_handle();
