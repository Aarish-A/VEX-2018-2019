#include "main.h"
#include "libraries/logs.hpp"
#include "config.hpp"
#include "controls.hpp"
#include "macros/shot_queueing.hpp"
#include "menu.hpp"
#include "libraries/util.hpp"
#include "libraries/task.hpp"

void capper_move_to_cap_flip_macro(void* _params) {
	capper.move_to_velocity(39 * Capper::GEAR_RATIO, 200);
	pros::delay(200);
	intake.intake();
}


pilons::Task capper_move_to_cap_flip_task("Capper Cap Flip", [](void* param) {
	capper.move_to_velocity(37 * Capper::GEAR_RATIO, 200);
	pros::delay(200);
	intake.intake();
	angler.move_to(Angler::PICKUP_POSITION);
});

double get_shot_angle(long double _x, long double _y) {
	// constexpr long double ANGLER_MIN_ANGLE = 24.2;
	constexpr long double ANGLER_MIN_ANGLE = 13.95;
	constexpr long double ANGLER_MAX_ANGLE = 120.0;
	constexpr long double ANGLER_HEIGHT = 11.6_in;
	constexpr long double ANGLER_DISTANCE_FROM_FRONT = 9.5_in;
	constexpr long double DX = 3.25_in;
	constexpr long double M_BOLT = 68.8;
	constexpr long double M_BALL = 56.0;
	constexpr long double G = 9.80665;

	long double y = _y - ANGLER_HEIGHT;
	long double x = _x + ANGLER_DISTANCE_FROM_FRONT;
	//
	// long double k = 100.0; // N / in
	// long double v_bolt = sqrt(k * DX * DX / M_BOLT);
	// long double v_ball = (2 * M_BOLT) / (M_BOLT + M_BALL) * v_bolt;
	// long double v_ball = 31.367;
	long double v_ball = 46.45;

	long double t1 = pow(v_ball, 2.0);

	long double discriminant = pow(v_ball, 4.0) - G * (G * x * x + 2 * y * v_ball * v_ball);
	long double t2 = 0;
	if (discriminant >= 0) t2 = sqrt(discriminant);
	else return 0;
	long double t3 = G * x;

	long double angle_1 = (atan((t1 + t2) / t3) * 180 / M_PI) - ANGLER_MIN_ANGLE;
	long double angle_2 = (atan((t1 - t2) / t3) * 180 / M_PI) - ANGLER_MIN_ANGLE;

	bool angle_1_valid = false;
	bool angle_2_valid = false;

	if (angle_1 >= 0 && angle_1 <= ANGLER_MAX_ANGLE) angle_1_valid = true;
	if (angle_2 >= 0 && angle_2 <= ANGLER_MAX_ANGLE) angle_2_valid = true;

	if (angle_1_valid && !angle_2_valid) return angle_1 * 7.0;
	else if (!angle_1_valid && angle_2_valid) return angle_2 * 7.0;
	else if (!angle_1_valid && !angle_2_valid) return 0.0;
	else return fminl(angle_1 * 7.0, angle_2 * 7.0);
}


void opcontrol() {
	log_ln(PROGRAM_FLOW, "   --- %d START OPCONTROL --- \n", pros::millis());
	if (auto_routine == Auto_Routines::DRIVER_SKILLS) autonomous();
	pilons::Task::stop_all_tasks();
	Subsystem::enable_all();
	enc_r.reset();
	enc_l.reset();
	pos.reset();
	while (true) {
		log_ln(MOVE, "Start f/w");
		drive_move_async(7_in);
		drive.wait_for_stop();
		pros::delay(500);
		log_ln(MOVE, "Start b/w");
		drive_move_async(-7_in);
		drive.wait_for_stop();
		pros::delay(500);
	}
}
