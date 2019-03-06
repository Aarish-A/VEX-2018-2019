#pragma once
#include "../libraries/subsystem.hpp"
#include "../button.hpp"
#include "../controls.hpp"

class Intake : public Subsystem {
private:
  pros::Motor intake_motor;

  enum State {
    state_idle,
    state_reset,
    state_pickup,
    state_cap_flip,
    state_jam
  };

public:
  Intake(pros::Motor& intake_motor);

  void change_state(State new_state);
  void update();
  void reset();

  void enable();
  void disable();

  void set_power();
  void set_velocity();
};

virtual void change_state(State new_state); // Changes the state and sets the state variables
virtual void update() = 0; // Updates state machine, should be run in main loop
virtual void reset(); // Resets subsystem through state machine

virtual void enable() = 0; // Puts subsystem in the state where it should be when enabled

double get_current(); // Return the current position
double get_target(); // Return the current target (if there is any)
double get_error(); // Return the error between it's current position and target (if there is any)

virtual void set_target(double target) = 0; // Sets the target and moves into the required state
virtual void set_power(double power) = 0; // Sets the power and moves into the required state
virtual void set_velocity(double velocity) = 0; // Sets the velocity and moves into the required state
void operator= (State new_statestate); // Sets the state by doing Subsystem = Subsystem::State
