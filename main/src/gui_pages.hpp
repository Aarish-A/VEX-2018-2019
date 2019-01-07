#pragma once

#include "gui.hpp"
#include <atomic>

constexpr size_t NUM_SHOT_POS = 4;

extern std::atomic<bool> disable_controls;

void init_gui();
