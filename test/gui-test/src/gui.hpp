// #pragma once
#include "main.h"

// Main Screen
lv_obj_t* screen;
lv_obj_t* menu;

// Tabs
extern lv_obj_t* diagnostics_tab;
extern lv_obj_t* shot_tuning_tab;
extern lv_obj_t* auto_select_tab;

// Diagnostics Tab
extern lv_obj_t* diagnostics_tab_title;
extern lv_obj_t* battery_bar;
extern lv_obj_t* battery_bar_text;
extern char battery_bar_string[4];

// Shot Tuning Tab
extern lv_obj_t* shot_tuning_title;
extern lv_obj_t* shot_slider[4];
extern lv_obj_t* shot_slider_text[4];
int shot_slider_value[] = { 0, 0, 0, 0 };

// Diagnostics Tab
extern lv_obj_t* auto_select_tab_title;

void gui_init();
void gui_handle();
lv_res_t shot_tuning_slider_action(lv_obj_t * slider);
