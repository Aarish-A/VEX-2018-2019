#pragma once

#include "main.h"
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <initializer_list>

namespace piln::gui {
  class DisplayPage {
    const std::string name;

  public:
    DisplayPage(const std::string &name);
    virtual void init(lv_obj_t *path) = 0;
    virtual void cycle();
    virtual void opened();
    virtual void closed();
    virtual ~DisplayPage();

    constexpr const std::string &get_name() const {
      return name;
    }
  };

  class DisplayHandler final {
  private:
    static std::vector<std::pair<DisplayPage *, lv_obj_t *>> pages;
    static std::map<std::string, size_t> page_map;
    static size_t cur_page;
    static pros::Task *task;

  public:
    DisplayHandler() = delete;
    static void init(std::initializer_list<DisplayPage *> page_list);
  };
}
