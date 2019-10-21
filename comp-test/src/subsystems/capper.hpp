#pragma once
#include "main.h"
#include "../libraries/logs.hpp"
#include "../libraries/subsystem.hpp"

class Capper final : public Subsystem {
public:
  static const uint8_t STATE_AUTO_CONTROL = 0x10;
  static const uint8_t STATE_HOLD = 0x11;
  static const uint8_t STATE_CAPPING = 0x12;
  static const uint8_t STATE_FINISH_CAPPING = 0x13;

  static constexpr double GEAR_RATIO = 5.0 / 1.0;

  static constexpr uint16_t PICKUP_POSITION = 7 * Capper::GEAR_RATIO;
  static constexpr uint16_t CAP_FLIP_POSITION = 37 * Capper::GEAR_RATIO;
  static constexpr uint16_t CARRY_POSITION = 55 * Capper::GEAR_RATIO;
  static constexpr uint16_t FLAG_FLIP_POSITION = 60 * Capper::GEAR_RATIO;
  static constexpr uint16_t CAPPING_HOLD_POSITION = 190 * Capper::GEAR_RATIO;
  static constexpr uint16_t CAPPING_END_POSIITON = 240 * Capper::GEAR_RATIO;

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
  void wait_for_target_reach();

  void move_to_pickup();
  void move_to_cap_flip(bool holding_cap = true, double velocity = 125);
  void move_to_flag_flip(double velocity = 65);
  void pickup_cap(bool expansion_zone = false);
  void start_capping();
  void finish_capping();
  bool capping();
  bool at_pickup_position();
};
