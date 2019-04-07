#pragma once
#include "main.h"
#include "tracking.hpp"
#include "logs.hpp"

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
  uint8_t DEFAULT_STATE = STATE_DISABLED;
  uint32_t state_change_time = 0;
  uint32_t velocity_exceeded_time = 0;

  double position;
  double target;
  double error;
  double velocity;
  double last_velocity;

  /* Protected Non-Virtual Functions */
  bool timed_out(uint32_t timeout); // Return whether or not the subsystem has been in a state for longer than the timeout
  bool above_vel_threshold(double velocity, uint32_t timeout); // Return whether or not the subsystem has been above a velocity for longer than some duration
  bool below_vel_threshold(double velocity, uint32_t timeout); // Return whether or not the subsystem has been below a velocity for longer than some duration

  /* Protected Virtual Functions */
  virtual void set_state(uint8_t new_state); // Changes the state and sets the state variables

public:
  /* Constructor */
  Subsystem(std::string subsystem_name, uint8_t default_state);

  /* Public Non-Virtual Functions */
  double get_position(); // Return the current position
  double get_target(); // Return the current target (if there is any)
  double get_error(); // Return the error between it's current position and target (if there is any)
  double get_power(); // Returns the current power of the subsystem, if applicable
  double get_velocity(); // Returns the current velocity of the subsystem

  void disable(); // Puts subsystem disabled state
  bool enabled(); // Returns whether or not the subsystem is not in disabled
  bool disabled(); // Returns whether or not the subsystem is in disabled
  void reset(); // Resets subsystem through state machine (calibrate)

  /* Public Virtual Functions */
  virtual void enable(); // Puts subsystem in the state where it should be when enabled

  /* Public Pure-Virtual Functions */
  virtual void update() = 0; // Updates state machine, should be run in main loop

  // Static Functions
  static void update_all();
  static void enable_all();
  static void disable_all();
  static void reset_all();
  static bool any_resetting();
};
