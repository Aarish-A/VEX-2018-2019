#pragma once
#include "main.h"
#include "../logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../libraries/angle_target.hpp"
#include "../tracking.hpp"

class Drive final : public Subsystem {
public:
  static const uint8_t STATE_DRIVER_CONTROL = 0x10;
  static const uint8_t STATE_AUTO_CONTROL = 0x11;

private:
  pros::Motor& fl_motor;
  pros::Motor& fr_motor;
  pros::Motor& bl_motor;
  pros::Motor& br_motor;
  pros::ADIAnalogIn& pole_poti;
  pros::ADIEncoder& enc_l;
  pros::ADIEncoder& enc_r;

  static constexpr double GEAR_RATIO = 60.0 / 84.0;
  static constexpr double TPR = 360.0 * GEAR_RATIO;
  static constexpr double WHEEL_DIAMETER = 3.95;

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

  friend void drive_move(void* _params);
  friend void drive_turn(void* _params);
  friend void drive_task_stop_function();

public:
  /* Constructor */
  Drive(std::string subsystem_name, uint8_t default_state, pros::Motor& front_left_motor, pros::Motor& front_right_motor, pros::Motor& back_left_motor, pros::Motor& back_right_motor, pros::ADIAnalogIn& pole_poti, pros::ADIEncoder& enc_l, pros::ADIEncoder& enc_r);

  /* Public Functions */
  void update() override;

  void driver_set(int8_t x, int8_t y, int8_t a);

  void reset_global_angle();
  void flatten_against_wall(bool forward = true, bool hold = true, uint8_t hold_power = 15);
  void align_with_pole(uint16_t poti_zero = 2730);
  bool moving();
  void set_error(double error);
};
