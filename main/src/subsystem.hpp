#pragma once
#include "main.h"
#include "logs.hpp"

class Subsystem {
public:
  static const uint8_t STATE_IDLE = 0x00;
  static const uint8_t STATE_RESET = 0x01;

protected:
  std::string subsystem_name;
  std::string state_names[32];

  uint8_t state = STATE_IDLE;
  uint8_t last_state = STATE_RESET;
  uint32_t state_change_time;
  uint32_t state_timeout_time;
  double state_timeout_velocity;

  double power;
  double position;
  double target;
  double error;
  double velocity;

public:
  Subsystem();

  // Non-Virtual Functions
  double get_position(); // Return the current position
  double get_target(); // Return the current target (if there is any)
  double get_error(); // Return the error between it's current position and target (if there is any)
  double get_power(); // Returns the current power of the subsystem, if applicable
  double get_velocity(); // Returns the current velocity of the subsystem

  void operator= (uint8_t new_state); // Sets the state by doing Subsystem = Subsystem::State

  // Virtual Functions
  virtual void change_state(uint8_t new_state); // Changes the state and sets the state variables
  virtual void reset(); // Resets subsystem through state machine
  virtual void update() = 0; // Updates state machine, should be run in main loop

  virtual void disable(); // Puts subsystem into the state where it should be when disabled
  virtual void enable() = 0; // Puts subsystem in the state where it should be when enabled

  virtual void set_target(double target) = 0; // Sets the target and moves into the required state
  virtual void set_power(double power) = 0; // Sets the power and moves into the required state
  virtual void set_velocity(double velocity) = 0; // Sets the velocity and moves into the required state
};
