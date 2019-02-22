#pragma once
#include "main.h"
#include "shot_select.hpp"
#include "config.hpp"
#include "button.hpp"
#include <string.h>

// Controller
constexpr uint32_t UPDATE_INTERVAL = 60;
extern char partner_screen_lines[3][16];
extern int update_line;
extern uint32_t update_time;

// Menu
extern bool in_menu;
enum class menu_screens : int {
  shot_tuning,
  auto_select,
  number_of_menu_screens
};
constexpr int num_menu_screens = (int)menu_screens::number_of_menu_screens;
extern menu_screens menu_screen;
extern const char* menu_screen_titles[num_menu_screens];

// Shot Tuning
enum menu_shot_positions {
  front_top,
  front_mid,
  back_top,
  back_mid,
  auto_mid_flag_top,
  auto_mid_flag_mid,
  auto_far_flag_top,
  auto_far_flag_mid,
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

extern int shot_positions[num_shot_positions];
extern const char* menu_shot_strings[num_shot_positions];

// Auto select
enum auto_routines {
  auto_front,
  auto_back,
  auto_skills,
  number_of_auto_routes
};

constexpr int num_auto_routes = (int)number_of_auto_routes;
extern auto_routines menu_auto_route;
extern char game_side;

extern const char* menu_auto_strings[num_auto_routes];
// Functions
void switch_menu_screen(menu_screens menu_screen);
void write_line(int line_num, std::string line);
void gui_init();
void gui_handle();
void read_shot_positions_from_file();
void write_shot_positions_to_file();
void read_auto_route_from_file();
void write_auto_route_to_file();
void read_game_side_from_file();
void swap_game_side();
