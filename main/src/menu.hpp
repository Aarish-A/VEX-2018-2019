#pragma once
#include "main.h"
#include "config.hpp"
#include "libraries/util.hpp"
#include "macros/shot_queueing.hpp"
#include <string>
#include "subsystems/drive.hpp"

enum class Menu_Screens {
  SHOT_TUNING,
  AUTO_SELECT,
  FLAG_SELECT,
  TURN_CURVE,
  NUM_OF_ELEMENTS
};

const std::string menu_screen_strings[] = {
  "Shot Tuning",
  "Auto Select",
  "Flag Select",
  "Turn Curve"
};

enum SP {
  G_FRONT_TOP,
  G_FRONT_MID,
  G_FRONT_ANGLE_TOP,
  G_FRONT_ANGLE_MID,
  G_PLATFORM_TOP,
  G_PLATFORM_MID,
  G_PLATFORM_MID_FAR,
  G_PLATFORM_TOP_FAR,
  G_BACK_TURN_TOP,
  G_BACK_TURN_MID,
  G_BACK_TOP,
  G_BACK_MID,

  S_FRONT_TOP,
  S_FRONT_MID,
  S_BACK_TOP,
  S_BACK_MID,
  S_BACK_BOT,
  S_FINAL_LOW,
  S_FINAL_MID,

  DS_BACK_TOP,
  DS_BACK_MID,
  DS_FINAL_TOP,
  DS_FINAL_MID,

  A_SHOT_1,
  A_SHOT_2,
  A_SHOT_3,
  A_SHOT_4,

  NUM_OF_ELEMENTS
};

const std::string menu_shot_position_strings[] = {
  "G FRONT TOP",
  "G FRONT MID",
  "G FRONT ANG TOP",
  "G FRONT ANG MID",
  "G PF TOP",
  "G PF MID",
  "G PF MID FAR",
  "G PF TOP FAR",
  "G BACK TURN TOP",
  "G BACK TURN MID",
  "G BACK TOP",
  "G BACK MID",

  "S FRONT TOP",
  "S FRONT MID",
  "S BACK TOP",
  "S BACK MID",
  "S BACK BOT",
  "S FINAL LOW",
  "S FINAL MID",

  "DS BACK TOP",
  "DS BACK MID",
  "DS FINAL TOP",
  "DS FINAL MID",

  "A_SHOT_1",
  "A_SHOT_2",
  "A_SHOT_3",
  "A_SHOT_4"
};

enum class Auto_Routines {
  FRONT,
  BACK,
  FRONT_PARK,
  PROGRAMMING_SKILLS,
  DRIVER_SKILLS,
  NUM_OF_ELEMENTS
};

const std::string auto_routine_strings[] = {
  "Front",
  "Back",
  "Front Park",
  "Prog. Skills",
  "Driv. Skills"
};

enum class Flags {
  LEFT_TOP,
  LEFT_MID,
  MID_TOP,
  MID_MID,
  RIGHT_TOP,
  RIGHT_MID,
  NO_FLAG,
  NUM_OF_ELEMENTS
};

extern bool menu_enabled;
extern char game_side;

extern Menu_Screens menu_screen;
extern SP menu_shot_position;
extern uint8_t shot_positions[(int)SP::NUM_OF_ELEMENTS];

extern Auto_Routines auto_routine;
extern std::string flag_set[3][2];
extern uint8_t flag_shot_counter;
extern vector flag_select_index;
constexpr uint8_t MAX_NUMBER_OF_SHOTS = 5;
extern Flags flag_config[MAX_NUMBER_OF_SHOTS];

void menu_init();
void menu_update();

void toggle_menu();
void menu_next_screen();
void menu_previous_screen();
void menu_next_element();
void menu_previous_element();
void menu_element_increment_action();
void menu_element_decrement_action();
void menu_selected_action();
void menu_clear();
void menu_save();

void update_flag_set(bool clear = false);

void write_to_file(const char* file_name, const char* file_mode, const char* format, ...);
void read_from_file(const char* file_name, const char* file_mode, const char* format, ...);
