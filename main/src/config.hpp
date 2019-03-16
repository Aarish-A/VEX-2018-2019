#pragma once
#include "main.h"

extern pros::Controller ctrler;
extern pros::Controller partner;
extern pros::Motor drive_fl;
extern pros::Motor drive_bl;
extern pros::Motor drive_fr;
extern pros::Motor drive_br;
extern pros::Motor angler;
extern pros::Motor intake;
extern pros::Motor decapper;
extern pros::Motor puncherLeft;
extern pros::Motor puncherRight;
extern pros::ADIEncoder enc_l;
extern pros::ADIEncoder enc_r;
extern pros::ADIEncoder enc_s;
extern pros::ADIAnalogIn autoPoti;
extern pros::ADILineSensor ball_sensor;
extern pros::ADIGyro gyro;
//extern pros::ADILineSensor left_platform_sensor;
extern pros::ADILineSensor right_platform_sensor;
//extern pros::Vision vision_sensor;
extern pros::ADIAnalogIn capper_poti;

extern bool is_disabled;
