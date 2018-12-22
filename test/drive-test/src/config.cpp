#include "config.hpp"

pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::Motor driveFL(1, true);
pros::Motor driveBL(11, true);
pros::Motor driveFR(10);
pros::Motor driveBR(20);
pros::ADIEncoder encL(1, 2);
pros::ADIEncoder encR(5, 6, true);
pros::ADIEncoder encS(3, 4);
pilons::tracking::Tracking pos(encL, encR, encS);
