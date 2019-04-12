#include "menu.hpp"

bool menu_enabled = false;
char game_side = 'R';
Menu_Screens menu_screen = Menu_Screens::SHOT_TUNING;

SP menu_shot_position = SP::G_PLATFORM_TOP;
uint8_t shot_positions[(int)SP::NUM_OF_ELEMENTS];

Auto_Routines auto_routine = Auto_Routines::FRONT;
std::string flag_set[3][2] = { {"> 0", "  0"}, {"  0", "  0"}, {"  0", "  0"} };
uint8_t flag_shot_counter = 0;
vector flag_select_index = {0, 0};
Flags flag_config[MAX_NUMBER_OF_SHOTS] = {Flags::NO_FLAG, Flags::NO_FLAG, Flags::NO_FLAG, Flags::NO_FLAG, Flags::NO_FLAG};

void menu_init() {

  uint32_t temp = 0;
  read_from_file("/usd/auto_routine.txt", "r", "%d ", &temp);
  auto_routine = static_cast<Auto_Routines>(temp);


  char temp2 = 'R';
  read_from_file("/usd/game_side.txt", "r", &temp2);
  game_side = temp2;

  FILE* flag_config_file = nullptr;
  flag_config_file = fopen("/usd/flag_config.txt", "r");
  for (int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
    if (flag_config_file != nullptr) {
      int32_t temp = -1;
      fscanf(flag_config_file, "%d ", &temp);
      flag_config[i] = static_cast<Flags>(temp);

      int8_t index1 = -1;
      int8_t index2 = -1;
      if (temp == 0) { index1 = 0; index2 = 0; }
      else if (temp == 1) { index1 = 0; index2 = 1; }
      else if (temp == 2) { index1 = 1; index2 = 0; }
      else if (temp == 3) { index1 = 1; index2 = 1; }
      else if (temp == 4) { index1 = 2; index2 = 0; }
      else if (temp == 5) { index1 = 2; index2 = 1; }

      if (index1 != -1 && index2 != -1) flag_set[index1][index2][2] = (i + 1) + '0';
      log_ln(IO, "%d Successfully read %d from /usd/flag_config.txt", pros::millis(),temp);
    } else log_ln(IO, "%d Could not read from /usd/flag_config.txt",pros::millis());
  }
  if (flag_config_file != nullptr) fclose(flag_config_file);

  FILE* shot_positions_file = nullptr;
  shot_positions_file = fopen("/usd/shot_positions.txt", "r");
  for (int i = 0; i < (int)SP::NUM_OF_ELEMENTS; i++) {
    if (shot_positions_file != nullptr) {
      uint32_t temp;
      fscanf(shot_positions_file, "%d ", &temp);
      shot_positions[i] = temp;
      log_ln(IO, "Successfully read %d from /usd/shot_positions.txt", temp);
    } else log_ln(IO, "Could not read from /usd/shot_positions.txt", pros::millis());
  }
  if (shot_positions_file != nullptr) fclose(shot_positions_file);
  else for(int i = 0; i < (int)SP::NUM_OF_ELEMENTS; i++) shot_positions[i] = 0;
}

