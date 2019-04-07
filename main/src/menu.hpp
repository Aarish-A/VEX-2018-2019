#pragma once
#include "main.h"
#include "config.hpp"
#include "libraries/util.hpp"
#include "macros/shot_queueing.hpp"
#include <string>

enum class Menu_Screens {
  SHOT_TUNING,
  AUTO_SELECT,
  FLAG_SELECT,
  NUM_OF_ELEMENTS
};

const std::string menu_screen_strings[] = {
  "Shot Tuning",
  "Auto Select",
  "Flag Select"
};

// enum class Menu_Shot_Positions {
//   PF_MID,
//   PF_TOP,
//   FRONT_MID,
//   FRONT_TOP,
//   BACK_MID,
//   BACK_TOP,
//
//   A_MID_FLAG_MID,
//   A_MID_FLAG_TOP,
//   A_FAR_FLAG_MID,
//   A_FAR_FLAG_TOP,
//   NUM_OF_ELEMENTS
// };

enum class SP {
  G_FRONT_TOP,
  G_FRONT_MID,
  G_PLATFORM_TOP,
  G_PLATFORM_MID,
  G_BACK_TOP,
  G_BACK_MID,

  A_BACK_MID_FLAG_TOP,
  A_BACK_MID_FLAG_MID,
  A_BACK_FAR_FLAG_TOP,
  A_BACK_FAR_FLAG_MID,

  NUM_OF_ELEMENTS
};

const std::string menu_shot_position_strings[] = {
  "G FRONT TOP",
  "G FRONT MID",
  "G PF TOP",
  "G PF MID",
  "G BACK TOP",
  "G BACK MID",

  "A BACK MID F TOP",
  "A BACK MID F MID",
  "A BACK FAR F TOP",
  "A BACK FAR F MID",
};

enum class Auto_Routines {
  FRONT,
  BACK,

  PROGRAMMING_SKILLS,
  DRIVER_SKILLS,
  NUM_OF_ELEMENTS
};

const std::string auto_routine_strings[] = {
  "Front",
  "Back",
  "Programming Skills",
  "Driver Skills"
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
