#ifndef SLEW_H
#define SLEW_H

class Slew {
private:
    double upLim, downLim, value;

public:
  Slew(double upLim, double downLim, double value);

  double slewSet(double newVal);
  void setSlewVal(double value);
};

#endif
