#pragma once
#include "main.h"
#include "subsystem.hpp"
#include "stdarg.h"

class Macro {
private:
  pros::Task* macro_task = nullptr;
  void* macro_function = nullptr;
  void* completed_function = nullptr;
  Macro* parent_macro = nullptr;
  bool temporary = true;

  static Macro* macros[256];
  static uint8_t number_of_macros;

public:
  /*
  What is needed when a macro is created?
   * What to do during the function
   * What to do when the function ends (finished as it was supposed to)
   * What to do when the function is interrupted (did not finish as it was supposed to)
   */
  Macro(void* macro_function, void* completed_function = nullptr, bool temporary = true);

  void run();
  void stop();


  static void stop_all();
};
