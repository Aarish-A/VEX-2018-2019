#include "lcd.hpp"
#include <cstring>

namespace pi::gui {
  pros::Task *DisplayHandler::lcdTask;
  int DisplayHandler::curPage = 0;
  const char *DisplayHandler::map[];
  lv_style_t DisplayHandler::style_label;

  DisplayHandler::DisplayPage DisplayHandler::pages[] = {
    { "Dbg",
      [](lv_obj_t *page) {
        lv_obj_t *label = lv_label_create(page, NULL);
        lv_label_set_style(label, &style_label);
        lv_label_set_text(label, "Debug Menu");
      }
    },
    { "Shot",
      [](lv_obj_t *page) {
        lv_obj_t *label = lv_label_create(page, NULL);
        lv_label_set_style(label, &style_label);
        lv_label_set_text(label, "Shot Adjust");
      }
    },
    { "Auto",
      [](lv_obj_t *page) {
        lv_obj_t *label = lv_label_create(page, NULL);
        lv_label_set_style(label, &style_label);
        lv_label_set_text(label, "Autonomous Select");
      }
    }
  };

  void DisplayHandler::init() {
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

    for (int i = 0; i < PAGE_COUNT; i++) {
      map[i * 2] = pages[i].name;
      map[(i * 2) + 1] = "\n";
      pages[i].page = lv_page_create(lv_scr_act(), NULL);
      lv_page_set_style(pages[i].page, LV_PAGE_STYLE_BG, &style_page_right);
      lv_obj_set_size(pages[i].page, LV_HOR_RES - (LV_DPI / 2), LV_VER_RES);
      lv_obj_align(pages[i].page, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
      lv_obj_set_hidden(pages[i].page, true);
      const char a[2] = { (char)(49 + i), 0 };
      lv_label_set_text(lv_label_create(pages[i].page, NULL), a);
    }
    map[PAGE_COUNT * 2 - 1] = "";
    lv_obj_set_hidden(pages[curPage].page, false);

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
    lv_btnm_set_map(btnMap, map);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_REL, &style_btn_rel);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_PR, &style_btn_pr);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_TGL_REL, &style_btn_sel_rel);
    lv_btnm_set_style(btnMap, LV_BTNM_STYLE_BTN_TGL_PR, &style_btn_sel_pr);
    lv_btnm_set_toggle(btnMap, true, 0);
    lv_btnm_set_action(btnMap, handleBtn);
    lv_obj_align(btnMap, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_size(btnMap, LV_DPI / 2, LV_DPI / 2 * PAGE_COUNT);

    for (int i = 0; i < PAGE_COUNT; i++) {
      pages[i].initPage(pages[i].page);
    }

    DisplayHandler::lcdTask = new pros::Task(&DisplayHandler::handleLCD);
  }

  lv_res_t DisplayHandler::handleBtn(lv_obj_t *btnm, const char *txt) {
    for (int i = 0; i < PAGE_COUNT; i++) {
      if (!std::strcmp(txt, pages[i].name))
        lv_obj_set_hidden(pages[i].page, false);
      else
        lv_obj_set_hidden(pages[i].page, true);
    }

    return LV_RES_OK;
  }

  void DisplayHandler::handleLCD(void *param) {
    while (true) {
      switch (curPage) {
        case 0:

          break;
      }

      pros::delay(100);
    }
  }
}
