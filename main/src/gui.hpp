#pragma once
#include "main.h"
#include "shot_select.hpp"
#include "config.hpp"
#include "auto.hpp"
#include "puncher.hpp"
#include "button.hpp"
#include <string.h>

extern bool inMenu;
enum class menu_screens : int {
  game_screen,
  shot_tuning,
  number_of_menu_screens
};
constexpr int num_menu_screens = (int)menu_screens::number_of_menu_screens;


enum menu_shot_positions {
  front_top,
  front_mid,
  back_top,
  back_mid,
  auto_top,
  auto_mid,
  skills_front_top,
  skills_front_mid,
  skills_corner_top,
  skills_corner_mid,
  skills_back_top,
  skills_back_mid,
  skills_back_bot,
  number_of_shot_positions
};
constexpr int num_shot_positions = (int)number_of_shot_positions;
extern menu_shot_positions menu_shot_position;
extern const char* menu_shot_strings[num_shot_positions];
extern int shot_positions[num_shot_positions];

extern menu_screens menu_screen;
extern const char* menu_screen_titles[num_menu_screens];
extern char partner_screen_lines[3][16];
extern int update_line;
extern uint32_t update_time;
constexpr uint32_t UPDATE_INTERVAL = 60;

void gui_init();
void gui_handle();
void read_shot_positions_from_file();
void write_shot_positions_to_file();
void write_line(int line_num, std::string line);
