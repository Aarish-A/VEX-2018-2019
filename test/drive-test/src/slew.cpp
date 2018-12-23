#include "slew.hpp"
#include <cmath>

Slew::Slew(double dLim, double valLst, double valCur) :
  dLim_(fabs(dLim)), valLst_(valLst), valCur_(valCur) {}

double Slew::slewSet(double valCur)
{
  double upLim = valLst_ + dLim_;
  double lowLim = valLst_ - dLim_;

  if (valCur > upLim) valCur_ = upLim;
  else if (valCur < lowLim) valCur_ = lowLim;
  else valCur_ = valCur;

  valLst_ = valCur_;

  return valCur_;
}
