#include "util.hpp"

namespace pilons::util {
  void BackgroundTask::startTask() {
    if (!this->task) {
      this->task.reset(new pros::Task([] (void *ptr) {
        static_cast<BackgroundTask *>(ptr)->taskImpl();
      }, this));
    }
  }

  void BackgroundTask::stopTask() {
    printf("  %d Enter Background Task Destructor \n", pros::millis());
    if (this->task) {
      printf("  %d Destruct Background Task \n", pros::millis());
      this->task->remove();
      this->task = nullptr;
    }
  }

  BackgroundTask::~BackgroundTask() {
    stopTask();
  }
}
