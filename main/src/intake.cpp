#include "intake.hpp"

using namespace pros;

IntakeState intake_state = IntakeState::Off;

void intake_init() { }

void intake_set(int power) {
  intake.move(power);
}

void intake_handle() {
/*  if (angler.get_actual_velocity() < 1 && intake_state == IntakeState::Forw) {
      intake_set(-127);
  }

  else*/
   if (ctrler.get_digital_new_press(BTN_INTAKE_UP)) {
    if (intake_state != IntakeState::Off) {
      intake_set(0);
      intake_state = IntakeState::Off;
    }
    else {
      intake_set(127);
      intake_state = IntakeState::Forw;
    }
  }

  else if (ctrler.get_digital_new_press(BTN_INTAKE_DOWN)) {
    if (intake_state != IntakeState::Off) {
      intake_set(0);
      intake_state = IntakeState::Off;
    }
    else {
      intake_set(-80);
      intake_state = IntakeState::Back;
    }
  }
}
