#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../button.hpp"
#include "../controls.hpp"

class Intake final : public Subsystem {
public:
  static const uint8_t STATE_PICKUP = 0x10;
  static const uint8_t STATE_CAP_FLIP = 0x11;
  static const uint8_t STATE_JAM = 0x12;

private:
  pros::Motor& intake_motor;

public:
  Intake(std::string subsystem_name, pros::Motor& intake_motor);

  void set_state(uint8_t new_state) override;
  void update() override;
  void enable() override;
};
