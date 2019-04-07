#pragma once
#include "main.h"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/angler.hpp"
#include "subsystems/puncher.hpp"
#include "subsystems/capper.hpp"
#include "libraries/controller.hpp"
#include "libraries/tracking.hpp"

extern pilons::Controller master;
extern pilons::Controller partner;

extern pros::ADIEncoder enc_s;
extern pros::Motor m_intake;
extern pros::Motor m_drive_fl;
extern pros::Motor m_drive_fr;
extern pros::Motor m_drive_bl;
extern pros::Motor m_drive_br;
extern pros::Motor m_angler;
extern pros::ADIAnalogIn s_pole_poti;
extern pros::Motor m_puncher;
extern pros::ADILineSensor s_ball_detector;
extern pros::Motor m_capper;
extern pros::ADILineSensor right_platform_sensor;
extern pros::ADIGyro gyro;


extern Intake intake;
extern Drive drive;
extern Angler angler;
extern Puncher puncher;
extern Capper capper;

extern Tracking pos;
