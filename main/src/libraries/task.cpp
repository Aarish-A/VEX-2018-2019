#include "task.hpp"

pilons::Task* pilons::Task::tasks[32];
uint8_t pilons::Task::number_of_tasks = 0;

pilons::Task::Task(std::string task_name, pros::task_fn_t task_function, void(*function)()) : task_name(task_name), task_function(task_function) {
  this->task_stop_function = function;
  pilons::Task::tasks[pilons::Task::number_of_tasks] = this;
  pilons::Task::number_of_tasks++;
}

void pilons::Task::start_task(void* params) {
  if (this->task != nullptr) {
    if ((this->task)->get_state() != pros::E_TASK_STATE_DELETED) (this->task)->remove();
    delete this->task;
    this->task = nullptr;
    log_ln(PROGRAM_FLOW, "Stopped %s task from task start", (this->task_name).c_str());
  }

  log_ln(PROGRAM_FLOW, "Started %s task", (this->task_name).c_str());
  this->task = new pros::Task(this->task_function, params);
}

void pilons::Task::stop_task() {
  if (this->task != nullptr) {
    if (this->task_stop_function != nullptr) this->task_stop_function();
    if ((this->task)->get_state() != pros::E_TASK_STATE_DELETED) (this->task)->remove();
    // pros::Task([](void* params) {
    //   pros::Task* temp = (pros::Task*)params;
    //   if (temp->get_state() != pros::E_TASK_STATE_DELETED) temp->remove();
    //   delete temp;
    //   temp = nullptr;
    //   pros::delay(10);
    // }, (void*)this->task);
    delete this->task;
    this->task = nullptr;
    log_ln(PROGRAM_FLOW, "Stopped %s task from task end (shouldn't get here lol)", (this->task_name).c_str());
  }
}

bool pilons::Task::running() {
  return !(this->task == nullptr || this->task->get_state() == pros::E_TASK_STATE_DELETED);
}

void pilons::Task::stop_all_tasks() {
  for (int i = 0; i < pilons::Task::number_of_tasks; i++) {
    pilons::Task::tasks[i]->stop_task();
  }
  log_ln(PROGRAM_FLOW, "Stopped all tasks!");
}

int pilons::Task::get_state() {
  return (int)this->task->get_state();
}
