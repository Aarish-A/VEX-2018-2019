#include "tracking.hpp"

double operator "" _in(long double val) {
  return val;
}

double operator "" _cm(long double val) {
  return val * 2.54;
}

double operator "" _in(unsigned long long val) {
  return val;
}

double operator "" _cm(unsigned long long val) {
  return val * 2.54;
}

double operator "" _rad(long double val) {
  return val;
}

double operator "" _deg(long double val) {
  return  DEG_TO_RAD(val);
}

double operator "" _rad(unsigned long long val) {
  return val;
}

double operator "" _deg(unsigned long long val) {
  return  DEG_TO_RAD(val);
}

Tracking::Tracking(pros::ADIEncoder &encL, pros::ADIEncoder &encR, pros::ADIEncoder &encS, double x, double y, double a) : encL(encL), encR(encR), encS(encS) {
  this->x = this->xLst = x;
  this->y = this->yLst = y;
  this->aRst = this->aLst = a;

  encL.reset();
  encR.reset();
  encS.reset();

  this->encLLst = 0;
  this->encRLst = 0;
  this->encSLst = 0;

  this->xVel = 0;
  this->yVel = 0;
  this->aVel = 0;

  this->velLstTime = pros::millis();
}

void Tracking::task() {
  {}//log_ln(LOG_AUTO, "%d Start Update Task", pros::millis());
  while (true) {
    update();
    pros::delay(1);
  }
}

void Tracking::update() {

  int left = encL.get_value();
  int right = encR.get_value();
  int strafe = 0;//encS.get_value();

  //{}//log_ln("%d pos(%f, %f, %f) (%d, %d)", pros::millis(), x, y, RAD_TO_DEG(a), left, right);


  double L = (left - encLLst) * SPN_TO_IN_L; // The amount the left side of the robot moved
  double R = (right - encRLst) * SPN_TO_IN_R; // The amount the right side of the robot moved
  double S = (strafe - encSLst) * SPN_TO_IN_S; // The amount the back side of the robot moved

  encLLst = left;
  encRLst = right;
  encSLst = strafe;

  double h; // The hypotenuse of the triangle formed by the middle of the robot on the starting position and ending position and the middle of the circle it travels around
	double i; // Half on the angle  traveled
	double h2; // The same as h but using the back instead of the side wheels
  double ga = this->a;
  double a = aRst + ((left * SPN_TO_IN_L) - (right * SPN_TO_IN_R)) / (WHL_DIS_L + WHL_DIS_R); // The angle traveled
  double ar = a - ga;
  if (ar) {
		double r = R / ar; // The radius of the circle the robot travel's around with the right side of the robot
		i = ar / 2.0;
		double sinI = sin(i);
		h = ((r + WHL_DIS_R) * sinI) * 2.0;

		double r2 = S / ar; // The radius of the circle the robot travel's around with the back of the robot
		h2 = ((r2 + WHL_DIS_S) * sinI) * 2.0;
	}
	else {
		h = R;
		i = 0;

		h2 = S;
	}

  double p = i + ga;
  double cosP = cos(p);
  double sinP = sin(p);

  // Update the global position
  this->x += h * sinP + h2 * cosP;
  this->y += h * cosP - h2 * sinP;
  this->a = a;

  //Calculate velocity
  uint32_t curTime = pros::millis();
  if (curTime > velLstTime + VEL_TIME) {
    uint32_t dt = curTime - velLstTime;
    velLstTime = curTime;

    xVel = 1000 * (this->x - xLst) / dt;
    yVel = 1000 * (this->y - yLst) / dt;
    aVel = 1000 * (this->a - aLst) / dt;

    velLocal = rotate({xVel, yVel}, -(this->a));

    xLst = this->x;
    yLst = this->y;
    aLst = this->a;
  }
}

void Tracking::reset(double x, double y, double a) {
  {}//log_ln(LOG_AUTO, "\n%d Resetting Pos (%f, %f, %f) from (%f, %f, %f)", pros::millis(), x, y, RAD_TO_DEG(a), this->x, this->y, RAD_TO_DEG(this->a));
  this->encL.reset();
  this->encR.reset();
  this->encS.reset();
  this->encLLst = 0;
  this->encRLst = 0;
  this->encSLst = 0;

  this->x = x;
  this->y = y;
  this->a = 0;
  this->aRst = a;
}

vector Tracking::position() const{
  return {x, y};
}

vector Tracking::velocity() const{
  return {xVel, yVel};
}
