#pragma once
#include "main.h"
#include "util.hpp"
#include "logs.hpp"
#include <cmath>
#include <memory>

//namespace pilons::tracking {
  const double WHL_DIA_L = 2.783;
  const double WHL_DIA_R = 2.783;
  const double WHL_DIA_S = 0; //4;

  const double WHL_DIS_L = 7.4653975; //7.454625; //7.207894;
  const double WHL_DIS_R = 7.4653975; //7.454625; //7.207894;
  const double WHL_DIS_S = 0; //4.75;

  const double WHL_TPR_L = 360;
  const double WHL_TPR_R = 360;
  const double WHL_TPR_S = 360;

  const double SPN_TO_IN_L = (WHL_DIA_L * M_PI / WHL_TPR_L);
  const double SPN_TO_IN_R = (WHL_DIA_R * M_PI / WHL_TPR_R);
  const double SPN_TO_IN_S = (WHL_DIA_S * M_PI / WHL_TPR_S);

  const uint32_t VEL_TIME = 40;

  constexpr double RAD_TO_DEG(double rad) { return rad * 180 / M_PI; }
  constexpr double DEG_TO_RAD(double deg) { return deg *  M_PI / 180; }

  double operator "" _in(long double val);
  double operator "" _cm(long double val);
  double operator "" _in(unsigned long long val);
  double operator "" _cm(unsigned long long val);

  double operator "" _rad(long double val);
  double operator "" _deg(long double val);
  double operator "" _rad(unsigned long long val);
  double operator "" _deg(unsigned long long val);

  class Tracking {
  private:
    pros::ADIEncoder &encL, &encR, &encS;
    int encLLst, encRLst, encSLst;
    double aRst;

    double xLst, yLst, aLst;
    uint32_t velLstTime;
/*
  protected:
    void taskImpl() override;
*/
  public:
    double x, y, a = 0;
    double xVel = 0, yVel = 0, aVel = 0;
    vector velLocal;

    Tracking(pros::ADIEncoder &encL, pros::ADIEncoder &encR, pros::ADIEncoder &encS, double x = 0, double y = 0, double a = 0);

    void task();

    void update();
    void reset(double x = 0, double y = 0, double a = 0);
    vector position();
    vector velocity();
  };
//}
