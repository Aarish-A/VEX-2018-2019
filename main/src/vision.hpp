#pragma once
#include "main.h"
#include "config.hpp"
#include "logs.hpp"

// filter
//
pros::vision_object_s_t object_buffer_array[10];
int buffer_index = 0;

void vison_init();
void vision_handle();
bool detecting_signature(short signature);
