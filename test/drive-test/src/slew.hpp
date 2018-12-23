#ifndef SLEW_H
#define SLEW_H
class Slew {
private:
    double dLim_, valLst_, valCur_;

public:
  Slew(double dLim, double valLst, double valCur);

  double slewSet(double valCur);

};
#endif
