#pragma once
#include "main.h"
#include "../libraries/logs.hpp"
#include "../libraries/subsystem.hpp"
#include "../libraries/angle_target.hpp"
#include "../libraries/tracking.hpp"

class Drive final : public Subsystem {
public:
  static const uint8_t STATE_DRIVER_CONTROL = 0x10;
  static const uint8_t STATE_AUTO_CONTROL = 0x11;
  static const uint8_t STATE_DRIVE_LOCK = 0x12;
  static const uint8_t STATE_TURN_BRAKE = 0x13;
  double get_global_angle();
  void set_power(double x, double y, double a);
  void set_power(double power);

  static int8_t turn_curve[256];
  static double drive_turn_coefficient;

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
  const uint8_t DRIVE_TURN_BRAKE_POWER = 10;
  uint32_t above_turn_brake_threshold = 0;

  int8_t x, y, a, last_a;
  bool locked = false;

  /* Private Functions */
  void set_vel(double x, double y, double a);
  void set_vel(double velocity);
  void set_side_power(double left, double right);
  void set_side_vel(double left, double right);
  void brake();
  void set_state(uint8_t new_state) override;

  friend void autonomous();
  friend void climb_on_platform();
  friend void drive_move(void* _params);
  friend void drive_turn(void* _params);
  friend void drive_task_stop_function();
  friend void drive_move_async(double dist_target, double angle_target, bool brake, uint8_t max_power, int8_t start_power, bool decel);
  // friend void drive_turn_async(const AngleTarget& target);
  friend void drive_turn_async(FixedAngleTarget target);
  friend void drive_turn_sync(FixedAngleTarget target);
  friend void drive_turn_async(PointAngleTarget target);
  friend void drive_turn_sync(PointAngleTarget target);
  friend void sweep_turn(const AngleTarget& target, double radius, bool forwards, double post_distance, bool clockwise, bool brake, int max_power);
  friend void drive_turn_side(const AngleTarget& target, double kP, double offset, bool forwards);
  friend void shot_queue_handle_intern();
  friend void shot_queue_handle(void* param);
  friend void cap_on_pole_stop_function();

public:
  /* Constructor */
  Drive(std::string subsystem_name, uint8_t default_state, pros::Motor& front_left_motor, pros::Motor& front_right_motor, pros::Motor& back_left_motor, pros::Motor& back_right_motor, pros::ADIAnalogIn& pole_poti, pros::ADIEncoder& enc_l, pros::ADIEncoder& enc_r);

  /* Public Functions */
  void update() override;

  void driver_set(int8_t x, int8_t y, int8_t a);

  void reset_global_angle();
  double read_global_angle();
  void flatten_against_wall(bool forward = true, bool hold = true, int8_t hold_power = 15);
  void flatten_against_wall_base(bool forward, int8_t power, bool hold, int8_t hold_power);
  void align_with_pole(uint16_t poti_zero = 2750);
  bool moving();

  void set_error(double error);
  void set_target(double target);

  void wait_for_stop();
  void wait_for_distance(double target_distance);
  void wait_for_angle(double target_angle);

  void lock();
  void unlock();
};
