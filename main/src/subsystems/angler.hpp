#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"

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

  static const uint16_t TOP_LIMIT_POSITION = 320;
  static const uint16_t BOTTOM_LIMIT_POSITION = 40;

  int8_t power;
  uint8_t error_threshold = 5;
  uint32_t move_timeout = 800;

  /* Private Functions */
  void set_state(uint8_t new_state) override;

public:
  /* Constructor */
  Angler(std::string subsystem_name, uint8_t default_state, pros::Motor& angler_motor);

  /* Public Functions */
  void update() override;

  void driver_set(int8_t power);
  void move_to(double target, uint32_t timeout = 800, uint8_t error_threshold = 5);
  bool moving_to_target();
};
