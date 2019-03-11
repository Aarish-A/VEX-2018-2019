#pragma once
#include "main.h"
#include "../logs.hpp"

class Subsystem {
public:
  static const uint8_t STATE_DISABLED = 0x00;
  static const uint8_t STATE_RESET = 0x01;

protected:
  std::string subsystem_name;
  std::string state_names[32];
  static uint8_t number_of_subsystems;
  static Subsystem* subsystems[8];

  uint8_t state = STATE_DISABLED;
  uint8_t last_state = STATE_DISABLED;
  uint32_t state_change_time;
  uint32_t velocity_exceeded_time;
  uint8_t DEFAULT_STATE = STATE_DISABLED;

  double position;
  double target;
  double error;
  double velocity;
  double last_velocity;

  bool timed_out(uint32_t timeout);
  bool above_vel_threshold(double velocity, uint32_t timeout);
  bool below_vel_threshold(double velocity, uint32_t timeout);

  virtual void set_state(uint8_t new_state); // Changes the state and sets the state variables

public:
  Subsystem(std::string subsystem_name, uint8_t default_state);

  // Non-Virtual Functions
  double get_position(); // Return the current position
  double get_target(); // Return the current target (if there is any)
  double get_error(); // Return the error between it's current position and target (if there is any)
  double get_power(); // Returns the current power of the subsystem, if applicable
  double get_velocity(); // Returns the current velocity of the subsystem

  void disable(); // Puts subsystem disabled state
  void enable(); // Puts subsystem in the state where it should be when enabled
  bool enabled();
  bool disabled();
  void reset(); // Resets subsystem through state machine (calibrate)

  // Virtual Functions

  // Pure-Virtual Functions
  virtual void update() = 0; // Updates state machine, should be run in main loop

  // Static Functions
  static void disable_all();
  static void reset_all();
};
