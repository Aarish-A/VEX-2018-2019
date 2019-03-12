#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../libraries/angle_target.hpp"

class Drive final : public Subsystem {
public:
  static const uint8_t STATE_DRIVER_CONTROL = 0x10;
  static const uint8_t STATE_AUTO_CONTROL = 0x11;

  static const uint8_t THROTTLE_DEADZONE = 10;
  static const uint8_t TURN_DEADZONE = 30;
  static const uint8_t STRAFE_DEADZONE = 10;

private:
  pros::Motor& fl_motor;
  pros::Motor& fr_motor;
  pros::Motor& bl_motor;
  pros::Motor& br_motor;
  pros::ADIAnalogIn& pole_poti;

  static constexpr double GEAR_RATIO = 60.0 / 84.0;
  static constexpr double TPR = 360.0 * GEAR_RATIO;
  static constexpr double WHEEL_DIAMETER = 3.95;

  pros::Task* move_alg_task = nullptr;

  int8_t x, y, a;

  /* Private Functions */
  void set_power(double x, double y, double a);
  void set_power(double power);
  void set_vel(double x, double y, double a);
  void set_vel(double velocity);
  void set_side_power(double left, double right);
  void set_side_vel(double left, double right);
  void brake();
  double get_global_angle();

  void set_state(uint8_t new_state) override;


public:
  /* Constructor */
  Drive(std::string subsystem_name, uint8_t default_state, pros::Motor& front_left_motor, pros::Motor& front_right_motor, pros::Motor& back_left_motor, pros::Motor& back_right_motor, pros::ADIAnalogIn& pole_poti);

  /* Public Functions */
  void update() override;

  void driver_set(int8_t x, int8_t y, int8_t a);

  void reset_global_angle();
  void move(double distance, uint8_t max_power = 200, bool brake = true, double angle_target = 1000);
  void move_async(double distance, uint8_t max_power = 200, bool brake = true, double angle_target = 1000);
  void turn(const AngleTarget& target);
  void turn_async(const AngleTarget& target);
  void flatten_against_wall(bool forward, bool hold = true, uint8_t hold_power = 15);
};
