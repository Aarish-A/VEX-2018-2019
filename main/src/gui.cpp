#include "gui.hpp"

// Controller
char partner_screen_lines[3][16] = {"               ", "               ", "               "};
int update_line = 0;
uint32_t update_time = 0;

// Menu
bool in_menu = false;
menu_screens menu_screen = menu_screens::shot_tuning;
const char* menu_screen_titles[num_menu_screens] = {"Shot Tuning", "Auto Select"};

// Shot Tuning
menu_shot_positions menu_shot_position = front_top;
int shot_positions[num_shot_positions] = { pf_SP.top, pf_SP.mid, front_SP.top, front_SP.mid, auto_mid_flag_SP.top, auto_mid_flag_SP.mid,
                                           auto_far_flag_SP.top, auto_far_flag_SP.mid, skills_front_SP.top, skills_front_SP.mid, skills_corner_SP.top,
                                           skills_corner_SP.mid, skills_back_SP.top, skills_back_SP.mid, skills_back_SP.bot };
const char* menu_shot_strings[num_shot_positions] = {"BackTop <<<<", "BackMid <<<<", "FrontTop", "FrontMid", "AutoMidFlagTop", "AutoMidFlagMid",
                                                     "AutoFarFlagTop", "AutoFarFlapMid", "SkillFrTop", "SkillFrMid", "SkillCorTop",
                                                     "SkillCorMid", "SkillBackTop", "SkillBackMid", "SkillsBackBot" };

// Auto Select
auto_routines menu_auto_route = auto_front;
char game_side = 'R';
const char* menu_auto_strings[num_auto_routes] = {"Front Auto", "Back Auto", "Auto Skills"};

void write_line(int line_num, std::string line) {
  while(line.size() < 15) {
     line += " ";
   }
   strcpy(partner_screen_lines[line_num], line.c_str());
}

void gui_init() {
  if (partner_connected) {
    partner.print(0, 0, "               ");
    pros::delay(60);
    partner.print(1, 0, "               ");
    pros::delay(60);
    partner.print(2, 0, "               ");
  }

  read_shot_positions_from_file();
  read_auto_route_from_file();
  read_game_side_from_file();
  update_time = pros::millis() + UPDATE_INTERVAL;
}

void gui_handle() {
  if (pros::millis() >= update_time) {
    partner.print(update_line, 0, partner_screen_lines[update_line]);
    update_time = pros::millis() + UPDATE_INTERVAL;
    if (update_line < 2) update_line++;
    else update_line = 0;

    // Update Line 0
    if (!in_menu) write_line(0, " ");
    else write_line(0, menu_screen_titles[(int)menu_screen]);

    // Update Line 2

  }

  if (!in_menu) {
    if (check_single_press(BTN_ENTER_MENU, true)) {
      in_menu = true;
      menu_screen = menu_screens::shot_tuning;
      menu_shot_position = back_top;
    }

    std::string field_pos_s = "default";
    FieldPos field_pos = shot_req[0].field_pos;
    if (field_pos == FieldPos_Front) field_pos_s = "Front";
    else if (field_pos == FieldPos_Back) field_pos_s = "Back";
    else if (field_pos == FieldPos_PF_Back_Red) field_pos_s = "PF Red";
    else if (field_pos == FieldPos_PF_Back_Blue) field_pos_s = "PF Blue";
    std::string team_s = "";
    team_s += game_side;
    write_line(1, " ");
    write_line(2, team_s + " " + field_pos_s);
  } else {
    // Changing between menu screens and exiting menu
    if (check_single_press(BTN_NEXT_MENU_SCREEN, true) && (int)menu_screen < num_menu_screens - 1) {
      int temp = (int)menu_screen;
      temp++;
      menu_screen = (menu_screens)temp;
    } else if (check_single_press(BTN_PREV_MENU_SCREEN, true) && menu_screen != menu_screens::shot_tuning) {
      int temp = (int)menu_screen;
      temp--;
      menu_screen = (menu_screens)temp;
    } else if (check_single_press(BTN_EXIT_MENU, true)) in_menu = false;

    // Do different stuff based on menu screen
    switch(menu_screen) {
      case menu_screens::shot_tuning: {
        write_line(1, menu_shot_strings[(int)menu_shot_position]);

        char num[10];
        sprintf(num, "%d", shot_positions[(int)menu_shot_position]);
        std::string num_str = num;
        write_line(2, num_str);

        if (check_single_press(BTN_PREVIOUS_SHOT_SLIDER, true) && (int)menu_shot_position != 0) {
          int temp = (int)menu_shot_position;
          temp--;
          menu_shot_position = (menu_shot_positions)temp;
        } else if (check_single_press(BTN_NEXT_SHOT_SLIDER, true) && (int)menu_shot_position < num_shot_positions - 1) {
          int temp = (int)menu_shot_position;
          temp++;
          menu_shot_position = (menu_shot_positions)temp;
        }

        if (check_single_press(BTN_INCREMENT_SHOT_SLIDER, true) && shot_positions[(int)menu_shot_position] + 5 < 200) shot_positions[(int)menu_shot_position] += 5;
        else if (check_single_press(BTN_DECREMENT_SHOT_SLIDER, true) && shot_positions[(int)menu_shot_position] - 5 > 0) shot_positions[(int)menu_shot_position] -= 5;

        if (check_single_press(BTN_TEST_SHOT_POSITION, true)) {
          pros::delay(1000);
          auto_set_first_shot(shot_positions[(int)menu_shot_position]);
          write_shot_positions_to_file();
        } else if (check_single_press(BTN_SAVE, true)) write_shot_positions_to_file();
        break;
      }
      case menu_screens::auto_select: {
        write_line(1, menu_auto_strings[(int)menu_auto_route]);

        std::string game_side_s = "";
        game_side_s += game_side;
        write_line(2, game_side_s);

        if (check_single_press(BTN_NEXT_AUTO, true) && (int)menu_auto_route < num_auto_routes - 1) {
          int temp = (int)menu_auto_route;
          temp++;
          menu_auto_route = (auto_routines)temp;
        } else if (check_single_press(BTN_PREV_AUTO, true) && (int)menu_auto_route != 0) {
          int temp = (int)menu_auto_route;
          temp--;
          menu_auto_route = (auto_routines)temp;
        } else if (check_single_press(BTN_SAVE, true)) write_auto_route_to_file();
          else if (check_single_press(BTN_SWAP_GAME_SIDE_L, true) || check_single_press(BTN_SWAP_GAME_SIDE_R, true)) swap_game_side();
        break;
      }
      case menu_screens::number_of_menu_screens:
        break;
    }
  }
}

