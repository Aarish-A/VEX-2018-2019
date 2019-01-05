#ifndef ANGLER_H
#define ANGLER_H
#include "main.h"

void angler_init();
void angler_set(int power);
void angler_move(double position, int32_t velocity = 200);
void angler_cal();
void angler_handle();

#endif
