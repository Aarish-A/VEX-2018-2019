#ifndef TRACKING_H
#define TRACKING_H

#include "pros/adi.h"
#include "pros/adi.hpp"

/* Constant Definitions*/
static constexpr double pi = 3.1415926535897932;
const float WHEEL_DIAMETER_IN_LR = 2.783;
const float WHEEL_DIAMETER_IN_B = 2.783; // 2.843
const float L_DISTANCE_IN = 4.766; //6.8198
const float R_DISTANCE_IN = 4.766; //6.8198
const float B_DISTANCE_IN = 7;

// The number of tick per rotation of the tracking wheel
const float TICKS_PER_ROTATION = 360.0;

// Used internally by trackPosition
const double SPIN_TO_IN_LR  = (WHEEL_DIAMETER_IN_LR * pi / TICKS_PER_ROTATION);
const double SPIN_TO_IN_B = (WHEEL_DIAMETER_IN_B * pi / TICKS_PER_ROTATION);
/*                                      */

class tracking
{
private:
  pros::ADIEncoder left;
  pros::ADIEncoder right;
  pros::ADIEncoder back;
  static bool task_started = false;

  int32_t lst_left;
  int32_t lst_right;
  int32_t lst_back;

  void track();

public:
  float gA;
  float gX;
  float gY;

  tracking(ADIEncoder& left, ADIEncoder& right, ADIEncoder& back);
  void resetPosition();
};
#endif
