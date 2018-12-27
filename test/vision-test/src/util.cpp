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
    if (this->task) {
      this->task->remove();
      this->task = nullptr;
    }
  }
}
