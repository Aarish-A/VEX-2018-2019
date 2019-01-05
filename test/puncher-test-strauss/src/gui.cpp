#include "gui.hpp"
#include <cstring>

using namespace pros;
using namespace std;

namespace piln::gui {
  std::vector<std::pair<DisplayPage *, lv_obj_t *>> DisplayHandler::pages;
  std::map<std::string, size_t> DisplayHandler::page_map;
  size_t DisplayHandler::cur_page = 0;
  pros::Task *DisplayHandler::task = nullptr;

  DisplayPage::DisplayPage(const string &name) : name(name) {}

  void DisplayPage::cycle() {
    Task(CURRENT_TASK).notify_take(true, 1000);
  }

  void DisplayPage::opened() {}

  void DisplayPage::closed() {}

  DisplayPage::~DisplayPage() = default;

  void DisplayHandler::init(initializer_list<DisplayPage *> page_list) {
    // Set up styles
    static lv_style_t style_label;

    lv_style_copy(&style_label, &lv_style_transp);
    style_label.text.color = LV_COLOR_WHITE;

    static lv_style_t style_page_left;
    static lv_style_t style_page_right;

    lv_style_copy(&style_page_left, &lv_style_scr);
    style_page_left.body.main_color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    style_page_left.body.grad_color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    style_page_left.body.opa = LV_OPA_COVER;
    style_page_left.body.border.color = LV_COLOR_MAKE(0x50, 0x50, 0x50);
    style_page_left.body.border.width = 1;
    style_page_left.body.border.opa = LV_OPA_COVER;
    style_page_left.body.padding = { 0, 0, 0 };

    lv_style_copy(&style_page_right, &style_page_left);
    style_page_right.body.main_color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
    style_page_right.body.grad_color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
    style_page_right.body.padding = { LV_DPI / 10, LV_DPI / 10, 0 };

    lv_obj_t *btnPage = lv_page_create(lv_scr_act(), NULL);
    lv_page_set_style(btnPage, LV_PAGE_STYLE_BG, &style_page_left);
    lv_obj_set_size(btnPage, LV_DPI / 2, LV_VER_RES);
    lv_obj_align(btnPage, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    // Set up pages
    const size_t n = page_list.size();
    pages.reserve(n);
    const char **matrix = new const char *[n * 2 + 1];
    size_t i = 0;
    for (auto p : page_list) {
      lv_obj_t *obj = lv_page_create(lv_scr_act(), NULL);
      pages[i] = {p, obj};
      page_map[p->get_name()] = i;
      matrix[i * 2] = p->get_name().c_str();
      matrix[i * 2 + 1] = "\n";
      lv_page_set_style(obj, LV_PAGE_STYLE_BG, &style_page_right);
      lv_obj_set_size(obj, LV_HOR_RES - (LV_DPI / 2), LV_VER_RES);
      lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
      lv_obj_set_hidden(obj, true);
    }
    matrix[n * 2 - 1] = "";
    if (n != 0) {
      lv_obj_set_hidden(pages[cur_page].second, false);
    }

    // Set up buttons
    static lv_style_t style_btn_rel;
    static lv_style_t style_btn_pr;
    static lv_style_t style_btn_sel_rel;
    static lv_style_t style_btn_sel_pr;

    lv_style_copy(&style_btn_rel, &lv_style_btn_rel);
    style_btn_rel.body.main_color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    style_btn_rel.body.grad_color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    style_btn_rel.body.border.color = LV_COLOR_MAKE(0x50, 0x50, 0x50);
    style_btn_rel.body.border.width = 1;
    style_btn_rel.body.border.opa = LV_OPA_COVER;
    style_btn_rel.body.radius = 0;

    lv_style_copy(&style_btn_pr, &style_btn_rel);
    style_btn_pr.body.main_color = LV_COLOR_GRAY;
    style_btn_pr.body.grad_color = LV_COLOR_GRAY;
    style_btn_pr.text.color = LV_COLOR_MAKE(0xbb, 0xd5, 0xf1);

    lv_style_copy(&style_btn_sel_rel, &style_btn_rel);
    style_btn_sel_rel.body.main_color = LV_COLOR_MAKE(0x40, 0x40, 0x40);
    style_btn_sel_rel.body.grad_color = LV_COLOR_MAKE(0x40, 0x40, 0x40);

    lv_style_copy(&style_btn_sel_pr, &style_btn_pr);

    lv_obj_t *btnMap = lv_btnm_create(btnPage, NULL);
    lv_page_glue_obj(btnMap, true);
    lv_btnm_set_map(btnMap, matrix);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_REL, &style_btn_rel);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_PR, &style_btn_pr);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_TGL_REL, &style_btn_sel_rel);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_TGL_PR, &style_btn_sel_pr);
    lv_btnm_set_toggle(btnMap, true, 0);
    lv_btnm_set_action(btnMap, [](lv_obj_t *btnm, const char *txt) {
      size_t new_page = page_map[txt];
      if (new_page != cur_page) {
        lv_obj_set_hidden(pages[cur_page].second, true);
        pages[cur_page].first->closed();
        cur_page = new_page;
        pages[cur_page].first->opened();
        lv_obj_set_hidden(pages[cur_page].second, false);
        task->notify();
      }
      return LV_RES_OK;
    });
    lv_obj_align(btnMap, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_size(btnMap, LV_DPI / 2, LV_DPI / 2 * n);

    // Start background task
    task = new Task([](void *) {
      while (true) {
        pages[cur_page].first->cycle();
        delay(100);
      }
    });
  }
}
