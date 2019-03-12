#pragma once
#include "config.hpp"
#include "tracking.hpp"
#include "angle_target.hpp"
#include "util.hpp"
#include "logs.hpp"
#include "puncher.hpp"
#include "angler.hpp"
#include "decapper.hpp"
#include <math.h>

/* Auto Task Handler */
extern pros::Task* auto_update_task;
void auto_update(void* param);
void auto_update_stop_task();
void auto_update_start_task() ;

/* Auto Puncher and Angler Setter */
void auto_set_angler_target(double target);
void auto_set_first_shot(double target);
void auto_set_second_shot(double target);

extern bool blue_team;

constexpr double DRIVE_DIA = 3.95;
constexpr double DRIVE_WIDTH = 13.0;
constexpr double DRIVE_LENGTH = 9.5;
constexpr double DRIVE_GEAR =  60.0 / 84.0;
constexpr double DRIVE_TPR = 360 * DRIVE_GEAR;
constexpr double MOVE_ACCEL_RATE = 5;
constexpr double MOVE_DECEL_RATE = 16;
constexpr double TURN_ACCEL_RATE = 3;
constexpr double TURN_DECEL_RATE = 10;
constexpr double IN_TO_TK(long double val) { return (val * DRIVE_TPR) / (DRIVE_DIA * M_PI); }
constexpr double TK_TO_IN(long double val) { return (val * (DRIVE_DIA * M_PI)) / DRIVE_TPR; }
constexpr double CM_TO_IN(long double val) { return val  * 2.54; }
constexpr double DEG_TO_RAD(long double val) { return val * M_PI / 180.0; }
constexpr double RAD_TO_DEG(long double val) { return val * 180.0 / M_PI; }

constexpr double MECANUM_DRIVE_WIDTH = 13.20;
constexpr double EDGE_TO_TRACKING_WHEEL = 1.25;
constexpr double DRIVE_EDGE_TO_MECANUM = 2.50;


double getGlobalAngle();
void resetGlobalAngle();
void setDrive(int x, int y, int a);
void setDrive(int pow);
void setDriveTurn(int left, int right);
void setDriveVel(int x, int y, int a);
void setDriveVel(int vel);

void drive_brake();
void move_drive_new(double distance, int max_power = 200, bool stop = true, double correct_angle = 1000);
void move_drive(vector targ, int vel = 200, bool stop = true);

void move_drive_rel(double targ, int vel = 200, bool brake = true);
void move_drive_rel_simple(double dis, int vel, bool stop = true);

//void turnDrive(double targ, int vel);
void turn_vel(const AngleTarget& target, double kP, double offset = 0, float drive_turn_handled_time = 0, short req_handled_num = 0,  double max_vel = 200);
void sweep_turn_new(const AngleTarget& target, float radius, bool fw, double postdis = 8_in, bool cw = true, bool brake = true, int max_power = 200);
void turn_vel_fast(const AngleTarget& target, double kP, double offset = 0, double correct_amount = 0, bool correct_left = 0);
void turn_vel_auto(const AngleTarget& target, double kP, double offset = 0, float drive_turn_handled_time = 0, double angler_target = 0);
void turn_vel_side(const AngleTarget& target, double kP, double offset, bool f_w);
void turn_vel_side_simple(const AngleTarget& target, double kP, double offset, bool f_w);
void turn_vel_new(const AngleTarget& target);

void flatten_against_wall(bool f_w, bool hold, int hold_power = 15);
void flatten_angle(bool left, bool right, bool hold);

double operator "" _tk(long double val);
