#pragma once
#include "main.h"
#include "../libraries/logs.hpp"
#include "../libraries/subsystem.hpp"

class Puncher final : public Subsystem {
public:
  static const uint8_t STATE_CANCEL = 0x10;
  static const uint8_t STATE_LOADING = 0x11;
  static const uint8_t STATE_LOADED = 0x12;
  static const uint8_t STATE_PULLBACK = 0x13;
  static const uint8_t STATE_BOLT_WAIT = 0x14;

private:
  pros::Motor& puncher_motor;
  pros::ADILineSensor& ball_sensor;
  bool reset_finished = false;

  static constexpr double GEAR_RATIO = 5.0 / 3.0;
  static constexpr double OFFSET = 50 * GEAR_RATIO;
  static constexpr double HOLD = 180 * GEAR_RATIO;
  static constexpr double TPR = 360 * GEAR_RATIO;
  static constexpr double NO_RETURN = 50 * GEAR_RATIO;
  static constexpr double BALL_CHECK_START = TPR - (HOLD + 150);
  static constexpr uint32_t BOLT_WAIT_TIME = 50;
  static constexpr uint32_t BALL_THRESHOLD = 2700;
  static constexpr uint32_t BALL_OFF_TIME = 100;
  static constexpr int8_t HOLD_POWER = 6;

  uint32_t shot_number = 0;
  uint32_t ball_on_time = 0;
  uint32_t ball_sensor_value = 2900;
  int32_t ball_check_target = 0;
  int32_t point_of_no_return = 0;
  bool ball_on = false;
  bool cancelling_shot = false;

  void set_power(int8_t power);
  void move_to(double target, double velocity = 200);
  void set_state(uint8_t new_state) override;

public:
  Puncher(std::string subsystem_name, uint8_t default_state, pros::Motor& puncher_motor, pros::ADILineSensor& bs);

  void update() override;
  void enable() override;
  void shoot();
  void cancel_shot();
  bool shooting();
  void wait_for_shot_finish();
  void set_holding();
};
