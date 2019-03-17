#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"

class Capper final : public Subsystem {
public:
  static const uint8_t STATE_AUTO_CONTROL = 0x10;
  static const uint8_t STATE_HOLD = 0x11;

  static constexpr double GEAR_RATIO = 5.0 / 1.0;
  static constexpr uint16_t PICKUP_POSITION = 0 * Capper::GEAR_RATIO;
  static constexpr uint16_t CAP_FLIP_POSITION = 31 * Capper::GEAR_RATIO;
  static constexpr uint16_t CARRY_POSITION = 46 * Capper::GEAR_RATIO;
  static constexpr uint16_t FLAG_FLIP_POSITION = 60 * Capper::GEAR_RATIO;

  static constexpr uint16_t CAP_START_POSITION = 110 * Capper::GEAR_RATIO;
  static constexpr uint16_t CAP_MID_POSITION = 120 * Capper::GEAR_RATIO;
  static constexpr uint16_t CAP_END_POSIITON = 202 * Capper::GEAR_RATIO;

private:
  pros::Motor& capper_motor;

  int8_t target_power = 0;
  uint8_t target_velocity = 0;
  uint8_t error_threshold = 5;
  uint32_t move_timeout = 1200;
  bool hold = false;

  /* Private Functions */
  void set_state(uint8_t new_state) override;

public:
  /* Constructor */
  Capper(std::string subsystem_name, uint8_t default_state, pros::Motor& capper_motor);

  /* Public Functions */
  void update() override;

  void move_to_power(double target, int8_t power, bool hold = true, uint8_t error_threshold = 5);
  void move_to_velocity(double target, uint8_t velocity, bool hold = true, uint8_t error_threshold = 5);

  void pickup_cap();
  void move_to_cap_flip();
  void move_to_flag_flip();
  void start_capping();
};
