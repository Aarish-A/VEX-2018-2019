#include "macro.hpp"

Macro* macros[256];
uint8_t number_of_macros = 0;

Macro::Macro(void* macro_function, void* completed_function, bool temporary) : macro_function(macro_function), completed_function(completed_function), temporary(temporary) {
  Macro::macros[number_of_macros] = this;
  Macro::number_of_macros++;
}

void Macro::run() {
  if (macro_task != nullptr) this->stop();
  macro_task = new pros::Task(this->macro_function);
  pros::Task* macro_task = nullptr;
}

void Macro::stop() {
  if (macro_task != nullptr) {
    macro_task->remove();
    delete macro_task;
    macro_task = nullptr;
  }
}

void Macro::stop_all() {
  for (int i = 0; i < number_of_macros; i++) {
    Macro::macros[i]->stop();
  }
}
