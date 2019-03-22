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
  PLATFORM_MID,
  PLATFORM_TOP,
  FRONT_MID,
  FRONT_TOP,
  BACK_MID,
  BACK_TOP,

  AUTO_BACK_MID_MID,
  AUTO_BACK_MID_TOP,
  AUTO_BACK_FAR_MID,
  AUTO_BACK_FAR_TOP,
  NUM_OF_ELEMENTS
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

void next_menu_screen();
void previous_menu_screen();
void next_menu_element();
void previous_menu_element();

void write_to_file(const char* file_name, const char* file_mode, bool close_at_end, const char* format, ...);
void read_from_file(const char* file_name, const char* file_mode, bool close_at_end, const char* format, ...);
