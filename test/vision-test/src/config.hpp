#include "main.h"
#include "tracking.hpp"
#include "auto.hpp"

extern pros::Controller controller;
extern pros::Motor driveFL;
extern pros::Motor driveBL;
extern pros::Motor driveFR;
extern pros::Motor driveBR;
extern pros::ADIEncoder encL;
extern pros::ADIEncoder encR;
extern pros::ADIEncoder encS;
extern pros::Vision visionSensor;
extern pilons::tracking::Tracking pos;
