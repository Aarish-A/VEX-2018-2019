#include "main.h"
#include "auto.hpp"
#include "config.hpp"

using namespace pros;
using namespace pilons::tracking;

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
void autonomous() {
	MotionController mc;
  pos.reset(0, 0, 0);
	mc.setStart({0, 0});
	mc.setEnd({0, 120});
	mc.setAngleTarget(new FixedAngleTarget(0_deg));
	//(new FixedAngleTarget(-90_deg));
	uint32_t tStart = millis();
	printf("\n\n\t%d Start Move", millis());
	mc.startTask();
	while (mc.dDistance() > 20) pros::delay(10);
	printf("\n\n\t%d Start Angle", millis());
	// mc.setAngleTarget(new PointAngleTarget({-48, 66}));

	while (fabs(mc.dAngle()) > 1_deg ) pros::delay(10);
	printf("\n\n\t%d DONE MOVE + ANGLE in %d", millis(), (millis()-tStart));
	mc.setStart({0, 120});
	mc.setEnd({0, 0});
	mc.setAngleTarget(new FixedAngleTarget(0_deg));

	while (true) pros::delay(10);
}
