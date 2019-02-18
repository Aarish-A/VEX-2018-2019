#include "intake.hpp"

using namespace pros;

IntakeState intake_state = IntakeState::Off;

void intake_init() { }

void intake_power_state_set(int power, IntakeState state) {
  intake_set(power);
  intake_state = state;
}

void intake_state_set(IntakeState state) {
  log_ln(LOG_INTAKE, "%d Changing intake state to %d", millis(), intake_state);
  switch(state)
  {
    case IntakeState::Off: intake_power_state_set(0, IntakeState::Off); break;
    case IntakeState::Forw: intake_power_state_set(127, IntakeState::Forw); break;
    case IntakeState::Back: intake_power_state_set(-80, IntakeState::Back); break;
    case IntakeState::Jam: intake_power_state_set(-80, IntakeState::Jam); break;
  }
}

void intake_set(int power) {
  intake.move(power);
}

void intake_handle() {
  static uint32_t intake_jam_time;
  static uint32_t intake_on_time;

  if (check_single_press(BTN_INTAKE_UP)) {
    if (intake_state != IntakeState::Off) {
      intake_state_set(IntakeState::Off);
    }
    else {
      intake_state_set(IntakeState::Forw);
      intake_on_time = millis();
    }
  }

  else if (check_single_press(BTN_INTAKE_DOWN)) {
    if (intake_state != IntakeState::Off) {
      intake_state_set(IntakeState::Off);
    }
    else {
      intake_state_set(IntakeState::Back);
    }
  }

  switch (intake_state) {
    case IntakeState::Forw:
      if (intake.get_actual_velocity() < 10 && millis() >= intake_on_time + 250) {
        intake_jam_time = millis();
        intake_state_set(IntakeState::Jam);
        log_ln(LOG_INTAKE, "%d INTAKE STATE JAM ENTER", millis());
      }
      break;

    case IntakeState::Jam:
      if (millis() >= intake_jam_time + 250) {
        intake_state_set(IntakeState::Forw);
        intake_on_time = millis();
      }
      break;

    default: break;
  }
}
