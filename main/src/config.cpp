#include "config.hpp"

pilons::Controller master(pros::E_CONTROLLER_MASTER, "Master");
pilons::Controller partner(pros::E_CONTROLLER_PARTNER, "Partner");

pros::ADIEncoder enc_s(5, 6, false);
pros::Motor m_intake(6, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_fl(7 , pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_fr(10, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_bl(4, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_br(8, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_angler(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADIAnalogIn s_pole_poti(3);
pros::Motor m_puncher(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADILineSensor s_ball_detector(5);
pros::Motor m_capper(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

Intake intake("Intake", Intake::STATE_OFF, m_intake);
Drive drive("Drive", Drive::STATE_DRIVER_CONTROL, m_drive_fl, m_drive_fr, m_drive_bl, m_drive_br, s_pole_poti, enc_l, enc_r);
Angler angler("Angler", Angler::STATE_DRIVER_CONTROL, m_angler);
Puncher puncher("Puncher", Puncher::STATE_LOADED, m_puncher, s_ball_detector);
Capper capper("Capper", Capper::STATE_HOLD, m_capper);
