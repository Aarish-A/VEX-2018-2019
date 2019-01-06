#pragma once

#include <cstdint>
#include <string>

namespace piln::settings {
  void init();
  int32_t get_int(const std::string &key, int32_t default_value = 0);
  double get_double(const std::string &key, double default_value = 0.0);
  void set_int(const std::string &key, int32_t value);
  void set_double(const std::string &key, double value);
}
