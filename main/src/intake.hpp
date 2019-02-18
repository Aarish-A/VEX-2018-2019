#pragma once
#include "main.h"
#include "config.hpp"
#include "button.hpp"
#include "util.hpp"
#include "logs.hpp"
#include "controls.hpp"

enum class IntakeState { Forw, Back, Off, Jam };

extern IntakeState intake_state;

void intake_init();
void intake_power_state_set(int power, IntakeState state);
void intake_state_set(IntakeState state);
void intake_set(int power);
void intake_handle();

extern bool intake_up_flag;
extern bool intake_down_flag;
extern bool intake_off_flag;
