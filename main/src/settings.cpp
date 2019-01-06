#include "settings.hpp"
#include <map>

using namespace std;

namespace {
  class SettingValue {
  public:
    virtual ~SettingValue() = default;
  };

  class IntSettingValue final : public SettingValue {
  public:
    int32_t value;

    IntSettingValue(int32_t value) : value(value) {}
  };

  class DoubleSettingValue final : public SettingValue {
  public:
    double value;

    DoubleSettingValue(double value) : value(value) {}
  };

  map<string, SettingValue *> settings_map;
}

namespace piln::settings {
  void init() {
  }

  int32_t get_int(const std::string &key, int32_t default_value) {
    auto iter = settings_map.find(key);
    if (iter == settings_map.end()) {
      settings_map[key] = new IntSettingValue(default_value);
      return default_value;
    }
    auto value = dynamic_cast<IntSettingValue *>(iter->second);
    if (value) return value->value;
    else {
      delete iter->second;
      iter->second = new IntSettingValue(default_value);
      return default_value;
    }
  }

  double get_double(const std::string &key, double default_value) {
    auto iter = settings_map.find(key);
    if (iter == settings_map.end()) {
      settings_map[key] = new DoubleSettingValue(default_value);
      return default_value;
    }
    auto value = dynamic_cast<DoubleSettingValue *>(iter->second);
    if (value) return value->value;
    else {
      delete iter->second;
      iter->second = new DoubleSettingValue(default_value);
      return default_value;
    }
  }

  void set_int(const std::string &key, int32_t value) {
    auto iter = settings_map.find(key);
    if (iter == settings_map.end()) {
      settings_map[key] = new IntSettingValue(value);
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
