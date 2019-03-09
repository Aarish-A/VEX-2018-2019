#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../button.hpp"
#include "../controls.hpp"
#include "../libraries/util.hpp"

class Angler final : public Subsystem {
public:
  static const uint8_t STATE_DRIVER_CONTROL = 0x10;
  static const uint8_t STATE_AUTO_CONTROL = 0x11;
  static const uint8_t STATE_HOLD = 0x12;

  static const uint16_t PICKUP_POSITION = 85;
  static const uint16_t CAP_PICKUP_POSITION = 220;
  static const uint16_t CAP_FLIP_POSITION = 50;

private:
  pros::Motor& angler_motor;
  static const uint8_t DEADZONE = 40;
  static const uint16_t TOP_LIMIT_POSITION = 320;
  static const uint16_t BOTTOM_LIMIT_POSITION = 40;

  void set_angler_motor_power(double power);

public:
  Angler(std::string subsystem_name, pros::Motor& angler_motor);

  void set_state(uint8_t new_state) override;
  void set_target(double target) override;
  void update() override;
  void enable() override;
};