void read_shot_positions_from_file() {
  FILE* shot_positions_file = NULL;
  shot_positions_file = fopen("/usd/shot_positions.txt", "r");
  if (shot_positions_file != NULL) {
    for(int i = 0; i < num_shot_positions; i++) {
      int temp;
      fscanf(shot_positions_file, "%d ", &temp);
      log_ln(LOG_SHOTS, "%d Reading shot position... %s is %d", pros::millis(), menu_shot_strings[i], temp);
      shot_positions[i] = temp;
      switch(i) {
        case 0: pf_SP.top = temp; break;
        case 1: pf_SP.mid = temp; break;
        case 2: front_SP.top = temp; break;
        case 3: front_SP.mid = temp; break;
        case 4: auto_mid_flag_SP.top = temp; break;
        case 5: auto_mid_flag_SP.mid = temp; break;
        case 6: auto_far_flag_SP.top = temp; break;
        case 7: auto_far_flag_SP.mid = temp; break;
        case 8: skills_front_SP.top = temp; break;
        case 9: skills_front_SP.mid = temp; break;
        case 10: skills_corner_SP.top = temp; break;
        case 11: skills_corner_SP.mid = temp; break;
        case 12: skills_back_SP.top = temp; break;
        case 13: skills_back_SP.mid = temp; break;
        case 14: skills_back_SP.bot = temp; break;
      }
      log_ln(LOG_SHOTS, "%d Sucessfully read shot positions from file!", pros::millis());
    }
    fclose(shot_positions_file);
  } else log_ln(LOG_SHOTS, "%d Could not read from shot positions files...", pros::millis());
}

void write_shot_positions_to_file() {
  FILE* shot_positions_file = NULL;
  shot_positions_file = fopen("/usd/shot_positions.txt", "w");
  if (shot_positions_file != NULL) {
    for(int i = 0; i < num_shot_positions; i++) {
      fprintf(shot_positions_file, "%d ", shot_positions[i]);
      log_ln(LOG_SHOTS, "%d Sucessfully wrote %s to shot position file, position is %d", pros::millis(), menu_shot_strings[i], shot_positions[i]);
    }
    partner.rumble("- -");
    ctrler.rumble("- -");
    fclose(shot_positions_file);
    read_shot_positions_from_file();
  } else log_ln(LOG_SHOTS, "%d Could not create shot positions files...", pros::millis());
}

void read_auto_route_from_file() {
  FILE* auto_route_file = NULL;
  auto_route_file = fopen("/usd/auto_route_file.txt", "r");
  if (auto_route_file != NULL) {
    int temp;
    fscanf(auto_route_file, "%d", &temp);
    menu_auto_route = (auto_routines)temp;
    log_ln(LOG_SHOTS, "%d Sucessfully read auto routine from file! Route is %s", pros::millis(), menu_auto_strings[(int)menu_auto_route]);
    fclose(auto_route_file);
  } else log_ln(LOG_SHOTS, "%d Could not read from auto routine file...", pros::millis());
}

void write_auto_route_to_file() {
  FILE* auto_route_file = NULL;
  auto_route_file = fopen("/usd/auto_route_file.txt", "w");
  if (auto_route_file != NULL) {
    fprintf(auto_route_file, "%d", (int)menu_auto_route);
    log_ln(LOG_SHOTS, "%d Successfully wrote to auto route file, route is %s", pros::millis(), menu_auto_strings[(int)menu_auto_route]);

    partner.rumble("- -");
    ctrler.rumble("- -");
    fclose(auto_route_file);
    read_auto_route_from_file();
  } else log_ln(LOG_SHOTS, "%d Could not create auto route file...", pros::millis());
}

void read_game_side_from_file() {
  FILE* game_side_file = NULL;
  game_side_file = fopen("/usd/game_side.txt", "r");
  if (game_side_file != NULL) {
    fscanf(game_side_file, "%c", &game_side);
    log_ln(LOG_SHOTS, "%d Sucessfully read game side from file! Game side is: %c", pros::millis(), game_side);
    fclose(game_side_file);
  } else log_ln(LOG_SHOTS, "%d Could not read from auto routine file...", pros::millis());
}

void swap_game_side() {
  if (game_side == 'R') game_side = 'B';
  else if (game_side == 'B') game_side = 'R';

  FILE* game_side_file = NULL;
  game_side_file = fopen("/usd/game_side.txt", "w");
  if (game_side_file != NULL) {
    fprintf(game_side_file, "%c", game_side);
    partner.rumble("- -");
    ctrler.rumble("- -");
    fclose(game_side_file);
  } else log_ln(LOG_SHOTS, "%d Could not write to game side file...", pros::millis());
}
