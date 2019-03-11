#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"

class Capper final : public Subsystem {
public:
  static const uint8_t STATE_PICKUP = 0x10;
  static const uint8_t STATE_CARRY = 0x11;
  static const uint8_t STATE_CAP_START = 0x12;
  static const uint8_t STATE_CAP_INTERIM = 0x13;
  static const uint8_t STATE_CAP_END = 0x14;
  static const uint8_t STATE_CAP_FINISHED = 0x15;

  static constexpr uint16_t PICKUP_POSITION = 0;
  static constexpr uint16_t CARRY_POSITION = 310;
  static constexpr uint16_t CAP_START_POSITION = 550;
  static constexpr uint16_t CAP_INTERIM_POSITION = 800;
  static constexpr uint16_t CAP_END_POSIITON = 1010;

private:
  pros::Motor& capper_motor;

  static constexpr double GEAR_RATIO = 5.0 / 1.0;
  uint8_t error_threshold = 5;
  uint32_t move_timeout = 800;

  /* Private Functions */
  void set_state(uint8_t new_state) override;

public:
  /* Constructor */
  Capper(std::string subsystem_name, uint8_t default_state, pros::Motor& capper_motor);

  /* Public Functions */
  void update() override;

  void pickup_cap();
  void start_capping();
  void finish_capping();
  void move_to_bottom();

  bool ready_to_cap();
};
