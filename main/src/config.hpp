#pragma once
#include "main.h"
#include "tracking.hpp"
#include "config.hpp"

extern pros::Controller ctrler;
extern pros::Motor drive_fl;
extern pros::Motor drive_bl;
extern pros::Motor drive_fr;
extern pros::Motor drive_br;
extern pros::Motor angler;
extern pros::Motor intake;
extern pros::Motor puncherLeft;
extern pros::Motor puncherRight;
extern pros::ADIEncoder enc_l;
extern pros::ADIEncoder enc_r;
extern pros::ADIEncoder enc_s;
extern pros::ADILineSensor ball_sensor;
extern pros::Vision vision_sensor;

extern Tracking pos;
