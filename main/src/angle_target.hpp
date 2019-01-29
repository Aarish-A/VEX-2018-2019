#pragma once
#include "main.h"
#include <memory>
#include "tracking.hpp"
#include "util.hpp"
#include "logs.hpp"
#include "config.hpp"
#include <cmath>

  class AngleTarget {
  public:
    virtual double getTarget() const = 0;
  };

  class FixedAngleTarget final : public AngleTarget {
    double target;

  public:
    FixedAngleTarget(double target);

    double getTarget() const override;
  };

  class PointAngleTarget final : public AngleTarget {
    vector target;

  public:
    PointAngleTarget(vector target);
    double getTarget() const override;
  };
