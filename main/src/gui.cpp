#include "gui.hpp"

// Main Screen
lv_obj_t* screen;
lv_obj_t* menu;

// Tabs
lv_obj_t* diagnostics_tab;
lv_obj_t* shot_tuning_tab;
lv_obj_t* auto_select_tab;
lv_obj_t* shot_test_tab;

// Diagnostics Tab
lv_obj_t* diagnostics_tab_title;
lv_obj_t* battery_bar;
lv_obj_t* battery_bar_text;
char battery_bar_string[4];

// Shot Tuning Tab
lv_obj_t* shot_tuning_title;
lv_obj_t* shot_slider[6];
lv_obj_t* shot_slider_text[6];
lv_obj_t* shot_slider_label[6];
int shot_slider_value[] = { 0, 0, 0, 0, 0, 0};
lv_obj_t* shot_tuning_save_button;
lv_obj_t* shot_tuning_save_button_label;

// Diagnostics Tab
lv_obj_t* auto_select_tab_title;
lv_obj_t* red_side_button;
lv_obj_t* red_side_button_label;
lv_obj_t* blue_side_button;
lv_obj_t* blue_side_button_label;
extern lv_obj_t* auto_buttons[8];
extern lv_obj_t* auto_buttons_label[8];

// Shot Testing Tab
lv_obj_t* shot_testing_title;
lv_obj_t* top_flag_auto_shot;
lv_obj_t* top_flag_auto_shot_label;
lv_obj_t* mid_flag_auto_shot;
lv_obj_t* mid_flag_auto_shot_label;
lv_obj_t* mid_flag_turn_shot;
lv_obj_t* mid_flag_turn_shot_label;
lv_obj_t* top_flag_turn_shot;
lv_obj_t* top_flag_turn_shot_label;


lv_obj_t* auto_buttons[8];
lv_obj_t* auto_buttons_label[8];
std::string auto_routines[8] = {
  "FRONT",
  "FRONT PARK",
  "BACK MID FIRST",
  "BACK FAR FIRST",
  "UNUSED4",
  "UNUSED5",
  "UNUSED6",
  "UNUSED7"
};

