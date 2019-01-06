#pragma once
#include "main.h"
#include "config.hpp"

enum class IntakeState { Forw, Back, Off, SpitOut};

extern IntakeState intake_state;

void intake_init();
void intake_set(int power);
void intake_handle();
