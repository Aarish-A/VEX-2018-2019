#pragma once
#include "main.h"
#include "config.hpp"
#include "util.hpp"
#include "auto.hpp"
#include "shot_select.hpp"
#include "logs.hpp"
#include "controls.hpp"
#include "decapper.hpp"

constexpr int DRIVE_TURN_DZ = 30;
constexpr int DRIVE_DZ = 10;

void drive_set(int x, int y, int a);
void drive_set(int pow);
void drive_set_vel(int x, int y, int a);
void drive_set_vel(int vel);
void drive_init();
void drive_handle();

void drive_shot_turn();

const int drive_brake_pow = 0;

static bool handling_shot_req = false;

constexpr int DRIVE_BRAKE_TIME = 300;
