#include "util.hpp"

int set_dz(int val, int dz) {
  return (abs(val) < dz ? 0 : val);
}
