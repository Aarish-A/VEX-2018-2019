#pragma once
#include "main.h"

constexpr int GET_BTN_INDEX(pros::controller_digital_e_t btn) { return (btn - pros::E_CONTROLLER_DIGITAL_L1); }

/* Intake */
constexpr int BTN_INTAKE_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_INTAKE_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);

/* Decapper */
constexpr int BTN_DECAPPER_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_X);
constexpr int BTN_DECAPPER_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);

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
// constexpr int BTN_SHOOT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B); //DELETE!
constexpr int BTN_SHOOT_CANCEL = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);
//
// /* Shooter Pos */
constexpr int BTN_FIELD_FRONT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_Y);
constexpr int BTN_FIELD_PF_BACK_RED = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_Y);
constexpr int BTN_FIELD_PF_BACK_BLUE = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
constexpr int BTN_FIELD_BACK = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);
//
// /* Shooter Queue Partner */
constexpr int BTN_SHOT_L_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
constexpr int BTN_SHOT_L_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_SHOT_R_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int BTN_SHOT_R_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);

/* GUI Controls */
constexpr int BTN_INCREMENT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_DECREMENT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr int BTN_PREVIOUS_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_NEXT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);
constexpr int BTN_SAVE_SHOT_POSITIONS = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
constexpr int BTN_TEST_SHOT_POSITION = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int BTN_EXIT_GUI = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