void gui_init() {
  printf("Log Program done\n");
  // pros::delay(50);
  int read_return = 0;
  int temp = 0;
  for(int i = 0; i < 6; i++) {
    FILE* log = NULL;
    printf("file pointer\n");
    if (i == 0) {
      log = fopen("/usd/back_mid_shot_position.txt", "r");
      if(log != NULL) read_return = fscanf(log, "%d", &temp);
      if (read_return == 1)
      {
          pf_back_SP.mid = temp;
          log_ln(LOG_IO, "%d pf_back_SP.mid var set (init) | Read_Return: %d | Temp: %d | pf_back_SP.mid: %d", pros::millis(), read_return, temp, pf_back_SP.mid);
      }
      else log_ln(LOG_IO, "%d pf_back_SP.mid var set failed (init) | Read_Return: %d | Temp: %d | pf_back_SP.mid: %d", pros::millis(), read_return, temp, pf_back_SP.mid);

    } else if (i == 1) {
      log = fopen("/usd/back_top_shot_position.txt", "r");
      if(log != NULL) read_return = fscanf(log, "%d", &temp);
      if (read_return == 1)
      {
          pf_back_SP.top = temp;
          log_ln(LOG_IO, "%d pf_back_SP.top var set (init) | Read_Return: %d | Temp: %d | pf_back_SP.top: %d", pros::millis(), read_return, temp, pf_back_SP.top);
      }
      else log_ln(LOG_IO, "%d pf_back_SP.top var set failed (init) | Read_Return: %d | Temp: %d | pf_back_SP.top: %d", pros::millis(), read_return, temp, pf_back_SP.top);
    } else if (i == 2) {
      log = fopen("/usd/front_mid_shot_position.txt", "r");
      if(log != NULL) read_return = fscanf(log, "%d", &temp);
      if (read_return == 1)
      {
          front_SP.mid = temp;
          log_ln(LOG_IO, "%d front_SP.mid var set (init) | Read_Return: %d | Temp: %d | front_SP.mid: %d", pros::millis(), read_return, temp, front_SP.mid);
      }
      else log_ln(LOG_IO, "%d front_SP.mid var set failed (init) | Read_Return: %d | Temp: %d | front_SP.mid: %d", pros::millis(), read_return, temp, front_SP.mid);
    } else if (i == 3) {
      log = fopen("/usd/front_top_shot_position.txt", "r");
      if(log != NULL) read_return = fscanf(log, "%d", &temp);
      if (read_return == 1)
      {
          front_SP.top = temp;
          log_ln(LOG_IO, "%d front_SP.top var set (init) | Read_Return: %d | Temp: %d | front_SP.top: %d", pros::millis(), read_return, temp, front_SP.top);
      }
      else log_ln(LOG_IO, "%d front_SP.top var set failed (init) | Read_Return: %d | Temp: %d | front_SP.top: %d", pros::millis(), read_return, temp, front_SP.top);
    } else if (i == 4) {
      log = fopen("/usd/front_top_auto_position.txt", "r");
      if(log != NULL) read_return = fscanf(log, "%d", &temp);
      if (read_return == 1)
      {
          auto_SP.top = temp;
          log_ln(LOG_IO, "%d auto_SP.top var set (init) | Read_Return: %d | Temp: %d | auto_SP.top: %d", pros::millis(), read_return, temp, auto_SP.top);
      }
      else log_ln(LOG_IO, "%d auto_SP.top var set failed (init) | Read_Return: %d | Temp: %d | auto_SP.top: %d", pros::millis(), read_return, temp, auto_SP.top);
    } else if (i == 5) {
      log = fopen("/usd/front_mid_auto_position.txt", "r");
      if(log != NULL) read_return = fscanf(log, "%d", &temp);
      if (read_return == 1)
      {
          auto_SP.mid = temp;
          log_ln(LOG_IO, "%d auto_SP.mid var set (init) | Read_Return: %d | Temp: %d | auto_SP.mid: %d", pros::millis(), read_return, temp, auto_SP.mid);
      }
      else log_ln(LOG_IO, "%d auto_SP.mid var set failed (init) | Read_Return: %d | Temp: %d | auto_SP.mid: %d", pros::millis(), read_return, temp, auto_SP.mid); 
    }

  //   printf("for loop exited");
  //   pros::delay(50);
    if (log == NULL) {
      printf("Failed to read shot positions file %d\n", i);
    } else {
      printf("Created shot positions file! %d", i);
    }
    pros::delay(50);
    printf("%d\n", shot_slider_value[i]);
    fclose(log);
  }

  //
  // // Main Screen
  screen = lv_obj_create(NULL, NULL);
  lv_scr_load(screen);
	menu = lv_tabview_create(lv_scr_act(), NULL);
  lv_tabview_set_sliding(menu, false);

  // Tabs
	diagnostics_tab = lv_tabview_add_tab(menu, "Diagnostics");
	shot_tuning_tab = lv_tabview_add_tab(menu, "Shot Tuning");
	auto_select_tab = lv_tabview_add_tab(menu, "Auto Select");
  shot_test_tab = lv_tabview_add_tab(menu, "Test Shots");

	// // Diagnostics Tab
	diagnostics_tab_title = lv_label_create(diagnostics_tab, NULL);
	lv_label_set_text(diagnostics_tab_title, "Diagnostics");
	lv_obj_align(diagnostics_tab_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

	// battery_bar_text = lv_bar_create(diagnostics_tab, NULL);
	// lv_obj_set_size(battery_bar, 200, 30);
	// lv_bar_set_value(battery_bar, pros::battery::get_capacity());
	// lv_obj_align(battery_bar, diagnostics_tab_title, LV_ALIGN_OUT_BOTTOM_MID, -120, 10);
	// battery_bar_text = lv_label_create(diagnostics_tab, NULL);
	// lv_obj_align(battery_bar_text, battery_bar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  // sprintf(battery_bar_string, "%.1f p/c", pros::battery::get_capacity());
  // lv_label_set_text(battery_bar_text, battery_bar_string);

  // Shot Testing Tab
  shot_testing_title = lv_label_create(shot_test_tab, NULL);
	lv_label_set_text(shot_testing_title, "Shot Testing");
	lv_obj_align(shot_testing_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

  top_flag_auto_shot = lv_btn_create(shot_test_tab, NULL);
  lv_obj_set_free_num(top_flag_auto_shot, 1);
  lv_btn_set_action(top_flag_auto_shot, LV_BTN_ACTION_LONG_PR, shot_test_top_auto_action);
  lv_obj_align(top_flag_auto_shot, NULL, LV_ALIGN_OUT_BOTTOM_MID, -20, 15);
  lv_obj_set_width(top_flag_auto_shot, 250);
  top_flag_auto_shot_label = lv_label_create(top_flag_auto_shot, NULL);
  lv_label_set_text(top_flag_auto_shot_label, "Top Auto");

  mid_flag_auto_shot = lv_btn_create(shot_test_tab, NULL);
  lv_obj_set_free_num(mid_flag_auto_shot, 1);
  lv_btn_set_action(mid_flag_auto_shot, LV_BTN_ACTION_LONG_PR, shot_test_mid_auto_action);
  lv_obj_align(mid_flag_auto_shot, top_flag_auto_shot, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
  lv_obj_set_width(mid_flag_auto_shot, 250);
  mid_flag_auto_shot_label = lv_label_create(mid_flag_auto_shot, NULL);
  lv_label_set_text(mid_flag_auto_shot_label, "Mid Auto");

  mid_flag_turn_shot = lv_btn_create(shot_test_tab, NULL);
  lv_obj_set_free_num(mid_flag_turn_shot, 1);
  lv_btn_set_action(mid_flag_turn_shot, LV_BTN_ACTION_LONG_PR, shot_test_mid_turn_action);
  lv_obj_align(mid_flag_turn_shot, mid_flag_auto_shot, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
  lv_obj_set_width(mid_flag_turn_shot, 250);
  mid_flag_turn_shot_label = lv_label_create(mid_flag_turn_shot, NULL);
  lv_label_set_text(mid_flag_turn_shot_label, "Mid Turn");

  top_flag_turn_shot = lv_btn_create(shot_test_tab, NULL);
  lv_obj_set_free_num(top_flag_turn_shot, 1);
  lv_btn_set_action(top_flag_turn_shot, LV_BTN_ACTION_LONG_PR, shot_test_top_turn_action);
  lv_obj_align(top_flag_turn_shot, mid_flag_turn_shot, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
  lv_obj_set_width(top_flag_turn_shot, 250);
  top_flag_turn_shot_label = lv_label_create(top_flag_turn_shot, NULL);
  lv_label_set_text(top_flag_turn_shot_label, "Top Turn");

	// Shot Tuning Tab
	shot_tuning_title = lv_label_create(shot_tuning_tab, NULL);
	lv_label_set_text(shot_tuning_title, "Shot Tuning");
	lv_obj_align(shot_tuning_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

  for(int i = 0; i < 6; i++) {
    shot_slider[i] = lv_slider_create(shot_tuning_tab, NULL);
    lv_obj_set_size(shot_slider[i], 300, 30);
    lv_slider_set_range(shot_slider[i], 0, 250);
    lv_slider_set_action(shot_slider[i], shot_tuning_slider_action);
    if (i == 0) lv_obj_align(shot_slider[i], shot_tuning_title, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);
    else lv_obj_align(shot_slider[i], shot_slider[i - 1], LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

    shot_slider_label[i] = lv_label_create(shot_tuning_tab, NULL);
    lv_obj_align(shot_slider_label[i], shot_slider[i], LV_ALIGN_OUT_LEFT_MID, -35, 0);

    if (i == 0) {
      lv_label_set_text(shot_slider_label[i], "Back Mid");
      lv_bar_set_value(shot_slider[i], pf_back_SP.mid);
    } else if (i == 1) {
      lv_label_set_text(shot_slider_label[i], "Back Top");
      lv_bar_set_value(shot_slider[i], pf_back_SP.top);
    } else if (i == 2) {
      lv_label_set_text(shot_slider_label[i], "Front Mid");
      lv_bar_set_value(shot_slider[i], front_SP.mid);
    } else if (i == 3) {
      lv_label_set_text(shot_slider_label[i], "Front Top");
      lv_bar_set_value(shot_slider[i], front_SP.top);
    } else if (i == 4) {
      lv_label_set_text(shot_slider_label[i], "Auto Top");
      lv_bar_set_value(shot_slider[i], auto_SP.top);
    } else if (i == 5) {
      lv_label_set_text(shot_slider_label[i], "Auto Mid");
      lv_bar_set_value(shot_slider[i], auto_SP.mid);
    }

    shot_slider_text[i] = lv_label_create(shot_tuning_tab, NULL);
    lv_obj_align(shot_slider_text[i], shot_slider[i], LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    char shot_slider_string[3];
    sprintf(shot_slider_string, "%d", lv_slider_get_value(shot_slider[i]));
  	lv_label_set_text(shot_slider_text[i], shot_slider_string);
  }

  shot_tuning_save_button = lv_btn_create(shot_tuning_tab, NULL);
  lv_obj_set_free_num(shot_tuning_save_button, 1);
  lv_btn_set_action(shot_tuning_save_button, LV_BTN_ACTION_LONG_PR, shot_tuning_save_button_action);
  lv_obj_align(shot_tuning_save_button, shot_slider[3], LV_ALIGN_OUT_BOTTOM_MID, -20, 100);
  lv_obj_set_width(shot_tuning_save_button, 250);
  shot_tuning_save_button_label = lv_label_create(shot_tuning_save_button, NULL);
  lv_label_set_text(shot_tuning_save_button_label, "Save?");

  // Auto Tab
  auto_select_tab_title = lv_label_create(auto_select_tab, NULL);
  lv_label_set_text(auto_select_tab_title, "Auto Select");
  lv_obj_align(auto_select_tab_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

  red_side_button = lv_btn_create(auto_select_tab, NULL);
  lv_obj_set_free_num(red_side_button, 1);
  lv_btn_set_action(red_side_button, LV_BTN_ACTION_LONG_PR, red_side_button_action);
  lv_obj_set_width(red_side_button, 200);
  lv_obj_align(red_side_button, auto_select_tab_title, LV_ALIGN_OUT_BOTTOM_MID, -100, 10);
  lv_obj_t* red_side_button_label = lv_label_create(red_side_button, NULL);
  lv_label_set_text(red_side_button_label, "Red");


  blue_side_button = lv_btn_create(auto_select_tab, NULL);
  lv_obj_set_free_num(blue_side_button, 1);
  lv_btn_set_action(blue_side_button, LV_BTN_ACTION_LONG_PR, blue_side_button_action);
  lv_obj_set_width(blue_side_button, 200);
  lv_obj_align(blue_side_button, auto_select_tab_title, LV_ALIGN_OUT_BOTTOM_MID, 100, 10);
  blue_side_button_label = lv_label_create(blue_side_button, NULL);
  lv_label_set_text(blue_side_button_label, "Blue");

  int heightOffset = 120;
  for (int i = 0; i < static_cast<int>(auto_routines::K_AUTO_ROUTINE_NUM); i++) {
    auto_buttons[i] = lv_btn_create(auto_select_tab, NULL);
    lv_obj_set_free_num(auto_buttons[i], 1);
    if (i == (int)auto_routines::FRONT) lv_btn_set_action(auto_buttons[i], LV_BTN_ACTION_LONG_PR, auto_button_action_front);
    else if (i == (int)auto_routines::FRONT_PARK) lv_btn_set_action(auto_buttons[i], LV_BTN_ACTION_LONG_PR, auto_button_action_front_park);
    else if (i == (int)auto_routines::BACK_MID_FIRST) lv_btn_set_action(auto_buttons[i], LV_BTN_ACTION_LONG_PR, auto_button_action_back_mid_first);
    else if (i == (int)auto_routines::BACK_FAR_FIRST) lv_btn_set_action(auto_buttons[i], LV_BTN_ACTION_LONG_PR, auto_button_action_back_far_first);
    // lv_obj_set_width(auto_buttons[i], 200);
    lv_obj_set_size(auto_buttons[i], 200, 50);
    if (((i + 1) % 2) == 0) heightOffset += 80;
    lv_obj_align(auto_buttons[i], auto_select_tab_title, LV_ALIGN_OUT_BOTTOM_MID, (((i + 1) % 2) == 1 ? -100 : 100), heightOffset);
    auto_buttons_label[i] = lv_label_create(auto_buttons[i], NULL);
    std::string str = auto_routines[i];
    const char* cstr = str.c_str();;
    lv_label_set_text(auto_buttons_label[i], cstr);
  }
}

void gui_handle() {
}

lv_res_t auto_button_action_front(lv_obj_t* button) {
  FILE* log = NULL;
  while ((log = fopen("/usd/auto_routine.txt", "w")) == NULL) {
    pros::delay(2);
    printf("yeyee");
  }
  if (log == NULL) {
    printf("Couldn't create auton routine file\n");
  } else {
    fprintf(log, "%d", auto_routines::FRONT);
    current_auto_routine = auto_routines::FRONT;
    printf("%d Set Auto: %d \n", pros::millis(), current_auto_routine);
    ctrler.rumble(". . .");
    fclose(log);
  }
  return LV_RES_OK;
}

lv_res_t auto_button_action_front_park(lv_obj_t* button) {
  FILE* log = NULL;
  while ((log = fopen("/usd/auto_routine.txt", "w")) == NULL) {
    pros::delay(2);
    printf("yeyee");
  }
  if (log == NULL) {
    printf("Couldn't create auton routine file\n");
  } else {
    fprintf(log, "%d", auto_routines::FRONT_PARK);
    current_auto_routine = auto_routines::FRONT_PARK;
    printf("%d Set Auto: %d \n", pros::millis(), current_auto_routine);
    ctrler.rumble(". . .");
    fclose(log);
  }
  return LV_RES_OK;
}

lv_res_t auto_button_action_back_mid_first(lv_obj_t* button) {
  FILE* log = NULL;
  while ((log = fopen("/usd/auto_routine.txt", "w")) == NULL) pros::delay(2);
  if (log == NULL) {
    printf("Couldn't create auto routine file\n");
  } else {
    fprintf(log, "%d", auto_routines::BACK_MID_FIRST);
    current_auto_routine = auto_routines::BACK_MID_FIRST;
    ctrler.rumble(". . .");
    fclose(log);
  }
  return LV_RES_OK;
}

lv_res_t auto_button_action_back_far_first(lv_obj_t* button) {
  FILE* log = NULL;
  while ((log = fopen("/usd/auto_routine.txt", "w")) == NULL) pros::delay(2);
  if (log == NULL) {
    printf("Couldn't create auto routine file\n");
  } else {
    fprintf(log, "%d", auto_routines::BACK_FAR_FIRST);
    current_auto_routine =  auto_routines::BACK_FAR_FIRST;
    ctrler.rumble(". . .");
    fclose(log);
  }
  return LV_RES_OK;
}

lv_res_t red_side_button_action(lv_obj_t* button) {
  FILE* log = NULL;
  while ((log = fopen("/usd/game_side.txt", "w")) == NULL) pros::delay(2);
  if (log == NULL) {
    printf("Couldn't create game side file\n");
  } else {
    fprintf(log, "R");
    game_side = 'R';
    ctrler.rumble(". . .");
    fclose(log);
  }
  return LV_RES_OK;
}

lv_res_t blue_side_button_action(lv_obj_t* button) {
  FILE* log = NULL;
  while ((log = fopen("/usd/game_side.txt", "w")) == NULL) pros::delay(2);
  if (log == NULL) {
    printf("Couldn't create game side file\n");
  } else {
    fprintf(log, "B");
    game_side = 'B';
    ctrler.rumble(". . .");
    fclose(log);
  }
  return LV_RES_OK;
}

lv_res_t shot_tuning_slider_action(lv_obj_t* slider) {
  for (int i = 0; i < 6; i++) {
    char shot_slider_string[4];
    sprintf(shot_slider_string, "%d", lv_slider_get_value(shot_slider[i]));
    lv_label_set_text(shot_slider_text[i], shot_slider_string);
    shot_slider_value[i] = lv_slider_get_value(slider);
  }
  return LV_RES_OK;
}
lv_res_t shot_test_top_auto_action(lv_obj_t* button)
{
  ctrler.rumble(". . .");
  pros::delay(2000);
  printf("position is %d",auto_SP.top);
  auto_set_first_shot(auto_SP.top);

  return LV_RES_OK;
}

lv_res_t shot_test_mid_auto_action(lv_obj_t* button)
{
  ctrler.rumble(". . .");
  pros::delay(2000);
  printf("position is %d",auto_SP.mid);
  auto_set_first_shot(auto_SP.mid);

  return LV_RES_OK;
}

lv_res_t shot_test_mid_turn_action(lv_obj_t* button)
{
  ctrler.rumble(". . .");
  pros::delay(2000);
  printf("position is %d",pf_back_SP.mid);
  auto_set_first_shot(pf_back_SP.mid);

  return LV_RES_OK;
}
lv_res_t shot_test_top_turn_action(lv_obj_t* button)
{
  ctrler.rumble(". . .");
  pros::delay(2000);
  printf("position is %d",pf_back_SP.top);
  auto_set_first_shot(pf_back_SP.top);

  return LV_RES_OK;
}
lv_res_t shot_tuning_save_button_action(lv_obj_t* button) {
  ctrler.rumble(". . .");
  for(int i = 0; i < 6; i++) {
    FILE* log = NULL;
    if (i == 0) {
      log = fopen("/usd/back_mid_shot_position.txt", "w");
      pf_back_SP.mid = lv_slider_get_value(shot_slider[0]);
    } else if (i == 1) {
      log = fopen("/usd/back_top_shot_position.txt", "w");
      pf_back_SP.top = lv_slider_get_value(shot_slider[1]);
    } else if (i == 2) {
      log = fopen("/usd/front_mid_shot_position.txt", "w");
      front_SP.mid = lv_slider_get_value(shot_slider[2]);
    } else if (i == 3) {
      log = fopen("/usd/front_top_shot_position.txt", "w");
      front_SP.top = lv_slider_get_value(shot_slider[3]);
    } else if (i == 4) {
      log = fopen("/usd/front_top_auto_position.txt", "w");
      auto_SP.top = lv_slider_get_value(shot_slider[4]);
    } else if (i == 5) {
      log = fopen("/usd/front_mid_auto_position.txt", "w");
      auto_SP.mid = lv_slider_get_value(shot_slider[5]);
    }

    if (log == NULL) {
      printf("Failed to create shot positions file %d\n", i);
      printf("shot save NULL");
    } else {
      printf("Created shot positions file! %d\n", i);
      fprintf(log, "%d", lv_slider_get_value(shot_slider[i]));
      printf("%d\n", lv_slider_get_value(shot_slider[i]));
      printf("shot save NOT NULL");
      fclose(log);
    }
  }
  return LV_RES_OK; /*Return OK if the message box is not deleted*/
}
