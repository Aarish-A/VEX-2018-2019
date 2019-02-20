#pragma once
#include "main.h"
#include "config.hpp"
/*
E_CONTROLLER_DIGITAL_L1 = 6 -- btn = 0
  E_CONTROLLER_DIGITAL_L2, 1
  E_CONTROLLER_DIGITAL_R1, 2
  E_CONTROLLER_DIGITAL_R2, 3
  E_CONTROLLER_DIGITAL_UP, 4
  E_CONTROLLER_DIGITAL_DOWN,5
  E_CONTROLLER_DIGITAL_LEFT, 6
  E_CONTROLLER_DIGITAL_RIGHT, 7
  E_CONTROLLER_DIGITAL_X, 8
  E_CONTROLLER_DIGITAL_B, 9
  E_CONTROLLER_DIGITAL_Y, 10
  E_CONTROLLER_DIGITAL_A 11
  */

constexpr int GET_BTN_INDEX(pros::controller_digital_e_t btn) { return (btn - pros::E_CONTROLLER_DIGITAL_L1); }
void controls_init();
extern bool partner_connected;


/* Constant Controls */
/* Intake */
constexpr int BTN_INTAKE_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_INTAKE_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);

/* Angler */
constexpr int BTN_ANGLER_PU = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr int BTN_ANGLER_CAP_PU = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr pros::controller_analog_e_t JOY_ANGLER = pros::E_CONTROLLER_ANALOG_RIGHT_Y;

/* Drive */
constexpr pros::controller_analog_e_t JOY_DRIVE_FW = pros::E_CONTROLLER_ANALOG_LEFT_Y;
constexpr pros::controller_analog_e_t JOY_DRIVE_TURN = pros::E_CONTROLLER_ANALOG_LEFT_X;
constexpr pros::controller_analog_e_t JOY_DRIVE_STRAFE = pros::E_CONTROLLER_ANALOG_RIGHT_X;

/* Shooter */
constexpr int BTN_SHOOT_CANCEL = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);
constexpr int BTN_SHOT_L_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
constexpr int BTN_SHOT_L_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_SHOT_R_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int BTN_SHOT_R_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);


/* Partner Controller Dependent Controls */
/* Decapper */
extern int BTN_DECAPPER_UP;
extern int BTN_DECAPPER_DOWN;
extern int BTN_CHANGE_GAME_SIDE;

/* Shooter */
extern int BTN_FIELD_FRONT_M;
extern int BTN_FIELD_FRONT_P;
extern int BTN_FIELD_PF_BACK_RED;
extern int BTN_FIELD_PF_BACK_BLUE;

/* GUI Controls */
constexpr int BTN_INCREMENT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_DECREMENT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr int BTN_PREVIOUS_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_NEXT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN); //5
constexpr int BTN_SAVE_SHOT_POSITIONS = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
constexpr int BTN_TEST_SHOT_POSITION = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1); //2
constexpr int BTN_EXIT_GUI = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
