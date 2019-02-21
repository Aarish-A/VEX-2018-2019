#include "gui.hpp"

menu_screens menu_screen = menu_screens::game_screen;
const char* menu_screen_titles[num_menu_screens] = {"Game Screen", "Shot Tuning"};
char partner_screen_lines[3][16] = {"               ", "               ", "               "};
int update_line = 0;
uint32_t update_time = 0;

menu_shot_positions menu_shot_position = front_top;
int shot_positions[num_shot_positions] = { front_SP.top, front_SP.mid, pf_SP.top, pf_SP.mid, auto_SP.top, auto_SP.mid,
                                           skills_front_SP.top, skills_front_SP.mid, skills_corner_SP.top, skills_corner_SP.mid,
                                           skills_back_SP.top, skills_back_SP.mid, skills_back_SP.bot };
const char* menu_shot_strings[num_shot_positions] = {"FrontTop", "FrontMid", "BackTop", "BackMid", "AutoTop", "AutoMid", "SkillFrTop", "SkillFrMid",
                                                     "SkillCorTop", "SkillCorMid", "SkillBackTop", "SkillBackMid", "SkillsBackBot" };

void gui_init() {
  if (partner_connected) {
    partner.print(0, 0, "               ");
    pros::delay(60);
    partner.print(1, 0, "               ");
    pros::delay(60);
    partner.print(2, 0, "               ");
  }

  read_shot_positions_from_file();
  update_time = pros::millis();
}

void gui_handle() {
  if (pros::millis() >= update_time) {
    partner.print(update_line, 0, partner_screen_lines[update_line]);
    update_time = pros::millis() + UPDATE_INTERVAL;
    if (update_line < 2) update_line++;
    else update_line = 0;

    // Update Line 0
    write_line(0, menu_screen_titles[(int)menu_screen]);

    // Update Line 2

  }

  switch(menu_screen) {
    case menu_screens::game_screen: {
      if (check_single_press(BTN_ENTER_SHOT_TUNING, true)) {
        printf("got here\n");
        menu_screen = menu_screens::shot_tuning;
        strcpy(partner_screen_lines[0], menu_screen_titles[(int)menu_screen]);
        strcpy(partner_screen_lines[1], menu_shot_strings[(int)menu_shot_position]);
        char num[10];
        sprintf(num, "  %d", shot_positions[(int)menu_shot_position]);
        write_line(2, num);
      }

      std::string field_pos_s = "default";
      FieldPos field_pos= shot_req[0].field_pos;
      if (field_pos== FieldPos_Front) field_pos_s = "Fr ";
      else if (field_pos== FieldPos_Back) field_pos_s = "Bck";
      else if (field_pos== FieldPos_PF) field_pos_s = "PF ";
      else if (field_pos== FieldPos_PF_Back_Red) field_pos_s = "PfR";
      else if (field_pos== FieldPos_PF_Back_Blue) field_pos_s = "PfB";
      std::string team_s = blue_team ? "B" : "R";
      // sprintf(partner_screen_lines[2], "%c %s", game_side, field_pos_s.c_str());
      write_line(2, field_pos_s);
      break;
    }

    case menu_screens::shot_tuning: {
      write_line(1, menu_shot_strings[(int)menu_shot_position]);

      char num[10];
      sprintf(num, "%d", shot_positions[(int)menu_shot_position]);
      std::string num_str = num;
      write_line(2, num_str);

      if (check_single_press(BTN_PREVIOUS_SHOT_SLIDER, true) && menu_shot_position != front_top) {
        int temp = (int)menu_shot_position;
        temp--;
        menu_shot_position = (menu_shot_positions)temp;
      }
      else if (check_single_press(BTN_NEXT_SHOT_SLIDER, true) && menu_shot_position != skills_back_bot) {
        int temp = (int)menu_shot_position;
        temp++;
        menu_shot_position = (menu_shot_positions)temp;
      }

      if (check_single_press(BTN_INCREMENT_SHOT_SLIDER, true)) {
        shot_positions[(int)menu_shot_position] += 5;
      } else if (check_single_press(BTN_DECREMENT_SHOT_SLIDER, true)) {
        shot_positions[(int)menu_shot_position] -= 5;
      }

      if (check_single_press(BTN_TEST_SHOT_POSITION, true)) {
        pros::delay(1000);
        auto_set_first_shot(shot_positions[(int)menu_shot_position]);
        write_shot_positions_to_file();
      }

      if (check_single_press(BTN_EXIT_SHOT_TUNING, true)) {
        menu_screen = menu_screens::game_screen;
        write_line(1, " ");
        write_line(2, " ");
      }
      break;
    }
    case menu_screens::number_of_menu_screens:
      break;
  }
}

void read_shot_positions_from_file() {
  FILE* shot_positions_file = NULL;
  shot_positions_file = fopen("/usd/shot_positions.txt", "r");
  if (shot_positions_file != NULL) {
    for(int i = 0; i < num_shot_positions; i++) {
      int temp;
      fscanf(shot_positions_file, "%d", &temp);
      printf("i: %d temp: %d\n", i, temp);
      switch(i) {
        case 0: front_SP.top = temp; break;
        case 1: front_SP.mid = temp; break;
        case 2: pf_SP.top = temp; break;
        case 3: pf_SP.mid = temp; break;
        case 4: auto_SP.top = temp; break;
        case 5: auto_SP.mid = temp; break;
        case 6: skills_front_SP.top = temp; break;
        case 7: skills_front_SP.mid = temp; break;
        case 8: skills_corner_SP.top = temp; break;
        case 9: skills_corner_SP.mid = temp; break;
        case 10: skills_back_SP.top = temp; break;
        case 11: skills_back_SP.mid = temp; break;
        case 12: skills_back_SP.bot = temp; break;
      }
      log_ln(LOG_SHOTS, "%d Sucessfully read %s shot position file, position is %d", pros::millis(), menu_shot_strings[i], temp);
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
    ctrler.rumble("- -");
    pros::delay(500);
    fclose(shot_positions_file);
    read_shot_positions_from_file();
  } else log_ln(LOG_SHOTS, "%d Could not create shot positions files...", pros::millis());
}

void write_line(int line_num, std::string line) {
  while(line.size() < 15) {
    line += " ";
  }
  strcpy(partner_screen_lines[line_num], line.c_str());
}
