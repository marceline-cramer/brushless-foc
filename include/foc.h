#pragma once

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include "global_settings.h"
#include "motor.h"

void foc_start(struct motor_state*);
void foc_entry(void*, void*, void*);