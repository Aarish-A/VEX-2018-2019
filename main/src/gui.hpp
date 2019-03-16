#pragma once
#include "main.h"
#include "config.hpp"
#include "macros/shot_queueing.hpp"

template<class T> T operator++ (T enum_type, int);

enum class Menu_Screens {
  SHOT_TUNING,
  AUTO_SELECT,
  NUM_MENU_SCREENS
};
std::string menu_screen_strings[(int)Menu_Screens::NUM_MENU_SCREENS];

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
  NUM_MENU_SHOT_POSITIONS
};
std::string menu_shot_positions[(int)Menu_Shot_Positions::NUM_MENU_SHOT_POSITIONS];

enum class Auto_Routines {
  FRONT,
  BACK,
  NUM_AUTO_ROUTINES
};

extern char game_side;
extern Menu_Screens menu_screen;
extern Menu_Shot_Positions menu_shot_position;
extern Auto_Routines auto_routine;
