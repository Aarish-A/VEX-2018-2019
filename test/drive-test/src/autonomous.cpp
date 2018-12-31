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

	autoTest,
	autoTurnSpeedTest,
	autoAccelTest,
	autoTrackingTest,
	autoTrackingXTest
};

eAuto gAuto = autoTest;//autoFourFlags;//autoAccelTest;;

void accelTest()
{
	for (double y = 0; y < 150; y+=1)
	{
		setDriveY(y);
		printf("%d %lf: %lf, %lf, %lf, %lf  \n", millis(), y, driveFL.get_actual_velocity(), driveFR.get_actual_velocity(), driveBL.get_actual_velocity(), driveBR.get_actual_velocity());
		pros::delay(8);
		//printf("%d %lf: (%lf, %lf, %lf) \n", millis(), y, pos.x, pos.y, RAD_TO_DEG(pos.a));
	}
	std::cout<<"\n"<<millis()<<"Start Decel";
	for (double y = 150; y > 0; y-=1)
	{
		setDriveY(y);
		printf("%d %lf: %lf, %lf, %lf, %lf  \n", millis(), y, driveFL.get_actual_velocity(), driveFR.get_actual_velocity(), driveBL.get_actual_velocity(), driveBR.get_actual_velocity());
		pros::delay(8);
	//	printf("%d %lf: (%lf, %lf, %lf) \n", millis(), y, pos.x, pos.y, RAD_TO_DEG(pos.a));
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

	printf("%d Start Auto \n", millis());
	switch(gAuto)
	{
		case autoFourFlags:
		{
			pos.reset(108, 0, 0);
			//1 Pick up
			printf("%d 1 PU\n", millis());
			moveToTarget({pos.x, 1.75*24}, false);

			//2 Back up
			printf("%d 2 BU\n", millis());
			turn(new PointAngleTarget({5.3*24, 36}), -180_deg, true, false);
			moveToTarget({5.3*24, 36}, false);

			//3 Turn and shoot
			printf("%d 3 Turn Shoot\n", millis());
			turn(new PointAngleTarget({10, 65}), 0, false, true);
			pros::delay(1200);

			//4 pick up balls off of cap
			printf("%d 4 Cap Balls\n", millis());
			turn(new PointAngleTarget({5*24, 2.5*24}), 0, true, false);
			moveToTarget({5*24, 2.5*24});

			//5 Back up turn and shoot
			printf("%d 5 Trn Shoot\n", millis());
			moveToTarget({pos.x, pos.y-10});
			turn(new PointAngleTarget({10, 65+49}), 0, false, true);
			pros::delay(1200);


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
		case autoTest:
		{
			pos.reset(0, 0, -90_deg);
			printf("%d Reset", millis());
			pros::delay(50);
			turn(new PointAngleTarget({pos.x, pos.y+15}), 0, true, false);
			moveToTarget({pos.x, pos.y+15});
/*
			pos.reset(0, 0, 0);
			turn(new FixedAngleTarget(-180_deg));//PointAngleTarget({127, 36}), -180_deg);
			pros::delay(1000);
			printf("%d Done turn Sleep %f %f %f %f\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.yVel);
*/
			/*
			turn(new PointAngleTarget({5.3*24, 36}), -180_deg);
			moveToTarget({5.3*24, 36}, false);
			pros::delay(1000);
			printf("After sleep out %d %lf %lf %lf \n", millis(), pos.x, pos.y, pos.a);
			*/

			/*
			setDriveY(1200);
			pros::delay(5000);
			setDriveY(0);
			printf("%d Done move %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			double yOld = pos.y;
			pros::delay(1000);
			printf("%d Done move Sleep %f %f %f %f : %f\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.aVel, pos.y-yOld);
			*/


/*
			moveToTarget({pos.x, -20}, false);
			pros::delay(1000);
			printf("%d Done move Sleep %f %f %f %f\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.yVel);
			moveToTarget({pos.x, 10}, false);
			pros::delay(1000);
			printf("%d Done move Sleep %f %f %f %f\n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), pos.yVel);
*/
			break;
		}
		case autoTurnSpeedTest:
		{
			pos.reset(0, 0, 0);

			setDriveVoltage(0, 0, 30);
			pros::delay(1000);
			printf("%d Done Turn %f %f %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a));
			double oldA = pos.a;
			setDriveVoltage(0, 0, 0);
			driveFL.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
			driveFR.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
			driveBL.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
			driveBR.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
			//setDrive(0, 0, 0);
			pros::delay(1000);
			printf("%d Done Turn Sleep %f %f %f | %f \n", pros::millis(), pos.x, pos.y, RAD_TO_DEG(pos.a), RAD_TO_DEG(pos.a-oldA));

			break;
		}
		case autoAccelTest:
		{
			printf("%d Start \n", millis());
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
}
