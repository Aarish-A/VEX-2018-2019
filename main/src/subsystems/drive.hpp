#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../button.hpp"
#include "../controls.hpp"
#include "../libraries/util.hpp"

class Drive final : public Subsystem {
public:
  static const uint8_t STATE_DRIVER_CONTROL = 0x10;
private:
  pros::Motor& fl_motor;
  pros::Motor& fr_motor;
  pros::Motor& bl_motor;
  pros::Motor& br_motor;

  static const uint8_t THROTTLE_DEADZONE = 10;
  static const uint8_t TURN_DEADZONE = 30;
  static const uint8_t STRAFE_DEADZONE = 10;


  void set_state(uint8_t new_state) override;
  void set(double x, double y, double a);
  void set(double power);
  void set_vel(double x, double y, double a);
  void set_vel(double velocity);
  void set_side(double left, double right);
  void set_side_vel(double left, double right);

public:
  Drive(std::string subsystem_name, pros::Motor& front_left_motor, pros::Motor& front_right_motor, pros::Motor& back_left_motor, pros::Motor& back_right_motor);

  void update() override;

  void enable() override;
};
