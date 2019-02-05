#include "intake.hpp"

using namespace pros;

IntakeState intake_state = IntakeState::Off;

void intake_init() {
  log_ln(LOG_ANGLER, "%d Finished intake init", pros::millis());
}

void intake_state_set(int power, IntakeState state) {
  intake_set(power);
  intake_state = state;
}

void intake_set(int power) {
  intake.move(power);
}

void intake_handle() {
  static uint32_t intake_jam_time;
  static uint32_t intake_on_time;

  if (check_single_press(BTN_INTAKE_UP)) {
    if (intake_state != IntakeState::Off) {
      intake_set(0);
      intake_state = IntakeState::Off;
    }
    else {
      intake_set(127);
      intake_state = IntakeState::Forw;
      intake_on_time = millis();
    }
  }

  else if (check_single_press(BTN_INTAKE_DOWN)) {
    if (intake_state != IntakeState::Off) {
      intake_set(0);
      intake_state = IntakeState::Off;
    }
    else {
      intake_set(-80);
      intake_state = IntakeState::Back;
    }
  }

  switch (intake_state) {
    case IntakeState::Forw:
      if (intake.get_actual_velocity() < 10 && millis() >= intake_on_time + 250) {
        intake_set(-80);
        intake_jam_time = millis();
        intake_state = IntakeState::Jam;
      }
      break;

    case IntakeState::Jam:
      if (millis() >= intake_jam_time + 250) {
        intake_set(127);
        intake_state = IntakeState::Forw;
        intake_on_time = millis();
      }
      break;

    default: break;
  }
}
