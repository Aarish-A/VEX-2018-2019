#pragma once
#include "main.h"
#include "../libraries/logs.hpp"
#include "../libraries/subsystem.hpp"

class Angler final : public Subsystem {
public:
  static const uint8_t STATE_DRIVER_CONTROL = 0x10;
  static const uint8_t STATE_AUTO_CONTROL = 0x11;
  static const uint8_t STATE_HOLD = 0x12;
  static const uint8_t STATE_MOVE_HOLD = 0x13;

  static const uint16_t PICKUP_POSITION = 85;
  static const uint16_t CAP_PICKUP_POSITION = 224;
  static const uint16_t CAP_FLIP_POSITION = 40;

private:
  pros::Motor& angler_motor;

  static const uint16_t TOP_LIMIT_POSITION = 320;
  static const uint16_t BOTTOM_LIMIT_POSITION = 40;

  int8_t power;
  uint8_t hold_velocity = 200;
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
  void move_to(double target, uint8_t hold_velocity = 200, uint32_t timeout = 1200, uint8_t error_threshold = 3);
  bool moving_to_target();
  void wait_for_target_reach();
};
