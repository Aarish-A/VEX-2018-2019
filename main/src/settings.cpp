#include "settings.hpp"
#include <map>

using namespace std;

namespace {
  class SettingValue {
  public:
    virtual int32_t get_int(int32_t default_value) = 0;
    virtual double get_double(double default_value) = 0;
  };

  class IntSettingValue final : public SettingValue {
  public:
    int32_t value;

    IntSettingValue(int32_t value) : value(value) {}

    int32_t get_int(int32_t default_value) override {
      return value;
    }

    double get_double(double default_value) override {
      return value;
    }
  };

  class DoubleSettingValue final : public SettingValue {
  public:
    double value;

    DoubleSettingValue(double value) : value(value) {}

    int32_t get_int(int32_t default_value) override {
      return static_cast<int32_t>(value);
    }

    double get_double(double default_value) override {
      return value;
    }
  };

  map<string, SettingValue *> settings_map;
}

namespace piln::settings {
  void init() {
  }

  int32_t get_int(const std::string &key, int32_t default_value) {
    auto iter = settings_map.find(key);
    return iter == settings_map.end() ? default_value : iter->second->get_int(default_value);
  }

  double get_double(const std::string &key, double default_value) {
    auto iter = settings_map.find(key);
    return iter == settings_map.end() ? default_value : iter->second->get_double(default_value);
  }

  void set_int(const std::string &key, int32_t value) {
    auto iter = settings_map.find(key);
    if (iter == settings_map.end()) {
      settings_map[key] = new IntSettingValue(value);
      return;
    }
    auto ptr = dynamic_cast<IntSettingValue *>(iter->second);
    if (ptr) {
      ptr->value = value;
    }
    else {
      delete iter->second;
      iter->second = new IntSettingValue(value);
    }
  }

  void set_double(const std::string &key, double value) {
    auto iter = settings_map.find(key);
    if (iter == settings_map.end()) {
      settings_map[key] = new DoubleSettingValue(value);
      return;
    }
    auto ptr = dynamic_cast<DoubleSettingValue *>(iter->second);
    if (ptr) {
      ptr->value = value;
    }
    else {
      delete iter->second;
      iter->second = new DoubleSettingValue(value);
    }
  }
}
