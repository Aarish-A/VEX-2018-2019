#include "config.hpp"

pros::Motor m_intake(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

Intake intake("Intake", m_intake);
