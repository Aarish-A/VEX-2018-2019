#include "menu.hpp"

bool menu_enabled = false;
char game_side = 'R';
Menu_Screens menu_screen = Menu_Screens::SHOT_TUNING;
Menu_Shot_Positions menu_shot_position = Menu_Shot_Positions::PF_MID;
Auto_Routines auto_routine = Auto_Routines::FRONT;

void menu_init() {
  uint8_t temp;
  read_from_file("/usd/auto_routine", "r", true, "%d ", &temp);
  auto_routine = (Auto_Routines)temp;

  for (int i = 0; i < (int)Menu_Shot_Positions::NUM_OF_ELEMENTS; i++) {
    if (i < (int)Menu_Shot_Positions::NUM_OF_ELEMENTS - 1) read_from_file("/usd/shot_positions", "a", false, "%d ", &shot_pos[i]);
    else read_from_file("/usd/shot_positions", "a", true, "%d ", &shot_pos[i]);
  }
}

void menu_update() {
  if (menu_enabled) {
    partner.write_line(0, menu_screen_strings[(int)menu_screen].c_str());

    switch(menu_screen) {
      case Menu_Screens::SHOT_TUNING:
        partner.write_line(1, menu_shot_position_strings[(int)menu_shot_position].c_str());
        partner.write_line(2, "Val: %d", shot_pos[(int)menu_shot_position]);
        break;
      case Menu_Screens::AUTO_SELECT:
        partner.write_line(1, auto_routine_strings[(int)auto_routine].c_str());
        break;
      case Menu_Screens::NUM_OF_ELEMENTS:
        break;
    }
  } else {

  }
}

void toggle_menu() {
  menu_enabled = !menu_enabled;
}

void menu_next_screen() {
  menu_screen = next_enum_value(menu_screen);
}

void menu_previous_screen() {
  menu_screen = previous_enum_value(menu_screen);
}

void menu_save() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      for (int i = 0; i < (int)Menu_Shot_Positions::NUM_OF_ELEMENTS; i++) {
        if (i < (int)Menu_Shot_Positions::NUM_OF_ELEMENTS - 1) write_to_file("/usd/shot_positions", "w", false, "%d ", shot_pos[i]);
        else write_to_file("/usd/shot_positions", "w", true, "%d ", shot_pos[i]);
      }
      break;
    case Menu_Screens::AUTO_SELECT:
      write_to_file("/usd/auto_routine", "w", true, "%d", (int)auto_routine);
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_next_element() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      menu_shot_position = next_enum_value(menu_shot_position);
      break;
    case Menu_Screens::AUTO_SELECT:
      auto_routine = next_enum_value(auto_routine);
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_previous_element() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      menu_shot_position = previous_enum_value(menu_shot_position);
      break;
    case Menu_Screens::AUTO_SELECT:
      auto_routine = previous_enum_value(auto_routine);
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_element_increment_action() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      if (shot_pos[(int)menu_shot_position] > 0) shot_pos[(int)menu_shot_position] -= 5;
      break;
    case Menu_Screens::AUTO_SELECT:
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_element_decrement_action() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      if (shot_pos[(int)menu_shot_position] < 250) shot_pos[(int)menu_shot_position] += 5;
      break;
    case Menu_Screens::AUTO_SELECT:
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void write_to_file(const char* file_name, const char* file_mode, bool close_at_end, const char* format, ...) {
  FILE* file = nullptr;
  file = fopen(file_name, file_mode);

  va_list args;
  va_start(args, format);

  if (file != nullptr) {
    vfprintf(file, format, args);
    log_ln(LOG_IO, "Successfully wrote to %s", file_name);
    if (close_at_end) fclose(file);
  } else log_ln(LOG_ERROR, "Could not write to %s", file_name);
  va_end(args);
}


void read_from_file(const char* file_name, const char* file_mode, bool close_at_end, const char* format, ...) {
  FILE* file = nullptr;
  file = fopen(file_name, file_mode);

  va_list args;
  va_start(args, format);

  if (file != nullptr) {
    vfscanf(file, format, args);
    log_ln(LOG_IO, "Successfully read from %s", file_name);
    if (close_at_end) fclose(file);
  } else log_ln(LOG_ERROR, "Could not read from %s", file_name);
  va_end(args);
}
