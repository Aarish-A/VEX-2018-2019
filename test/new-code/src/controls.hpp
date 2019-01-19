#pragma once
#include "main.h"

constexpr int GET_BTN_INDEX(pros::controller_digital_e_t btn) { return (btn - pros::E_CONTROLLER_DIGITAL_L1); }

constexpr int DP_R1 = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int DP_L1 = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);

constexpr int SP_A = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
constexpr int FE_X = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_X);
