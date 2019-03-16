#include "gui.hpp"

template<class T> T operator++ (T enum_type, int) {
  return static_cast<T>(enum_type + 1);
}

char game_side = 'R';
Menu_Screens menu_screen = Menu_Screens::SHOT_TUNING;
Menu_Shot_Positions menu_shot_position = Menu_Shot_Positions::PLATFORM_MID;
Auto_Routines auto_routine = Auto_Routines::FRONT;
