#include "intake.hpp"

using namespace pros;

IntakeState intake_state = IntakeState::Off;

void intake_init() { }

void intake_set(int power) {
  intake.move(power);
}

void intake_handle() {
  if (ctrler.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
    if (intake_state != IntakeState::Forw) {
      intake_set(127);
      intake_state = IntakeState::Forw;
    }
    else {
      intake_set(0);
      intake_state = IntakeState::Off;
    }
  }

  if (ctrler.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
    if (intake_state != IntakeState::Back) {
      intake_set(-127);
      intake_state = IntakeState::Back;
    }
    else {
      intake_set(0);
      intake_state = IntakeState::Off;
    }
  }
}
