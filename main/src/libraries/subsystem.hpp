#pragma once
#include "main.h"

class Subsystem {

private:
  enum State { state_idle, state_reset };
  State state = state_idle;
  State last_state = state_reset;
  uint32_t state_change_time;
  uint32_t state_timeout_time;
  double state_timeout_velocity;

  double current;
  double target;
  double error;


public:
  virtual void change_state(State new_state); // Changes the state and sets the state variables
  virtual void update() = 0; // Updates state machine, should be run in main loop
  virtual void reset(); // Resets subsystem through state machine

  virtual void enable() = 0; // Puts subsystem in the state where it should be when enabled
  virtual void disable() = 0; // Puts subsystem into the state where it should be when disabled

  double get_current(); // Return the current position
  double get_target(); // Return the current target (if there is any)
  double get_error(); // Return the error between it's current position and target (if there is any)

  virtual void set_target(double target) = 0; // Sets the target and moves into the required state
  virtual void set_power(double power) = 0; // Sets the power and moves into the required state
  virtual void set_velocity(double velocity) = 0; // Sets the velocity and moves into the required state
  void operator= (State new_state); // Sets the state by doing Subsystem = Subsystem::State
};
