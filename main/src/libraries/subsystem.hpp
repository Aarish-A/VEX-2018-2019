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
  void change_state(State state); // Changes the state and sets the state variables
  void reset(); // Resets subsystem through state machine
  virtual void update() = 0; // Updates state machine, should be run in main loop

  virtual void enable() = 0; // Puts subsystem in the state where it should be when enabled
  virtual void disable() = 0; // Puts subsystem into the state where it should be when disabled

  double get_current(); // Return the current position
  double get_target(); // Return the current target (if there is any)
  double get_error(); // Return the error between it's current position and target (if there is any)

  virtual void set_target(double target) = 0; // Sets the target and moves into the required state
  virtual void set_power(double power) = 0; // Sets the power and moves into the required state
  virtual void set_velocity(double velocity) = 0; // Sets the velocity and moves into the required state
  void operator= (State state); // Sets the state by doing Subsystem = Subsystem::State
};

class Tracking {
private:
  pros::ADIEncoder &encL, &encR, &encS;
  int encLLst, encRLst, encSLst;
  double aRst;

  double xLst, yLst, aLst;
  uint32_t velLstTime;
/*
protected:
  void taskImpl() override;
*/
public:
  double x, y, a = 0;
  double xVel = 0, yVel = 0, aVel = 0;
  vector velLocal;

  Tracking(pros::ADIEncoder &encL, pros::ADIEncoder &encR, pros::ADIEncoder &encS, double x = 0, double y = 0, double a = 0);

  void task();

  void update();
  void reset(double x = 0, double y = 0, double a = 0);
  vector position() const;
  vector velocity() const;
};
