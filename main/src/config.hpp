#pragma once
#include "main.h"
#include "libraries/subsystem.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/drive.hpp"

extern pros::Motor m_intake;
extern pros::Motor m_drive_fl;
extern pros::Motor m_drive_fr;
extern pros::Motor m_drive_bl;
extern pros::Motor m_drive_br;

extern Intake intake;
extern Drive drive;

extern Subsystem* subsystems[3];
