#pragma once
#include "main.h"
#include "config.hpp"
#include "libraries/util.hpp"
#include "macros/shot_queueing.hpp"

enum class Menu_Screens {
  SHOT_TUNING,
  AUTO_SELECT,
  NUM_OF_ELEMENTS
};

const std::string menu_screen_strings[] = {
  "Shot Tuning",
  "Auto Select"
};

enum class Menu_Shot_Positions {
  PF_MID,
  PF_TOP,
  FRONT_MID,
  FRONT_TOP,
  BACK_MID,
  BACK_TOP,

  A_MID_FLAG_MID,
  A_MID_FLAG_TOP,
  A_FAR_FLAG_MID,
  A_FAR_FLAG_TOP,
  NUM_OF_ELEMENTS
};

enum SP {
  PF_MID,
  PF_TOP,
  FRONT_MID,
  FRONT_TOP,
  BACK_MID,
  BACK_TOP,

  A_MID_FLAG_MID,
  A_MID_FLAG_TOP,
  A_FAR_FLAG_MID,
  A_FAR_FLAG_TOP,

  S_BACK_CORNER_MID,
  S_BACK_CORNER_TOP
};

volatile uint8_t shot_pos[] = {
  70, // PF_MID
  90, // PF_TOP
  0, // FRONT_MID,
  98, // FRONT_TOP,
  10, // BACK_MID,
  90, // BACK_TOP,

  0, // A_MID_FLAG_MID,
  88, // A_MID_FLAG_TOP,
  30, // A_FAR_FLAG_MID,
  118, // A_FAR_FLAG_TOP,

  0, // S_BACK_CORNER_MID
  98 // S_BACK_CORNER_TOP
};

const std::string menu_shot_position_strings[] = {
  "Platform Mid",
  "Platform Top",
  "Front Mid",
  "Front Top",
  "Back Mid",
  "Back Top",

  "Auto Back Mid Mid",
  "Auto Back Mid Top",
  "Auto Back Far Mid",
  "Auto Back Far Top",
};

enum class Auto_Routines {
  FRONT,
  BACK,
  NUM_OF_ELEMENTS
};

const std::string auto_routine_strings[] = {
  "Front",
  "Back"
};

extern bool menu_enabled;
extern char game_side;
extern Menu_Screens menu_screen;
extern Menu_Shot_Positions menu_shot_position;
extern Auto_Routines auto_routine;

void menu_init();
void menu_update();

void toggle_menu();

void menu_next_screen();
void menu_previous_screen();
void menu_next_element();
void menu_previous_element();
void menu_element_increment_action();
void menu_element_decrement_action();

void write_to_file(const char* file_name, const char* file_mode, bool close_at_end, const char* format, ...);
void read_from_file(const char* file_name, const char* file_mode, bool close_at_end, const char* format, ...);
