#pragma once
#include "main.h"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/angler.hpp"
#include "subsystems/puncher.hpp"
#include "tracking.hpp"

extern pros::Motor m_intake;
extern pros::Motor m_drive_fl;
extern pros::Motor m_drive_fr;
extern pros::Motor m_drive_bl;
extern pros::Motor m_drive_br;
extern pros::Motor m_angler;
extern pros::ADIAnalogIn s_pole_poti;
extern pros::Motor m_puncher;
extern pros::ADILineSensor s_ball_detector;
extern pros::ADIEncoder enc_l;
extern pros::ADIEncoder enc_r;
extern pros::ADIEncoder enc_s;

extern Intake intake;
extern Drive drive;
extern Angler angler;
extern Puncher puncher;
extern Tracking pos;
