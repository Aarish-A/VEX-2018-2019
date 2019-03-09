#pragma once
#include "main.h"
#include "../logs.hpp"

class Subsystem {
public:
  static const uint8_t STATE_IDLE = 0x00;
  static const uint8_t STATE_RESET = 0x01;
  static const uint8_t STATE_DISABLED = 0x02;
  static const uint8_t STATE_MOVE_POS = 0x03;
  static uint8_t number_of_subsystems;
  static Subsystem* subsystems[8];

protected:
  // std::string subsystem_name;
  bool done_reset_accel;
  std::string subsystem_name;
  std::string state_names[32];

  uint8_t state = STATE_IDLE;
  uint8_t last_state = STATE_IDLE;
  uint32_t state_change_time;
  uint32_t state_timeout_time;
  double state_timeout_velocity;
  uint32_t state_timeout_velocity_time;
  double velocity_exceeded_time;

  double power;
  double position;
  double target;
  double error;
  double velocity;
  double last_velocity;

  void set_timeout_time(uint32_t time = 0);
  void set_timeout_velocity(double velocity = 0, uint32_t time = 0);
  bool timed_out();
  bool above_vel_threshold();
  bool below_vel_threshold();

public:
  Subsystem();

  // Non-Virtual Functions
  double get_position(); // Return the current position
  double get_target(); // Return the current target (if there is any)
  double get_error(); // Return the error between it's current position and target (if there is any)
  double get_power(); // Returns the current power of the subsystem, if applicable
  double get_velocity(); // Returns the current velocity of the subsystem
  void operator= (uint8_t new_state); // Sets the state by doing Subsystem = Subsystem::State
  void set_state_target(uint8_t new_state, double target); // Sets the target and moves into the required state
  void set_state_power(uint8_t new_state, double power); // Sets the power and moves into the required state
  void set_state_velocity(uint8_t new_state, double velocity); // Sets the velocity and moves into the required state

  void disable(); // Puts subsystem disabled state
  void reset(); // Resets subsystem through state machine (calibrate)

  // Virtual Functions
  virtual void set_state(uint8_t new_state); // Changes the state and sets the state variables
  virtual void set_target(double target); // Set target only - define in subsystem
  virtual void set_power(double power); // Set power only - define in subsystem
  virtual void set_velocity(double power); // Set velocity only - define in subsystem

  // Pure-Virtual Functions
  virtual void update() = 0; // Updates state machine, should be run in main loop
  virtual void enable() = 0; // Puts subsystem in the state where it should be when enabled

  // Static Functions
  static void disable_all();
  static void reset_all();
};
