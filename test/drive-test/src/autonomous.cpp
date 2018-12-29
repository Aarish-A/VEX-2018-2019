#include "main.h"
#include "tracking.hpp"
#include "motion.hpp"
#include "config.hpp"
#include "slew.hpp"

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
enum eAuto
{
	autoFourFlags,
	autoAccelTest,
	autoTrackingTest,
	autoTrackingXTest
};

eAuto gAuto = autoFourFlags;

void accelTest()
{
	for (double y = 0; y < 150; y++)
	{
		setDriveY(-y);
		pros::delay(8);
		printf("%d (%lf, %lf, %lf) \n", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
	}
	std::cout<<"\n"<<millis()<<"Start Decel";
	for (double y = 150; y > 0; y--)
	{
		setDriveY(-y);
		pros::delay(8);
		printf("%d (%lf, %lf, %lf) \n", millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
	}
	setDriveY(0);
	std::cout<<"\n"<<millis()<<"Done Move";
}

void trackingTest()
{
	pos.reset(0,0,0);
	while(true)
	{
		printf("%d Enc(%d, %d, %d) Pos(%lf, %lf, %lf) Vel(%lf, %lf) VelLoc(%lf, %lf) \n", millis(), encL.get_value(), encR.get_value(), encS.get_value(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.xVel, pos.yVel, pos.velLocal.x, pos.velLocal.y);
		delay(100);
	}
}

void trackingXTest()
{
	pos.reset(0,0,0);
	setDriveY(100);
	while (pos.y < 100)
	{
		printf("%d %lf %lf %lf", millis(), pos.x, pos.y, pos.a);
		delay(10);
	}
	setDriveY(0);
	while (true)
	{
		printf("%d %lf %lf %lf", millis(), pos.x, pos.y, pos.a);
		delay(10);
	}
}

void autonomous() {
	switch(gAuto)
	{
		case autoFourFlags:
		{
			printf("%d Start Auto \n", millis());
			pos.reset(60, 0, 0);
			moveToTarget({pos.x, 1.75*24}, false);
			turn(new PointAngleTarget({10, 65}));
			/*MotionController mc;
			//int startY = WHL_DIA_S + 4;
		  pos.reset(60, 0, 0);
			mc.setFull(pos.position(), {pos.x, 24});
			mc.startTask();
			while (mc.dDistance() > 1) delay(10);
			setDrive(0, 0, 0);
			printf("%d Done \n", millis());
			while(true) delay(10);
			break;
			*/
		//	turn(new FixedAngleTarget(90_deg));//PointAngleTarget({10, 10}));
		//	moveToTarget({pos.x+5, 48}, true);

		/*
			setDrive(0, 0, 15);
			pros::delay(2000);
			printf("%d Done Turn %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			setDrive(0, 0, 0);
			pros::delay(1000);
			printf("%d Done Turn Sleep %f %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel);
			*/
			break;
		}

		case autoAccelTest:
		{
			accelTest();
			break;
		}

		case autoTrackingTest:
		{
			trackingTest();
			break;
		}

		case autoTrackingXTest:
		{
			trackingXTest();
			break;
		}
	}
	/*
	MotionController mc;
	int startY = WHL_DIA_S + 4;
  pos.reset(60, 0, 0);
	//mc.setStart({60, 0});
	mc.setStartToCurrent();
	mc.setEnd({pos.x, -48});
	mc.setAngleTarget(new FixedAngleTarget(0_deg));
	//(new FixedAngleTarget(-90_deg));
	uint32_t tStart = millis();
	printf("\n\n\t%d Start Move", millis());
	mc.startTask();
	while (mc.dDistance() > 1) pros::delay(10);
	mc.stopTask();
	setDrive(0, 0, 0);
	*/


	/*
	while (true)
	{
		printf("%d pos:%d", millis(), encS.get_value());
		pros::delay(10);
	}
	*/

	/*
	printf("\n\n\t%d Start Angle", millis());
	// mc.setAngleTarget(new PointAngleTarget({-48, 66}));

	while (fabs(mc.dAngle()) > 1_deg ) pros::delay(10);
	printf("\n\n\t%d DONE MOVE + ANGLE in %d", millis(), (millis()-tStart));
	mc.setStart({0, 120});
	mc.setEnd({0, 0});
	mc.setAngleTarget(new FixedAngleTarget(0_deg));

	while (mc.dDistance() > 1.0) pros::delay(10);
	*/
}
