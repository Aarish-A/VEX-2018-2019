#pragma once
#include "main.h"
#include "../libraries/subsystem.hpp"

class Intake final : public Subsystem {
public:
  static const uint8_t STATE_OFF = 0x10;
  static const uint8_t STATE_FORWARDS = 0x11;
  static const uint8_t STATE_BACKWARDS = 0x12;
  static const uint8_t STATE_JAM = 0x13;

private:
  pros::Motor& intake_motor;
  int8_t power;
  void set_state(uint8_t new_state) override;
  void set_power(int8_t power);

public:
  Intake(std::string subsystem_name, uint8_t default_state, pros::Motor& intake_motor);

  void update() override;
  void intake(int8_t power = 127);
  void outtake(int8_t power = -80);
  void stop();
  bool off();
};
