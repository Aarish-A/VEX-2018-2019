#include "config.hpp"

pros::Controller ctrler(pros::E_CONTROLLER_MASTER);
pros::Motor drive_fl(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor drive_bl(4, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor drive_fr(10, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor drive_br(8, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor angler(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor intake(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor puncher(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADIEncoder enc_l(1, 2, true);
pros::ADIEncoder enc_r(3, 4, true);
pros::ADIEncoder enc_s(5, 6, true);
pros::ADILineSensor ball_sensor(5);

Tracking pos(enc_l, enc_r, enc_s);
