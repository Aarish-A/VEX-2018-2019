#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../button.hpp"
#include "../controls.hpp"
#include "../libraries/util.hpp"

class Angler final : public Subsystem {
public:
  static const uint8_t STATE_MOVE_MANUAL = 0x10;
  static const uint8_t STATE_HOLD = 0x11;

private:
  pros::Motor& angler_motor;

  static const uint8_t DEADZONE = 40;

public:
  Angler(std::string subsystem_name, pros::Motor& angler_motor);

  void set_state(uint8_t new_state) override;
  void update() override;
  void enable() override;
};
