#include "slew.hpp"
#include <cmath>

Slew::Slew(double upLim, double downLim, double value) : upLim(fabs(upLim)), downLim(fabs(downLim)), value(value) {}

double Slew::slewSet(double newVal)
{
  value = value > 0 ? fmin(fmax(newVal, value - downLim), value + upLim) : fmin(fmax(newVal, value - upLim), value + downLim);
  return value;
}

void Slew::setSlewVal(double value)
{
  this->value = value;
}
