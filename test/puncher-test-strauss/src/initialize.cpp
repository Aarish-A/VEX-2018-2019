#include "main.h"
#include "gui.hpp"

using namespace std;
using namespace piln::gui;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  class SimplePage : public DisplayPage {
  public:
    SimplePage(const string &name) : DisplayPage(name) {}

    void init(lv_obj_t *path) override {
      printf("Init %x\n", path);
      lv_obj_t *content = lv_label_create(path, NULL);
      lv_label_set_text(content, get_name().c_str());
      lv_obj_align(content, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
      static lv_style_t style;
      lv_style_copy(&style, &lv_style_transp);
      style.text.color = LV_COLOR_WHITE;
      lv_obj_set_style(content, &style);
      // lv_obj_set_hidden(content, false);
      // lv_obj_set_style(content, &lv_style_plain);
    }
  };
  DisplayHandler::init({
    new SimplePage("Page 1")
  });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}
