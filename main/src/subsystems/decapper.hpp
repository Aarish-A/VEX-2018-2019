#pragma once
#include "main.h"
#include "../libraries/logs.hpp"
#include "../libraries/subsystem.hpp"

class Decapper final : public Subsystem {
public:
  static const uint8_t STATE_AUTO_CONTROL = 0x10;
  static const uint8_t STATE_HOLD = 0x11;
  static const uint8_t STATE_DECAP = 0x12;

  static constexpr double GEAR_RATIO = 5.0 / 1.0;

  static constexpr uint16_t BOTTOM_POSITION = 6 * Decapper::GEAR_RATIO;
  static constexpr uint16_t CARRY_POSITION = 68 * Decapper::GEAR_RATIO;
  static constexpr uint16_t DECAP_POSITION = 165 * Decapper::GEAR_RATIO;

private:
  pros::Motor& decapper_motor;

  int8_t target_power = 0;
  uint8_t target_velocity = 0;
  uint8_t error_threshold = 5;
  uint32_t move_timeout = 1200;
  bool hold = false;

  /* Private Functions */
  void set_state(uint8_t new_state) override;

public:
  /* Constructor */
  Decapper(std::string subsystem_name, uint8_t default_state, pros::Motor& decapper_motor);

  /* Public Functions */
  void update() override;

  void move_to_power(double target, int8_t power, bool hold = true, uint8_t error_threshold = 5);
  void move_to_velocity(double target, uint8_t velocity, bool hold = true, uint8_t error_threshold = 5);
  void next_position();
  void previous_position();

};
