#include <math.h>

const double WHL_DIA_L = 2.783;
const double WHL_DIA_R = 2.783;
const double WHL_DIS_L = 7.454625; //7.207894;
const double WHL_DIS_R = 7.454625; //7.207894;
const double WHL_TPR_L = 360;
const double WHL_TPR_R = 360;
const double SPN_TO_IN_L = (WHL_DIA_L * M_PI / WHL_TPR_L);
const double SPN_TO_IN_R = (WHL_DIA_R * M_PI / WHL_TPR_R);

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
constexpr double CM_TO_IN(long double val) { return val  * 2.54; }
constexpr double DEG_TO_RAD(long double val) { return val * M_PI / 180.0; }
constexpr double RAD_TO_DEG(long double val) { return val * 180.0 / M_PI; }

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

double getGlobalAngle();
void resetGlobalAngle();
void setDrive(int x, int y, int a);
void setDrive(int pow);
void setDriveVel(int x, int y, int a);
void setDriveVel(int vel);
void moveDrive(double dis, int vel, bool stop = true);
void turnDrive(double targ, int vel);

double operator "" _tk(long double val);
double operator "" _in(long double val);
double operator "" _cm(long double val);
double operator "" _deg(long double val);