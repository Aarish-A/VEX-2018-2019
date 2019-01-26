#pragma once
#include "main.h"
#include "config.hpp"
#include "logs.hpp"

extern pros::vision_object_s_t object_buffer_array[10];
extern int buffer_index;

void vison_init();
void vision_handle();
bool detecting_signature(short signature);