void menu_update() {
  master.write_line(0, menu_screen_strings[(int)menu_screen].c_str());

  // printf("------------------------------\n");
  // printf("FC: ");
  // for(int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
  //   printf("%d ", static_cast<int>(flag_config[i]));
  //   if (i == MAX_NUMBER_OF_SHOTS - 1) printf("\n");
  // }
  //
  // printf("SP: ");
  // for (int i = 0; i < static_cast<int>(SP::NUM_OF_ELEMENTS); i++) {
  //   printf("%d ", shot_positions[i]);
  //   if (i == static_cast<int>(SP::NUM_OF_ELEMENTS) - 1) printf("\n");
  // }
  //
  // printf("GS: %c\n", game_side);
  // printf("AR: %d\n", (int)auto_routine);

  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      master.write_line(1, menu_shot_position_strings[(int)menu_shot_position].c_str());
      master.write_line(2, "Val: %d", shot_positions[(int)menu_shot_position]);
      break;
    case Menu_Screens::AUTO_SELECT:
      master.write_line(1, auto_routine_strings[(int)auto_routine].c_str());
      master.write_line(2, "Side: %c", game_side);
      break;
    case Menu_Screens::FLAG_SELECT:
      master.write_line(1, "%s   %s   %s", flag_set[0][0].c_str(), flag_set[1][0].c_str(), flag_set[2][0].c_str());
      master.write_line(2, "%s   %s   %s", flag_set[0][1].c_str(), flag_set[1][1].c_str(), flag_set[2][1].c_str());
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
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

void menu_next_element() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      menu_shot_position = next_enum_value(menu_shot_position);
      break;
    case Menu_Screens::AUTO_SELECT:
      auto_routine = next_enum_value(auto_routine);
      break;
    case Menu_Screens::FLAG_SELECT:
      if (flag_select_index.y == 0) {
        flag_select_index.y++;
        update_flag_set();
      }
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
    case Menu_Screens::FLAG_SELECT:
      if (flag_select_index.y == 1) {
        flag_select_index.y--;
        update_flag_set();
      }
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_element_increment_action() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      if (shot_positions[(int)menu_shot_position] < 250) shot_positions[(int)menu_shot_position] += 5;
      break;
    case Menu_Screens::AUTO_SELECT:
      break;
    case Menu_Screens::FLAG_SELECT:
      if (flag_select_index.x < 2) {
        flag_select_index.x++;
        update_flag_set();
      }
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_element_decrement_action() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      if (shot_positions[(int)menu_shot_position] > 0) shot_positions[(int)menu_shot_position] -= 5;
      break;
    case Menu_Screens::AUTO_SELECT:
      break;
    case Menu_Screens::FLAG_SELECT:
      if (flag_select_index.x > 0) {
        flag_select_index.x--;
        update_flag_set();
      }
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_selected_action() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      break;
    case Menu_Screens::AUTO_SELECT:
      game_side == 'R' ? game_side = 'B' : game_side = 'R';
      write_to_file("/usd/game_side.txt", "w", "%c", game_side);
      break;
    case Menu_Screens::FLAG_SELECT:
      if (flag_set[(int)flag_select_index.x][(int)flag_select_index.y][2] == '0' && flag_shot_counter < MAX_NUMBER_OF_SHOTS) {
        flag_shot_counter++;
        flag_set[(int)flag_select_index.x][(int)flag_select_index.y][2] = flag_shot_counter + '0';
      }
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_clear() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING:
      break;
    case Menu_Screens::AUTO_SELECT:
      break;
    case Menu_Screens::FLAG_SELECT:
      flag_shot_counter = 0;
      update_flag_set(true);
      break;
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void menu_save() {
  switch(menu_screen) {
    case Menu_Screens::SHOT_TUNING: {
      FILE* shot_positions_file = nullptr;
      shot_positions_file = fopen("/usd/shot_positions.txt", "w");
      for (int i = 0; i < (int)SP::NUM_OF_ELEMENTS; i++) {
        if (shot_positions_file != nullptr) {
          fprintf(shot_positions_file, "%d ", shot_positions[i]);
          log_ln(IO, "Successfully wrote %d to /usd/shot_positions.txt", shot_positions[i]);
        } else log_ln(IO, "Could not write to /usd/shot_positions.txt");
      }
      if (shot_positions_file != nullptr) {
        fclose(shot_positions_file);
        master.rumble("--");
      }
      break;
    }
    case Menu_Screens::AUTO_SELECT:
      write_to_file("/usd/auto_routine.txt", "w", "%d", (int)auto_routine);
      break;
    case Menu_Screens::FLAG_SELECT: {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          Flags temp_flag = (Flags)(i * 2 + j);
          if (flag_set[i][j][2] != '0') {
            flag_config[flag_set[i][j][2] - '0' - 1] = temp_flag;
          }
        }
      }
      FILE* flag_config_file = nullptr;
      flag_config_file = fopen("/usd/flag_config.txt", "w");
      for (int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
        if (flag_config_file != nullptr) {
          fprintf(flag_config_file, "%d ", static_cast<int>(flag_config[i]));
          log_ln(IO, "Successfully wrote %d to /usd/flag_config.txt", static_cast<int>(flag_config[i]));
        } else log_ln(IO, "Could not write to /usd/flag_config.txt");
      }
      if (flag_config_file != nullptr) {
        fclose(flag_config_file);
        master.rumble("--");
      }
      break;
    }
    case Menu_Screens::NUM_OF_ELEMENTS:
      break;
  }
}

void update_flag_set(bool clear) {
  for(int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      if (i == (int)flag_select_index.x && j == (int)flag_select_index.y && flag_set[i][j][0] != '>') flag_set[i][j][0] = '>';
      else if (flag_set[i][j][0] == '>') flag_set[i][j][0] = ' ';
      if (clear) flag_set[i][j][2] = '0';
    }
  }
}

void write_to_file(const char* file_name, const char* file_mode, const char* format, ...) {
  FILE* file = nullptr;
  file = fopen(file_name, file_mode);

  va_list args;
  va_start(args, format);

  if (file != nullptr) {
    vfprintf(file, format, args);
    log_ln(IO, "Successfully wrote to %s", file_name);
    fclose(file);
    master.rumble("--");
  } else log_ln(FATAL_ERROR, "Could not write to %s", file_name);
  va_end(args);
}

void read_from_file(const char* file_name, const char* file_mode, const char* format, ...) {
  FILE* file = nullptr;
  file = fopen(file_name, file_mode);

  if (file != nullptr) {
    va_list args;
    va_start(args, format);
    vfscanf(file, format, args);
    log_ln(IO, "Successfully read from %s", file_name);
    fclose(file);
    master.rumble("--");
    va_end(args);
  } else log_ln(FATAL_ERROR, "%d Could not read from %s", pros::millis(),file_name);
}
