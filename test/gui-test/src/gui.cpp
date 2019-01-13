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
int shot_slider_value[] = { 0, 0, 0, 0 };

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
    // lv_slider_set_action(shot_slider[i], shot_tuning_slider_action);
    if (i == 0) lv_obj_align(shot_slider[i], shot_tuning_title, LV_ALIGN_OUT_BOTTOM_MID, -120, 10);
    else lv_obj_align(shot_slider[i], shot_slider[i - 1], LV_ALIGN_OUT_BOTTOM_RIGHT, -120, 10);

    shot_slider_text[i] = lv_label_create(shot_tuning_tab, NULL);
    lv_obj_align(shot_slider_text[i], shot_slider[i], LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    char shot_slider_string[4];
    sprintf(shot_slider_string, "%d", shot_slider_value[i]);
  	lv_label_set_text(shot_slider_text[i], shot_slider_string);
  }

  // Auto Tab
  auto_select_tab_title = lv_label_create(auto_select_tab, NULL);
  lv_label_set_text(auto_select_tab_title, "Auto Select");
  lv_obj_align(auto_select_tab_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
}

void gui_handle() {
  for(int i = 0; i < 4; i++) {
    char shot_slider_string[4];
    sprintf(shot_slider_string, "%d", lv_slider_get_value(shot_slider[i]));
    lv_label_set_text(shot_slider_text[i], shot_slider_string);
    shot_slider_value[i] = lv_slider_get_value(shot_slider[i]);
  }
}

lv_res_t shot_tuning_slider_action(lv_obj_t* slider) {
  char shot_slider_string[4];
  for (int i = 0; i < 4; i++) {
    sprintf(shot_slider_string, "%d", lv_slider_get_value(slider));
    lv_label_set_text(shot_slider_text[i], shot_slider_string);
    shot_slider_value[i] = lv_slider_get_value(slider);
  }
  return LV_RES_OK;
}
