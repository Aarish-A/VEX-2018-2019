#pragma once
#include "main.h"
#include "logs.hpp"

namespace pilons {
  class Task {
  private:
    static pilons::Task* tasks[32];
    static uint8_t number_of_tasks;

    std::string task_name;
    pros::Task* task = nullptr;
    pros::task_fn_t task_function = nullptr;
    void (*task_stop_function)() = nullptr;

  public:
    Task(std::string task_name, pros::task_fn_t task_function, void(*function)() = nullptr);

    void start_task(void* params = nullptr);
    void stop_task();
    bool running();

    static void stop_all_tasks();
  };
}
