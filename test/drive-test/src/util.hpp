#ifndef PILONS_UTIL_H
#define PILONS_UTIL_H

#include "main.h"
#include <memory>

#define SGN(x) ((x > 0) - (x < 0))

namespace pilons::util {
  class BackgroundTask {
    std::unique_ptr<pros::Task> task;

  protected:
    virtual void taskImpl() = 0;

  public:
    void startTask();
    void stopTask();

    ~BackgroundTask();
  };
}

#endif
