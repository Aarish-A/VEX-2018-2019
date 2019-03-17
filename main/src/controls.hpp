#pragma once
#include "main.h"

constexpr int GET_BTN_INDEX(pros::controller_digital_e_t btn) { return (btn - pros::E_CONTROLLER_DIGITAL_L1); }

void controls_init();
extern bool partner_connected;

/* Drive */
constexpr pros::controller_analog_e_t JOY_DRIVE_THROTTLE = pros::E_CONTROLLER_ANALOG_LEFT_Y;
constexpr pros::controller_analog_e_t JOY_DRIVE_TURN = pros::E_CONTROLLER_ANALOG_LEFT_X;
constexpr pros::controller_analog_e_t JOY_DRIVE_STRAFE = pros::E_CONTROLLER_ANALOG_RIGHT_X;

/* Angler */
constexpr pros::controller_analog_e_t JOY_ANGLER = pros::E_CONTROLLER_ANALOG_RIGHT_Y;

/* Intake */
constexpr int BTN_INTAKE_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_INTAKE_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);

/* Puncher */
constexpr int BTN_SHOT_CANCEL = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);

/* Macros */
constexpr int BTN_GROUND_PICKUP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_CAP_PICKUP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);

/* Capper */
constexpr int BTN_DECAPPER_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_DECAPPER_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);

constexpr int BTN_SHOT_L_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
constexpr int BTN_SHOT_L_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_SHOT_R_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int BTN_SHOT_R_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);


/* Partner Controller Dependent Controls */
/* Decapper */
// extern int BTN_DECAPPER_UP;
// extern int BTN_DECAPPER_DOWN;
extern int BTN_CHANGE_GAME_SIDE;

/* Shooter */
extern int BTN_FIELD_FRONT_M;
extern int BTN_FIELD_FRONT_P;
extern int BTN_FIELD_PF_BACK_RED;
extern int BTN_FIELD_PF_BACK_BLUE;

/* GUI Controls */
constexpr int BTN_ENTER_MENU = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_EXIT_MENU = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);
constexpr int BTN_SAVE = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
constexpr int BTN_NEXT_MENU_SCREEN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);
constexpr int BTN_PREV_MENU_SCREEN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);


constexpr int BTN_INCREMENT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_DECREMENT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr int BTN_PREVIOUS_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_NEXT_SHOT_SLIDER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);
constexpr int BTN_TEST_SHOT_POSITION = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1); //2


constexpr int BTN_PREV_AUTO = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr int BTN_NEXT_AUTO = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);
constexpr int BTN_SWAP_GAME_SIDE_L = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr int BTN_SWAP_GAME_SIDE_R = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
