#pragma once
#include "main.h"
#include "shot_select.hpp"

// Main Screen
extern lv_obj_t* screen;
extern lv_obj_t* menu;

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
extern int shot_slider_value[4];
extern lv_obj_t* shot_slider_label[4];
extern lv_obj_t* shot_tuning_save_button;
extern lv_obj_t* shot_tuning_save_button_label;

// Diagnostics Tab
extern lv_obj_t* auto_select_tab_title;

void gui_init();
void gui_handle();
lv_res_t shot_tuning_slider_action(lv_obj_t * slider);
lv_res_t shot_tuning_save_button_action(lv_obj_t* button);
