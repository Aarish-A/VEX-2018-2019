#include "config.hpp"

pros::Motor m_intake(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_fl(7 , pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_fr(10, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_bl(4, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_drive_br(8, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor m_angler(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADIAnalogIn s_pole_poti(3);
pros::Motor m_puncher(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADILineSensor s_ball_detector(5);
pros::ADIEncoder enc_r(7, 8, false);
pros::ADIEncoder enc_l(1, 2, false);
pros::ADIEncoder enc_s(5, 6, true);

Intake intake("Intake", Intake::STATE_OFF, m_intake);
Drive drive("Drive", Drive::STATE_AUTO_CONTROL, m_drive_fl, m_drive_fr, m_drive_bl, m_drive_br, s_pole_poti);
Angler angler("Angler", Angler::STATE_AUTO_CONTROL, m_angler);
Puncher puncher("Puncher", Puncher::STATE_DISABLED, m_angler, s_ball_detector);
