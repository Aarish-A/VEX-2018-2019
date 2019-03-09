#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../button.hpp"
#include "../controls.hpp"
#include "../libraries/util.hpp"

class Angler final : public Subsystem {
public:
  static const uint8_t STATE_MOVE_MANUAL= 0x10;
  static const uint8_t STATE_HOLD = 0x11;

  static bool angler_enabled;

  // Power & Position Constants 
  static const int ANGLER_DZ = 50;
  static constexpr double ANGLER_BOT_LIM_POS = 40;
  static constexpr double ANGLER_TOP_LIM_POS = 320;

private:
  int32_t angler_joy, angler_joy_lst;
  pros::Motor& angler_motor;

public:
  Angler(std::string subsystem_name, pros::Motor& angler_motor);

  void set_state(uint8_t new_state) override;
  void update() override;
  void enable() override;
};
