#include "main.h"
#include <functional>

namespace piln::gui {
  class DisplayHandler {
  public:
    struct DisplayPage {
      const char *name;
      std::function<void (lv_obj_t *path)> initPage;
      lv_obj_t *page;
    };

  private:
    DisplayHandler() = delete;

    static DisplayPage pages[3];
    static constexpr int PAGE_COUNT = sizeof(pages) / sizeof(DisplayPage);
    static const char *map[PAGE_COUNT * 2];
    static int curPage;

    static lv_res_t handleBtn(lv_obj_t *btnm, const char *txt);
    static void handleLCD(void *param);
    static pros::Task *lcdTask;

    static lv_style_t style_label;

    public:
      static void init();
  };
}
