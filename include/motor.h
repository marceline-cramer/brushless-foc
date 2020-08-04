#pragma once

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include "global_settings.h"

typedef struct motor_state
{
    /* PWM outputs */
    struct device *poles[3];

    /* encoder info */
    int32_t encoder_position;
    int32_t last_position;
    uint32_t counts_per_revolution;

    /* driver info */
    float current_draw;

    /* thread info */
    k_thread_stack_t foc_stack_array[FOC_STACK_SIZE];
    struct k_stack foc_stack;
    struct k_thread foc_thread;
    k_tid_t foc_tid;

    /* synchronization info */
    struct k_mutex in_use;
} motor_state;

void motor_start(struct motor_state*);