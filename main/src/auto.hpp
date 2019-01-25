#pragma once
#include "config.hpp"
#include "tracking.hpp"
#include "angle_target.hpp"
#include "util.hpp"
#include "logs.hpp"
#include "shot_select.hpp"
#include <math.h>

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


double getGlobalAngle();
void resetGlobalAngle();
void setDrive(int x, int y, int a);
void setDrive(int pow);
void setDriveTurn(int left, int right);
void setDriveVel(int x, int y, int a);
void setDriveVel(int vel);

void drive_brake();

void move_drive(double dis, int vel = 200, bool stop = true);
void move_drive_simple(double dis, int vel, bool stop = true);
//void turnDrive(double targ, int vel);
void turn_vel(AngleTarget *target, double kP, double offset = 0, float drive_turn_handled_time = 0, short req_handled_num = 0);
void turn_vel_side(AngleTarget *target, double kP, double offset, bool f_w);

void flatten_against_wall(bool f_w, bool hold);
void flatten_angle(bool left, bool right, bool hold);

double operator "" _tk(long double val);
