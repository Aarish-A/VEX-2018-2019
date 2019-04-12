#pragma once
#include "main.h"

constexpr int GET_BTN_INDEX(pros::controller_digital_e_t btn) { return (btn - pros::E_CONTROLLER_DIGITAL_L1); }

void controls_init();
extern bool partner_connected;

/* Drive */
constexpr pros::controller_analog_e_t JOY_DRIVE_THROTTLE = pros::E_CONTROLLER_ANALOG_LEFT_Y;
constexpr pros::controller_analog_e_t JOY_DRIVE_TURN = pros::E_CONTROLLER_ANALOG_LEFT_X;
constexpr pros::controller_analog_e_t JOY_DRIVE_STRAFE = pros::E_CONTROLLER_ANALOG_RIGHT_X;
constexpr uint8_t BTN_DRIVE_LOCK = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);

/* Angler */
constexpr pros::controller_analog_e_t JOY_ANGLER = pros::E_CONTROLLER_ANALOG_RIGHT_Y;

/* Intake */
constexpr int BTN_INTAKE_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr int BTN_INTAKE_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);

/* Puncher */
constexpr int BTN_SHOT_CANCEL = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);
constexpr uint8_t BTN_SHOT_CANCEL_PARTNER = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);

/* Macros */
constexpr int BTN_GROUND_PICKUP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_CAP_PICKUP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);

/* Capper */
constexpr int BTN_SHOT_L_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
constexpr int BTN_SHOT_L_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr int BTN_SHOT_R_T = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr int BTN_SHOT_R_M = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);
constexpr int BTN_FIELD_FRONT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_X);
constexpr int BTN_FIELD_RED_PF = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_Y);
constexpr int BTN_FIELD_BLUE_PF = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
constexpr uint8_t BTN_FIELD_BACK = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);

constexpr int BTN_CAPPER_DOWN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_Y);
constexpr int BTN_CAPPER_UP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_X);
constexpr int BTN_CAP_FLIP = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_B);



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
constexpr uint8_t BTN_MENU_NEXT_SCREEN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R2);
constexpr uint8_t BTN_MENU_PREVIOUS_SCREEN = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L2);
constexpr uint8_t BTN_MENU_NEXT_ELEMENT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_DOWN);
constexpr uint8_t BTN_MENU_PREVIOUS_ELEMENT = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_UP);
constexpr uint8_t BTN_MENU_INCREMENT_ACTION = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_RIGHT);
constexpr uint8_t BTN_MENU_DECREMENT_ACTION = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_LEFT);
constexpr uint8_t BTN_MENU_SELECTED_ACTION = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_R1);
constexpr uint8_t BTN_MENU_CLEAR = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_L1);
constexpr uint8_t BTN_MENU_SAVE = GET_BTN_INDEX(pros::E_CONTROLLER_DIGITAL_A);
