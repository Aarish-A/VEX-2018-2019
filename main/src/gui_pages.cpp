#include "gui_pages.hpp"
#include "settings.hpp"
#include "config.hpp"

using namespace std;

bool disable_controls = false;

void init_gui() {
  class ShotPositionsPage final : public piln::gui::DisplayPage {
    const char *matrix[NUM_SHOT_POS * 2 + 1] = {};
    double buffer[NUM_SHOT_POS] = {};
    size_t selected = NUM_SHOT_POS;
    lv_obj_t *btn_map;
    lv_obj_t *done_btn;

	public:
    ShotPositionsPage() : DisplayPage("Shot Pos") {}

		void init(lv_obj_t *path) override {
      static ShotPositionsPage *_this = this;
      matrix[0] = "";
      btn_map = lv_btnm_create(path, NULL);
      lv_page_glue_obj(btn_map, true);
      lv_btnm_set_map(btn_map, matrix);
      lv_btnm_set_action(btn_map, [](lv_obj_t *btnm, const char *txt) {
        lv_obj_set_hidden(done_btn, false);
        _this->selected = atoi(txt);
        disable_controls = true;
        return LV_RES_OK;
      });
      lv_obj_align(btn_map, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
      done_btn = lv_btn_create(path, NULL);
      lv_obj_align(btn_map, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
      lv_obj_set_hidden(done_btn, true);
      lv_obj_t *label = lv_label_create(done_btn, NULL);
      lv_label_set_static_text(label, "Done");
      lv_btn_set_action(done_btn, LV_BTN_ACTION_CLICK, [](lv_obj_t *btn) {
        lv_obj_set_hidden(done_btn, true);
        _this->selected = NUM_SHOT_POS;
        disable_controls = false;
        return LV_RES_OK;
      });
      for (size_t i = 0; i < NUM_SHOT_POS; ++i) {
        buffer[i] = piln::settings::get_double("shot-" + to_string(i));
      }
		}

    void cycle() override {
      if (selected < NUM_SHOT_POS) {
        double value = buffer[selected];
        if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) selected += 0.1;
        if (ctrler.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) selected -= 0.1;
        piln::settings::set_double("shot-" + to_string(selected), value);
      }
      for (size_t i = 0; i < NUM_SHOT_POS; ++i) {
        double value = piln::settings::get_double("shot-" + to_string(i));
        buffer[i] = value;
        delete[] matrix[i * 2];
        auto str = to_string(i) + ": " + to_string(value);
        if (selected == i) str += "*";
        char *s = new char[str.length() + 1];
        strcpy(s, str.c_str());
        matrix[i * 2] = s;
        matrix[i * 2 + 1] = "\n";
      }
      lv_btnm_set_map(btn_map, matrix);
    }

    void closed() override {
      lv_obj_set_hidden(done_btn, true);
      selected = NUM_SHOT_POS;
      disable_controls = false;
    }
	};

	piln::gui::DisplayHandler::init({
    new ShotPositionsPage()
  });
}
