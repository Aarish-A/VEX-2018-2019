#include "main.h"
#include "config.hpp"
#include "auto.hpp"
#include "button.hpp"

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

uint32_t autoStartTime;

void auto_red_front();
void auto_red_back();
void auto_red_back_no_second_shot();
void auto_blue_front();
void auto_blue_back();
void auto_blue_back_no_second_shot();
void programming_skills();

void autonomous() {
  uint32_t autonomous_time = pros::millis();
  start_auto_update_task();





  stop_auto_update_task();
  ctrler.print(2, 0, "Time: %d", pros::millis() - autonomous_time);
}


void auto_red_front() {

}

void auto_red_back() {

}

void auto_red_back_no_second_shot() {

}

void auto_blue_front() {

}

void auto_blue_back() {

}

void auto_blue_back_no_second_shot() {

}

void programming_skills() {

}
