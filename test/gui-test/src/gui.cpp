#include "main.h"
#include "gui.hpp"

// Main Screen
lv_obj_t* screen;
lv_obj_t* menu;

// Tabs
lv_obj_t* diagnostics_tab;
lv_obj_t* shot_tuning_tab;
lv_obj_t* auto_select_tab;

// Diagnostics Tab
lv_obj_t* diagnostics_tab_title;
lv_obj_t* battery_bar;
lv_obj_t* battery_bar_text;
char battery_bar_string[4];

// Shot Tuning Tab
lv_obj_t* shot_tuning_title;
lv_obj_t* shot_slider[4];
lv_obj_t* shot_slider_text[4];
lv_obj_t* shot_slider_label[4];
int shot_slider_value[] = { 0, 0, 0, 0 };
lv_obj_t* shot_tuning_save_button;
lv_obj_t* shot_tuning_save_button_label;

// Diagnostics Tab
lv_obj_t* auto_select_tab_title;


void gui_init() {
  // Main Screen
  screen = lv_obj_create(NULL, NULL);
  lv_scr_load(screen);
	menu = lv_tabview_create(lv_scr_act(), NULL);

  // Tabs
	diagnostics_tab = lv_tabview_add_tab(menu, "Diagnostics");
	shot_tuning_tab = lv_tabview_add_tab(menu, "Shot Tuning");
	auto_select_tab = lv_tabview_add_tab(menu, "Auto Select");

	// // Diagnostics Tab
	// diagnostics_tab_title = lv_label_create(diagnostics_tab, NULL);
	// lv_label_set_text(diagnostics_tab_title, "Diagnostics");
	// lv_obj_align(diagnostics_tab_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
  //
	// battery_bar_text = lv_bar_create(diagnostics_tab, NULL);
	// lv_obj_set_size(battery_bar, 200, 30);
	// lv_bar_set_value(battery_bar, pros::battery::get_capacity());
	// lv_obj_align(battery_bar, diagnostics_tab_title, LV_ALIGN_OUT_BOTTOM_MID, -120, 10);
	// battery_bar_text = lv_label_create(diagnostics_tab, NULL);
	// lv_obj_align(battery_bar_text, battery_bar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  // sprintf(battery_bar_string, "%.1f p/c", pros::battery::get_capacity());
  // lv_label_set_text(battery_bar_text, battery_bar_string);


	// Shot Tuning Tab
	shot_tuning_title = lv_label_create(shot_tuning_tab, NULL);
	lv_label_set_text(shot_tuning_title, "Shot Tuning");
	lv_obj_align(shot_tuning_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

  for(int i = 0; i < 4; i++) {
    shot_slider[i] = lv_slider_create(shot_tuning_tab, NULL);
    lv_obj_set_size(shot_slider[i], 300, 30);
    lv_slider_set_range(shot_slider[i], 50, 100);
    lv_slider_set_action(shot_slider[i], shot_tuning_slider_action);
    if (i == 0) lv_obj_align(shot_slider[i], shot_tuning_title, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);
    else lv_obj_align(shot_slider[i], shot_slider[i - 1], LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

    shot_slider_text[i] = lv_label_create(shot_tuning_tab, NULL);
    lv_obj_align(shot_slider_text[i], shot_slider[i], LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    char shot_slider_string[4];
    sprintf(shot_slider_string, "%d", shot_slider_value[i]);
  	lv_label_set_text(shot_slider_text[i], shot_slider_string);

    shot_slider_label[i] = lv_label_create(shot_tuning_tab, NULL);
    lv_obj_align(shot_slider_label[i], shot_slider[i], LV_ALIGN_OUT_LEFT_MID, -35, 0);

    if (i == 0) lv_label_set_text(shot_slider_label[i], "Back");
    else if (i == 1) lv_label_set_text(shot_slider_label[i], "Back PF");
    else if (i == 2) lv_label_set_text(shot_slider_label[i], "Mid PF");
    else if (i == 3) lv_label_set_text(shot_slider_label[i], "Front PF");
  }

  shot_tuning_save_button = lv_btn_create(shot_tuning_tab, NULL);
  lv_obj_set_free_num(shot_tuning_save_button, 1);
  lv_btn_set_action(shot_tuning_save_button, LV_BTN_ACTION_LONG_PR, shot_tuning_save_button_action);
  lv_obj_align(shot_tuning_save_button, shot_slider[3], LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
  lv_obj_set_width(shot_tuning_save_button, 250);
  shot_tuning_save_button_label = lv_label_create(shot_tuning_save_button, NULL);
  lv_label_set_text(shot_tuning_save_button_label, "Save?");

  // Auto Tab
  auto_select_tab_title = lv_label_create(auto_select_tab, NULL);
  lv_label_set_text(auto_select_tab_title, "Auto Select");
  lv_obj_align(auto_select_tab_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
}

void gui_handle() {
}

lv_res_t shot_tuning_slider_action(lv_obj_t* slider) {
  for (int i = 0; i < 4; i++) {
    char shot_slider_string[4];
    sprintf(shot_slider_string, "%d", lv_slider_get_value(shot_slider[i]));
    lv_label_set_text(shot_slider_text[i], shot_slider_string);
    shot_slider_value[i] = lv_slider_get_value(slider);
  }
  return LV_RES_OK;
}

lv_res_t shot_tuning_save_button_action(lv_obj_t* button) {
  for(int i = 0; i < 4; i++) {
    FILE* log = NULL;
    if (i == 0) log = fopen("/usd/back_shot_position.txt", "w");
    else if (i == 1) log = fopen("/usd/backPF_shot_position.txt", "w");
    else if (i == 2) log = fopen("/usd/midPF_shot_position.txt", "w");
    else if (i == 3) log = fopen("/usd/frontPF_shot_position.txt", "w");

    if (log == NULL) {
      printf("Failed to create shot positions file %d\n", i);
    } else {
      printf("Created shot positions file! %d", i);
    }
    fprintf(log, "%d", shot_slider_value[i]);
    printf("%d\n", shot_slider_value[i]);
    fclose(log);
  }
  return LV_RES_OK; /*Return OK if the message box is not deleted*/
}
