#pragma once
#include "main.h"

void drive_set(int x, int y, int a);
void drive_set(int pow);
void drive_set_vel(int x, int y, int a);
void drive_set_vel(int vel);
void drive_init();
void drive_handle();
