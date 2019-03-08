#pragma once
#include "main.h"
#include "logs.hpp"
#include "subsystem.hpp"
#include "button.hpp"
#include "controls.hpp"

class Intake final : public Subsystem {
public:
  static const uint8_t STATE_PICKUP = 0x10;
  static const uint8_t STATE_CAP_FLIP = 0x11;
  static const uint8_t STATE_JAM = 0x12;

private:
  pros::Motor& intake_motor;

  void change_state(uint8_t new_state) override;

public:
  Intake(std::string subsystem_name, pros::Motor& intake_motor);

  void update() override;
  
  void enable() override;

  void set_target(double target) override;
  void set_power(double power) override;
  void set_velocity(double velocity) override;
};
