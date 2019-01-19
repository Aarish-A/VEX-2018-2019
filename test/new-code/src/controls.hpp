#pragma once
#include "main.h"
#include "button.hpp"

constexpr int GET_BTN_INDEX(pros::controller_digital_e_t btn) { return (btn - pros::E_CONTROLLER_DIGITAL_L1); }

/* Intake */
constexpr int BTN_INTAKE_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_INTAKE_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);

// /* Angler */
constexpr int BTN_ANGLER_PU = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr pros::controller_analog_e_t JOY_ANGLER = pros::E_CONTROLLER_ANALOG_RIGHT_Y;
constexpr int BTN_ANGLER_CAP_PU = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
//
// /* Drive */
constexpr pros::controller_analog_e_t JOY_DRIVE_FW = pros::E_CONTROLLER_ANALOG_LEFT_Y;
constexpr pros::controller_analog_e_t JOY_DRIVE_TURN = pros::E_CONTROLLER_ANALOG_LEFT_X;
constexpr pros::controller_analog_e_t JOY_DRIVE_STRAFE = pros::E_CONTROLLER_ANALOG_RIGHT_X;
//
// /* Shooter */
constexpr int BTN_SHOOT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B); //DELETE!
constexpr int BTN_SHOOT_CANCEL = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
//
// /* Shooter Pos */
constexpr int BTN_FIELD_FRONT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_X);
constexpr int BTN_FIELD_PF_Back = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_Y);
constexpr int BTN_FIELD_BACK = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);
//
// /* Shooter Queue */
constexpr int BTN_SHOT_L_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
constexpr int BTN_SHOT_L_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_SHOT_R_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int BTN_SHOT_R_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);
